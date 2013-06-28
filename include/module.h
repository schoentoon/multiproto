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

#ifndef _MODULE_H
#define _MODULE_H

#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef void* mod_create_context();
typedef void mod_free_context(void* context);
typedef int mod_match_function(unsigned char* data, size_t length);
typedef int mod_log_function(char* format, unsigned char* data, size_t length, char* buf, size_t buflen);

struct module {
  void* handle;
  void* context;
  mod_match_function* matcher;
  char* address;
  char* logfile;
  char* logformat;
  mod_log_function* log_function;
  uint16_t port;
  struct module* next;
};

struct module* new_module(char* filename);

void free_module(struct module* module);

#endif //_MODULE_H