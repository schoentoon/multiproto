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

#ifndef _PROXY_H
#define _PROXY_H

#include "listener.h"

struct proxy_connection {
  struct listener* listener;
  struct module* module;
  struct bufferevent* client;
  struct bufferevent* proxied_connection;
};

struct proxy_connection* new_proxy(struct listener* listener, struct bufferevent* bev);

void free_proxy_connection(struct proxy_connection* conn);

void preproxy_readcb(struct bufferevent* bev, void* context);

void proxy_readcb(struct bufferevent* bev, void* context);

void proxy_eventcb(struct bufferevent* bev, short events, void* context);

void proxied_conn_readcb(struct bufferevent* bev, void* context);

void proxied_conn_eventcb(struct bufferevent* bev, short events, void* context);

#endif //_PROXY_H