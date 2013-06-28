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

#include "config.h"

#include "module.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <event2/bufferevent.h>

static struct config* config = NULL;

struct evdns_base* dns = NULL;

int parse_config(char* config_file) {
  FILE* f = fopen(config_file, "r");
  if (!f) {
    fprintf(stderr, "Error '%s' while opening '%s'\n", strerror(errno), config_file);
    return 1;
  }
  char line_buffer[BUFSIZ];
  config = malloc(sizeof(struct config));
  memset(config, 0, sizeof(struct config));
  struct listener* listener = NULL;
  struct module* module = NULL;
  unsigned int line_count = 0;
  while (fgets(line_buffer, sizeof(line_buffer), f)) {
    line_count++;
    if (strlen(line_buffer) == 1 || line_buffer[0] == '#')
      continue;
    char key[BUFSIZ];
    char value[BUFSIZ];
    if (sscanf(line_buffer, "%[a-z_] = %[^\t\n]", key, value) == 2) {
      if (strcmp(key, "listener") == 0) {
        listener = new_listener(value);
        module = NULL;
        if (listener) {
          if (!config->listeners)
            config->listeners = listener;
          else {
            struct listener* l = config->listeners;
            while (l->next)
              l = l->next;
            l->next = listener;
          }
        }
      } else if (listener && strcmp(key, "module") == 0) {
        module = new_module(value);
        if (module) {
          if (!listener->modules)
            listener->modules = module;
          else {
            struct module* m = listener->modules;
            while (m->next)
              m = m->next;
            m->next = module;
          }
        }
      } else if (listener && module && !module->address && strcmp(key, "address") == 0) {
        char address[129];
        uint16_t port;
        if (sscanf(value, "%128[^:]:%hd", address, &port) == 2) {
          module->address = strdup(address);
          module->port = port;
        }
      } else if (listener && module && !module->logfile && strcmp(key, "logfile") == 0) {
        if (strcmp(value, "stderr") == 0)
          module->logfile = STDERR;
        else {
          FILE* f = fopen(value, "a");
          if (f) {
            module->logfile = strdup(value);
            fclose(f);
          } else
            fprintf(stderr, "ERROR: %s\n", strerror(errno));
        }
      } else if (listener && module && !module->logformat && strcmp(key, "logformat") == 0)
        module->logformat = strdup(value);
    } else {
      fprintf(stderr, "Error on line %ud\tCould not be parsed correctly.\n", line_count);
      return 1;
    }
  };
  return 0;
};

int dispatch_config(struct event_base* event_base) {
  if (config->listeners) {
    struct listener* l = config->listeners;
    while (l) {
      if (initListener(event_base, l))
        return 1;
      l = l->next;
    }
  }
  return 0;
};