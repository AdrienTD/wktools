// ltttview - Analyses LTTT (Landscape Texture Translation Table?) files from Warrior Kings
// (C) 2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef struct
{
	char *srcname; int srcpos;
	char *dstname; int dstpos;
} tte;

FILE *file;
char **srcname, **dstname;
uint nsrc, ndst;
tte *table; uint ne;

char read8() {char a; fread(&a, 1, 1, file); return a;}
short read16() {short a; fread(&a, 2, 1, file); return a;}
int read32() {int a; fread(&a, 4, 1, file); return a;}

void main()
{
	int i; char *p;

	file = fopen("input.lttt", "rb"); assert(file);
	assert(read32() == 'TTTL');
	assert(read32() == 1);

	ndst = (ushort)read16();
	dstname = malloc(sizeof(char*) * ndst);
	for(i = 0; i < ndst; i++)
	{
		p = dstname[i] = malloc(256);
		while(*(p++) = read8());
	}

	nsrc = (ushort)read16();
	srcname = malloc(sizeof(char*) * nsrc);
	for(i = 0; i < nsrc; i++)
	{
		p = srcname[i] = malloc(256);
		while(*(p++) = read8());
	}

	ne = read32();
	table = malloc(sizeof(tte) * ne);
	for(i = 0; i < ne; i++)
	{
		table[i].srcname = srcname[read16()];
		table[i].srcpos = read8();
		table[i].dstname = dstname[read16()];
		table[i].dstpos = read8();
	}

	fclose(file);

	printf("%u entries:\n", ne);
	for(i = 0; i < ne; i++)
	{
		printf("%s (%i,%i) -> %s (%i, %i)\n",
			table[i].srcname, table[i].srcpos & 3, table[i].srcpos >> 2,
			table[i].dstname, table[i].dstpos & 3, table[i].dstpos >> 2);
	}
}