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

#include "debug.h"

#include <stdio.h>
#include <ctype.h>

/** I have no knowledge of how long this will keep working, but it is
 * based on a simple wireshark on a minecraft connection. Sort of looks
 * like this
00000000  02 3d 00 0a 00 73 00 63  00 68 00 6f 00 65 00 6e .=...s.c .h.o.e.n
00000010  00 74 00 6f 00 6f 00 6e  00 1d 00 31 00 39 00 32 .t.o.o.n ...1.9.2
00000020  00 2e 00 31 00 36 00 38  00 2e 00 32 00 2e 00 32 ...1.6.8 ...2...2
00000030  00 30 00 30 00 00 63 dd                          .0.0..c. 
 * In which schoentoon is the username and 192.168.2.200 is the ip address of
 * the server.
 */

int matchData(unsigned char* data, size_t length) {
  if (length >= 5) {
    if (data[0] == 0x02
      && data[1] == 0x3d
      && data[2] == 0x00
      && data[3] == 0x0a
      && data[4] == 0x00) {
      unsigned int i;
      for (i = 5; i < length; i++) {
        if (i % 2 == 1) {
          if (!isalnum(data[i]))
            return 0;
        } else if (data[i] != 0x00)
          return 0;
        else if (data[i] == 0x00 && data[i+1] == 0x1d)
          break;
      }
      return 1;
    }
  }
  return 0;
}