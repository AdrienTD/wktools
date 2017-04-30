// bcppack - BCP packer
// (C) 2016 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

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