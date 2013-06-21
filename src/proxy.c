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

#include <stdlib.h>
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
  if (conn) {
    if (conn->client)
      bufferevent_free(conn->client);
    if (conn->proxied_connection)
      bufferevent_free(conn->proxied_connection);
    free(conn);
  }
}

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
    struct proxy_connection* proxy = context;
    free_proxy_connection(proxy);
  }
}

void proxied_conn_readcb(struct bufferevent* bev, void* context) {
  struct proxy_connection* proxy = context;
  struct evbuffer* client_output = bufferevent_get_output(proxy->client);
  bufferevent_read_buffer(bev, client_output);
}