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

#include "module.h"

#include "debug.h"

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

struct module* new_module(char* filename) {
  void* handle = dlopen(filename, RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    return NULL;
  }
  mod_match_function* matcher = dlsym(handle, "matchData");
  if (!matcher) {
    fprintf(stderr, "ERORR: %s doesn't have the function matchData()\n\t%s\n", filename, dlerror());
    dlclose(handle);
    return NULL;
  }
  struct module* module = malloc(sizeof(struct module));
  memset(module, 0, sizeof(struct module));
  module->handle = handle;
  module->matcher = matcher;
  mod_create_context* create_context = dlsym(handle, "createContext");
  if (create_context) {
    module->context = create_context();
    mod_free_context* free_context = dlsym(handle, "freeContext");
    if (!free_context)
      fprintf(stderr, "WARNING, you seem to have a createContext() function but no freeContext() function, you're possibly leaking memory.\n");
  }
  return module;
};

void free_module(struct module* module) {
  if (module) {
    if (module->context) {
      mod_free_context* free_context = dlsym(module->handle, "freeContext");
      free_context(module->context);
    }
    dlclose(module->handle);
    free(module);
  }
};