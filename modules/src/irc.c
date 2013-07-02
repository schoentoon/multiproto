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
  char* user = strstr((char*) data, "USER ");
  char u0[65], u1[65], u2[65], u3[65];
  if (user) {
    if (sscanf(user, "USER %64s \"%64[^\"]\" \"%64[^\"]\" :%64s\r\n", u0, u1, u2, u3) == 4)
      return 1;
    else if (sscanf(user, "USER %64s %64s %64s :%64[^\r\n]\r\n", u0, u1, u2, u3) == 4)
      return 1;
  }
  return 0;
}

size_t log_username(unsigned char* data, size_t length, char* buf, size_t buflen) {
  char* user = strstr((char*) data, "USER ");
  if (user && sscanf(user, "USER %64s \"%*[^\"]\" \"%*[^\"]\" :%*[^\r\n]\r\n", buf) == 1)
    return strlen(buf);
  return 0;
}

size_t log_realname(unsigned char* data, size_t length, char* buf, size_t buflen) {
  char* user = strstr((char*) data, "USER ");
  if (user) {
    if (sscanf(user, "USER %*s \"%*[^\"]\" \"%*[^\"]\" :%64[^\r\n]\r\n", buf) == 1)
      return strlen(buf);
    else if (sscanf(user, "USER %*s %*s %*s :%64[^\r\n]\r\n", buf) == 1)
      return strlen(buf);
  }
  return 0;
}