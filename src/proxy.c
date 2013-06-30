/*  multiproto
 *  Copyright (C) 2013  Toon Schoenmakers
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "proxy.h"

#include "debug.h"
#include "config.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <syslog.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

struct proxy_connection* new_proxy(struct listener* listener, struct bufferevent* bev) {
  struct proxy_connection* output = malloc(sizeof(struct proxy_connection));
  bzero(output, sizeof(struct proxy_connection));
  output->listener = listener;
  output->client = bev;
  return output;
}

void free_proxy_connection(struct proxy_connection* conn) {
  if (conn)
    free(conn);
}

void disconnect_after_write(struct bufferevent* bev, void* context) {
  struct evbuffer* output = bufferevent_get_output(bev);
  if (evbuffer_get_length(output) == 0)
    bufferevent_free(bev);
}

size_t build_log(struct module* module, char* buf, size_t buflen, unsigned char* data, size_t length, struct proxy_connection* proxy);

void preproxy_readcb(struct bufferevent* bev, void* context) {
  struct proxy_connection* proxy = context;
  struct listener* listener = proxy->listener;
  struct evbuffer* input = bufferevent_get_input(bev);
  size_t length = evbuffer_get_length(input);
  unsigned char buffer[length + 1];
  if (evbuffer_copyout(input, buffer, length) == length) {
    buffer[length] = '\0';
    struct module* m = listener->modules;
    while (m) {
      if (m->matcher(buffer, length)) {
        DEBUG(255, "Connecting to port %s:%hd now.", m->address, m->port);
        if (m->logfile && m->logformat) {
          if (m->logfile == SYSLOG) {
            char buf[BUFSIZ];
            bzero(buf, sizeof(buf));
            if (build_log(m, buf, sizeof(buf), buffer, length, proxy) > 0)
              syslog(LOG_INFO, "%s", buf);
          } else {
            FILE* f = NULL;
            if (m->logfile == STDERR)
              f = stderr;
            else if (m->logfile == STDOUT)
              f = stdout;
            else
              f = fopen(m->logfile, "a");
            if (f) {
              char buf[BUFSIZ];
              bzero(buf, sizeof(buf));
              if (build_log(m, buf, sizeof(buf), buffer, length, proxy) > 0) {
                fprintf(f, "%s\n", buf);
                fflush(f);
              }
              if (m->logfile != STDERR && m->logfile != STDOUT)
                fclose(f);
            }
          }
        }
        struct event_base* base = bufferevent_get_base(bev);
        proxy->proxied_connection = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
        struct timeval timeout = {10, 0};
        bufferevent_set_timeouts(proxy->proxied_connection, &timeout, NULL);
        bufferevent_socket_connect_hostname(proxy->proxied_connection, dns, AF_INET, m->address, m->port);
        bufferevent_setcb(proxy->proxied_connection, proxied_conn_readcb, NULL, free_on_disconnect_eventcb, proxy);
        bufferevent_enable(proxy->proxied_connection, EV_READ);
        bufferevent_setcb(bev, proxy_readcb, NULL, free_on_disconnect_eventcb, proxy);
        struct evbuffer* proxied_output = bufferevent_get_output(proxy->proxied_connection);
        bufferevent_read_buffer(bev, proxied_output);
        break;
      }
      m = m->next;
    }
  }
}

void proxy_readcb(struct bufferevent* bev, void* context) {
  struct proxy_connection* proxy = context;
  struct evbuffer* proxied_output = bufferevent_get_output(proxy->proxied_connection);
  bufferevent_read_buffer(bev, proxied_output);
}

void free_on_disconnect_eventcb(struct bufferevent* bev, short events, void* context) {
  if (!(events & BEV_EVENT_CONNECTED)) {
    if (context) {
      struct proxy_connection* proxy = context;
      struct bufferevent* to_disconnect_later = NULL;
      if (proxy->client == bev)
        to_disconnect_later = proxy->proxied_connection;
      else if (proxy->proxied_connection == bev)
        to_disconnect_later = proxy->client;
      bufferevent_free(bev);
      free_proxy_connection(proxy);
      if (to_disconnect_later) {
        struct evbuffer* output = bufferevent_get_output(to_disconnect_later);
        if (evbuffer_get_length(output) == 0)
          bufferevent_free(to_disconnect_later);
        else {
          bufferevent_setcb(to_disconnect_later, NULL, disconnect_after_write, free_on_disconnect_eventcb, NULL);
          bufferevent_enable(to_disconnect_later, EV_WRITE);
        }
      }
    } else
      bufferevent_free(bev);
  }
}

void proxied_conn_readcb(struct bufferevent* bev, void* context) {
  struct proxy_connection* proxy = context;
  struct evbuffer* client_output = bufferevent_get_output(proxy->client);
  bufferevent_read_buffer(bev, client_output);
}

size_t build_log(struct module* module, char* buf, size_t buflen, unsigned char* data, size_t length, struct proxy_connection* proxy) {
  char* s = buf;
  char* end = s + buflen;
  if (config->dateformat) {
    struct tm *nowtm;
    time_t now = time(NULL);
    nowtm = localtime(&now);
    strftime(buf, end - s, config->dateformat, nowtm);
    while (*buf++);
    *(buf-1) = ' ';
  }
  char* fmt;
  for (fmt = module->logformat;*fmt != '\0'; fmt++) {
    if (*fmt == '%') {
      fmt++;
      char key[33];
      if (sscanf(fmt, "%32[a-z]", key) == 1) {
        if (strcmp(key, "ip") == 0) {
          char *i = proxy->client_ip;
          while (*i && buf < end)
            *buf++ = *i++;
        } else {
          char funcname[37];
          if (snprintf(funcname, sizeof(funcname), "log_%s", key) > 0) {
            mod_log_function* key_func = dlsym(module->handle, funcname);
            if (key_func) {
              char valuebuf[1025];
              if (key_func(data, length, valuebuf, sizeof(valuebuf) - 1)) {
                char *p = valuebuf;
                while (*p && buf < end)
                  *buf++ = *p++;
              }
            }
          }
        }
        fmt += strlen(key) - 1;
      }
    } else if (buf < end)
      *buf++ = *fmt;
    else
      break;
  }
  return buf - s;
}