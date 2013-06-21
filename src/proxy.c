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

struct proxy_connection* new_proxy(struct listener* listener) {
  struct proxy_connection* output = malloc(sizeof(struct proxy_connection));
  bzero(output, sizeof(struct proxy_connection));
  output->listener = listener;
  return output;
}

void preproxy_readcb(struct bufferevent* bev, void* context) {
  struct listener* listener = context;
  struct evbuffer* input = bufferevent_get_input(bev);
  size_t length = evbuffer_get_length(input);
  unsigned char buffer[length + 1];
  if (evbuffer_copyout(input, buffer, length) == length) {
    buffer[length] = '\0';
    struct module* m = listener->modules;
    while (m) {
      if (m->matcher(buffer, length)) {
        DEBUG(255, "I should be connecting to port %hd now.", m->port);
        break;
      }
      m = m->next;
    }
  }
}

void proxy_readcb(struct bufferevent* bev, void* context) {
}

void proxy_eventcb(struct bufferevent* bev, short events, void* context) {
}