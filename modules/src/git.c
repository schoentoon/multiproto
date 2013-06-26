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

/*
00000000  30 30 33 66 67 69 74 2d  75 70 6c 6f 61 64 2d 70 003fgit- upload-p
00000010  61 63 6b 20 2f 73 63 68  6f 65 6e 74 6f 6f 6e 2f ack /sch oentoon/
00000020  6d 75 6c 74 69 70 72 6f  74 6f 2e 67 69 74 00 68 multipro to.git.h
00000030  6f 73 74 3d 67 69 74 68  75 62 2e 63 6f 6d 00    ost=gith ub.com.

00000000  30 30 34 34 67 69 74 2d  75 70 6c 6f 61 64 2d 70 0044git- upload-p
00000010  61 63 6b 20 2f 73 63 68  6f 65 6e 74 6f 6f 6e 2f ack /sch oentoon/
00000020  6d 75 6c 74 69 70 72 6f  74 6f 2e 67 69 74 00 68 multipro to.git.h
00000030  6f 73 74 3d 31 32 37 2e  30 2e 30 2e 31 3a 32 33 ost=127. 0.0.1:23
00000040  34 35 36 00                                      456.
 */

int matchData(unsigned char* data, size_t length) {
  static unsigned char HANDSHAKE[] = { 0x67, 0x69, 0x74, 0x2d, 0x75, 0x70
                                     , 0x6c, 0x6f, 0x61, 0x64, 0x2d, 0x70
                                     , 0x61, 0x63, 0x6b, 0x20, 0x2f };
  if (length >= 20) {
    if (data[0] == 0x30 && data[1] == 0x30) {
      size_t i;
      for (i = 0; i < 17; i++) {
        if (data[i+4] != HANDSHAKE[i])
          return 0;
      }
      return 1;
    }
  }
  return 0;
}