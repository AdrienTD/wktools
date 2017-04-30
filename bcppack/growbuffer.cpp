// bcppack - BCP packer
// (C) 2016 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include "global.h"

char *GrowBuffer::addSize(int ds)
{
	char *o;
	while((apnt + ds) > memsize)
	{
		char *nm;
		nm = (char*)realloc(memory, memsize + GROWBUF_ALLOC_STEP);
		if(!nm) ferr("Failed to reallocate GrowBuffer for adding data.");
		memset(nm + memsize, 0, GROWBUF_ALLOC_STEP);
		memory = nm; memsize += GROWBUF_ALLOC_STEP;
	}
	o = memory + apnt;
	apnt += ds;
	if(apnt & (align-1))
		apnt = (apnt & (~(align-1))) + align;
	return o;
}

/*void GrowBuffer::addData(char *data, int ds)
{
	memcpy(addSize(ds), data, ds);
}*/

void GrowBuffer::clear()
{
	if(!apnt) return;
	free(memory);
	memory = (char*)malloc(memsize = GROWBUF_ALLOC_STEP);
	if(!memory) ferr("Failed to clear GrowBuffer.");
	apnt = 0;
}
