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
    if (major >= 1 && minor >= 0) {
      if (strcmp(buf, "GET") == 0
        || strcmp(buf, "POST") == 0
        || strcmp(buf, "PUT") == 0
        || strcmp(buf, "DELETE") == 0
        || strcmp(buf, "PATCH") == 0
        || strcmp(buf, "HEAD") == 0
        || strcmp(buf, "CONNECT") == 0
        || strcmp(buf, "TRACE") == 0
        || strcmp(buf, "OPTIONS") == 0)
        return 1;
    }
  }
  return 0;
}

unsigned char startsWith(char* str, char* start) {
  size_t str_len = strlen(str);
  size_t start_len = strlen(start);
  size_t lowest = (str_len < start_len) ? str_len : start_len;
  int i;
  for (i = 0; i < lowest; i++) {
    if (str[i] != start[i])
      return 0;
  };
  return 1;
};

int logData(char* format, unsigned char* data, size_t length, char* buf, size_t buflen) {
  char reqbuf[65];
  char request[1025];
  if (sscanf((char*) data, "%64s %1024[^ ] HTTP/", reqbuf, request) == 2) {
    char* s = buf;
    for (;*format != '\0'; format++) {
      if (*format == '%') {
        format++;
        if (startsWith(format, "request")) {
          format += 6;
          char* c = reqbuf;
          while (*c != '\0')
            *buf++ = *c++;
        } else if (startsWith(format, "page")) {
          format += 3;
          char* c = request;
          while (*c != '\0')
            *buf++ = *c++;
        }
      } else
        *buf++ = *format;
    }
    return buf - s;
  }
  return 0;
}
