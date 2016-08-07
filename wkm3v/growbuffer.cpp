// wkm3v - Simple ".mesh3" model viewer
// (C) 2016 Adrien Geets
// Licensed under the MIT license (see license.txt for more information)

//#include "global.h"
#include <cstring>
#include <cstdlib>
#include "growbuffer.h"

void GrowBuffer::addData(char *data, int ds)
{
	while((apnt + ds) > memsize)
	{
		char *nm;
		nm = (char*)realloc(memory, memsize + GROWBUF_ALLOC_STEP);
		if(!nm) ferr("Failed to reallocate GrowBuffer for adding data.");
		memset(nm + memsize, 0, GROWBUF_ALLOC_STEP);
		memory = nm; memsize += GROWBUF_ALLOC_STEP;
	}
	memcpy(memory + apnt, data, ds);
	apnt += ds;
	if(apnt & (align-1))
		apnt = (apnt & (~(align-1))) + align;
}