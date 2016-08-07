// bcppack - BCP packer
// Copyright (C) 2016 Adrien Geets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <conio.h>
#include <io.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctype.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
#ifdef _WIN64
typedef unsigned __int64 ucpuint;
typedef signed __int64 scpuint;
#else
typedef unsigned int ucpuint;
typedef signed int scpuint;
#endif

void ferr(char *s);

#include "growbuffer.h"