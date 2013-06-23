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

#include <stdio.h>
#include <ctype.h>
#include <string.h>

int matchData(unsigned char* data, size_t length) {
  char buf[65];
  int major, minor;
  if (sscanf((char*) data, "%64s / HTTP/%d.%d\r\n", buf, &major, &minor) == 3
    || sscanf((char*) data, "%64s /%*s HTTP/%d.%d\r\n", buf, &major, &minor) == 3) {
    if (major >= 1 && minor >= 1) {
      if (strcmp(buf, "GET") == 0 || strcmp(buf, "POST") == 0 || strcmp(buf, "PUT") == 0 || strcmp(buf, "DELETE") == 0 || strcmp(buf, "PATCH") == 0 || strcmp(buf, "HEAD") == 0 || strcmp(buf, "CONNECT") == 0 || strcmp(buf, "TRACE") == 0 || strcmp(buf, "OPTIONS") == 0)
        return 1;
    }
  }
  return 0;
}
