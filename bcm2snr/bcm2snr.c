// bcm2snr - Converts BCM files to SNR files
// (C) 2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

FILE *file, *fsnr, *fpcx, *ftrn;
int mapw, maph, nlakes, nnames, nids, ngrpbits, nidbits, nunk;

int curbit = 8, curbyte;
char readbit()
{
	if(curbit >= 8) {curbyte = fgetc(file); curbit = 0;}
	return (curbyte & (1 << curbit++)) ? 1 : 0;
}

char read8() {int i; char a = 0; for(i = 0; i < 8; i++) a |= readbit() << i; return a;}
short read16() {int i; short a = 0; for(i = 0; i < 16; i++) a |= readbit() << i; return a;}
int read32() {int i; int a = 0; for(i = 0; i < 32; i++) a |= readbit() << i; return a;}
float readfloat() {int a = read32(); return *(float*)(&a);}
int readnb(int n) {int i, a = 0; for(i = 0; i < n; i++) a |= readbit() << i; return a;}

void write8(FILE *f, uchar a) {fwrite(&a, 1, 1, f);}
void write16(FILE *f, ushort a) {fwrite(&a, 2, 1, f);}
void write32(FILE *f, uint a) {fwrite(&a, 4, 1, f);}

void ReadPrintWString(FILE *f)
{
	int i;
	uint n = read16() / 2;
	for(i = 0; i < n-1; i++)
		fputc(read16(), f);
	read16();
}

void ReadName(char *o)
{
	int i;
	uint n = read8();
	for(i = 0; i < n; i++) *(o++) = read8();
	*o = 0;
}

int GetMaxBits(int x)
{
	int i;
	for(i = 1; i < 32; i++)
		if((1 << i) > x) // > sign (not >=) intentional!
			return i;
	assert(0 & (int)"no max bits found");
	return 0;
}

void WritePCXHeader(FILE *f, int w, int h, int np)
{
	int i;
	write8(f, 10); write8(f, 5); write8(f, 1); write8(f, 8);
	write32(f, 0);
	write16(f, w-1); write16(f, h-1);
	write16(f, w); write16(f, h);
	for(i = 0; i < 12; i++) write32(f, 0);
	write8(f, 0);
	write8(f, np);
	write16(f, w + (w&1));
	write16(f, 1);
	write16(f, 0);
	for(i = 0; i < 14; i++) write32(f, 0);
}

void WritePCXData(FILE *f, uchar *pix, int w, int h, int np)
{
	int y, p, x;
	for(y = 0; y < h; y++)
	for(p = 0; p < np; p++)
	{
		uchar *sl = pix + y*w*np + p;
		uchar *d = sl;
		uchar o; // = *d;
		int cnt = 1;
		//d += np;
		for(x = 0; x < w-1; x++)
		{
			o = *d;
			d += np;
			if((*d != o) || (cnt >= 63))
			{
				if((cnt > 1) | (o >= 0xC0))
					write8(f, 0xC0 | cnt);
				write8(f, o);
				cnt = 1;
			}
			else cnt++;
		}
		if((cnt > 1) | (o >= 0xC0))
			write8(f, 0xC0 | cnt);
		write8(f, o);
		if(w & 1) write8(f, 0);
	}
}

int main(int argc, char *argv[])
{
	int i, j, x, z; char fnwoe[384], *e, *minimap, *hmap, **tname; int *tid;
	if(argc < 2) {printf("Usage: bcm2snr \"input.bcm\"\n"); return 0;}
	assert(file = fopen(argv[1], "rb"));

	fseek(file, 12, SEEK_CUR);
	mapw = read32(); maph = read32();

	strcpy(fnwoe, argv[1]);
	e = strrchr(fnwoe, '.');
	if(!e) {e = fnwoe + strlen(fnwoe); *e = '.';}

	strcpy(e, ".snr");
	assert(fsnr = fopen(fnwoe, "w"));
	fprintf(fsnr, "SCENARIO_VERSION 4.00\n");
	fprintf(fsnr, "SCENARIO_DIMENSIONS %u %u\n", mapw, maph);
	fprintf(fsnr, "SCENARIO_EDGE_WIDTH %u\n", read32());
	fprintf(fsnr, "SCENARIO_TEXTURE_DATABASE \""); ReadPrintWString(fsnr);
	fprintf(fsnr, "\"\n");
	strcpy(e, "");
	fprintf(fsnr, "SCENARIO_TERRAIN \"%s.trn\"\n", fnwoe);
	fprintf(fsnr, "SCENARIO_HEIGHTMAP \"%s_heightmap.pcx\"\n", fnwoe);
	fprintf(fsnr, "SCENARIO_HEIGHT_SCALE_FACTOR %f\n", readfloat());
	fprintf(fsnr, "SCENARIO_SUN_COLOUR");
	for(i = 0; i < 3; i++) fprintf(fsnr, " %u", read32());
	fprintf(fsnr, "\n");
	fprintf(fsnr, "SCENARIO_SUN_VECTOR");
	for(i = 0; i < 3; i++) fprintf(fsnr, " %f", readfloat());
	fprintf(fsnr, "\n");
	fprintf(fsnr, "SCENARIO_FOG_COLOUR");
	for(i = 0; i < 3; i++) fprintf(fsnr, " %u", read32());
	fprintf(fsnr, "\n");
	fprintf(fsnr, "SCENARIO_SKY_TEXTURES_DIRECTORY \""); ReadPrintWString(fsnr);
	fprintf(fsnr, "\"\n");
	fprintf(fsnr, "SCENARIO_MINIMAP \"%s_minimap.pcx\"\n", fnwoe);

	strcpy(e, "_minimap.pcx");
	assert(fpcx = fopen(fnwoe, "wb"));
	assert(minimap = (char*)malloc(0xC000));
	fread(minimap, 0xC000, 1, file);
	WritePCXHeader(fpcx, 128, 128, 3);
	WritePCXData(fpcx, minimap, 128, 128, 3);
	fclose(fpcx);
	free(minimap);

	nlakes = readnb(6);
	for(i = 0; i < nlakes; i++)
	{
		fprintf(fsnr, "SCENARIO_LAKE");
		for(j = 0; j < 3; j++) fprintf(fsnr, " %f", readfloat());
		fprintf(fsnr, " 0.0\n");
		readnb(2);
	}

	printf("Number of names: %u\n", nnames = read16());
	assert(tname = (char**)malloc(nnames * sizeof(char*)));
	for(i = 0; i < nnames; i++)
		{tname[i] = (char*)malloc(128); ReadName(tname[i]);}

	printf("Number of IDs: %u\n", nids = read16());
	assert(tid = (int*)malloc(nids * sizeof(int)));
	for(i = 0; i < nids; i++) tid[i] = read32();

	printf("\n------------\n\n");
	ngrpbits = GetMaxBits(nnames); //2;
	nidbits = GetMaxBits(nids); //3;

	strcpy(e, ".trn");
	assert(ftrn = fopen(fnwoe, "w"));
	for(z = maph-1; z >= 0; z--)
	for(x = 0; x < mapw; x++)
	{
		fprintf(ftrn, "X %u Z %u ", x+1, z+1);
		fprintf(ftrn, "GROUP \"%s\" ", tname[readnb(ngrpbits)]);
		fprintf(ftrn, "ID %u ", tid[readnb(nidbits)]);
		fprintf(ftrn, "ROTATION %u ", readnb(2));
		fprintf(ftrn, "XFLIP %u ", readbit());
		fprintf(ftrn, "ZFLIP %u\n", readbit());
	}
	fclose(ftrn);

	printf("End offset: 0x%08X\n", ftell(file));

	printf("\n------------\n\n");

	printf("Num of ?: %u\n", nunk = read32());
	printf("0x62: 0x%X\n", read32());
	for(i = 0; i < nunk; i++)
		{readnb(ngrpbits); readnb(nidbits);}

	assert(hmap = (char*)malloc((mapw+1)*(maph+1)));
	fread(hmap, (mapw+1)*(maph+1), 1, file);
	strcpy(e, "_heightmap.pcx");
	assert(fpcx = fopen(fnwoe, "wb"));
	WritePCXHeader(fpcx, mapw+1, maph+1, 1);
	WritePCXData(fpcx, hmap, mapw+1, maph+1, 1);
	write8(fpcx, 12);
	for(i = 0; i < 256; i++)
		for(j = 0; j < 3; j++)
			write8(fpcx, i);
	fclose(fpcx);
	free(hmap);

	printf("End offset: 0x%08X\n", ftell(file));

	fclose(file);
	fclose(fsnr);
	free(tid);
	for(i = 0; i < nnames; i++) free(tname[i]);
	free(tname);
}