// bcma - Analyses BCM (Black Cactus Map) files
// (C) 2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>
#include <assert.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

FILE *file;
int mapw, maph, nlakes, nnames, nids, ngrpbits, nidbits, nunk;

int curbit = 8, curbyte;
char readbit()
{
	if(curbit >= 8) {curbyte = fgetc(file); curbit = 0;}
	return (curbyte & (1 << curbit++)) ? 1 : 0;
}

/*
char read8() {char a; fread(&a, 1, 1, file); return a;}
short read16() {short a; fread(&a, 2, 1, file); return a;}
int read32() {int a; fread(&a, 4, 1, file); return a;}
float readfloat() {float a; fread(&a, 4, 1, file); return a;}
*/

char read8() {char a = 0; for(int i = 0; i < 8; i++) a |= readbit() << i; return a;}
short read16() {short a = 0; for(int i = 0; i < 16; i++) a |= readbit() << i; return a;}
int read32() {int a = 0; for(int i = 0; i < 32; i++) a |= readbit() << i; return a;}
float readfloat() {int a = read32(); return *(float*)(&a);}
int readnb(int n) {int a = 0; for(int i = 0; i < n; i++) a |= readbit() << i; return a;}

void ReadPrintWString()
{
	uint n = read16() / 2;
	for(int i = 0; i < n-1; i++)
		fputc(read16(), stdout);
	read16();
}

void ReadPrintName()
{
	uint n = read8();
	for(int i = 0; i < n; i++) fputc(read8(), stdout);
}

int GetMaxBits(int x)
{
	for(int i = 1; i < 32; i++)
		if((1 << i) > x) // > sign (not >=) intentional!
			return i;
	assert(0 & (int)"no max bits found");
	return 0;
}

int main(int argc, char *argv[])
{
	int i;
	assert(file = fopen((argc >= 2) ? argv[1] : "input.bcm", "rb"));
	fseek(file, 12, SEEK_CUR);
	mapw = read32(); maph = read32();
	printf("SCENARIO_DIMENSIONS %u %u\n", mapw, maph);
	printf("SCENARIO_EDGE_WIDTH %u\n", read32());
	printf("SCENARIO_TEXTURE_DATABASE \""); ReadPrintWString(); printf("\"\n");
	printf("SCENARIO_HEIGHT_SCALE_FACTOR %f\n", readfloat());
	printf("SCENARIO_SUN_COLOUR"); for(int i = 0; i < 3; i++) printf(" %u", read32()); printf("\n");
	printf("SCENARIO_SUN_VECTOR"); for(int i = 0; i < 3; i++) printf(" %f", readfloat()); printf("\n");
	printf("SCENARIO_FOG_COLOUR"); for(int i = 0; i < 3; i++) printf(" %u", read32()); printf("\n");
	printf("SCENARIO_SKY_TEXTURES_DIRECTORY \""); ReadPrintWString(); printf("\"\n");
	fseek(file, 0xC000, SEEK_CUR);

	printf("// Number of lakes: %u\n", nlakes = readnb(6));
	for(int i = 0; i < nlakes; i++)
	{
		printf("SCENARIO_LAKE");
		for(int j = 0; j < 3; j++) printf(" %f", readfloat());
		printf("\n");
		readnb(2);
	}
	printf("\n------------\n\n");

	printf("Number of names: %u\n", nnames = read16());
	for(int i = 0; i < nnames; i++) {printf(" - "); ReadPrintName(); printf("\n");}

	printf("Number of IDs: %u\n", nids = read16());
	for(int i = 0; i < nids; i++) printf(" - %u\n", read32());

	printf("\n------------\n\n");
	ngrpbits = GetMaxBits(nnames); //2;
	nidbits = GetMaxBits(nids); //3;

	for(int x = 0; x < mapw; x++)
	for(int z = maph-1; z >= 0; z--)
	{
		printf("X %u Z %u GROUP #%u ", x+1, z+1, readnb(ngrpbits));
		printf("ID #%u ", readnb(nidbits));
		printf("ROTATION %u ", readnb(2));
		printf("XFLIP %u ", readbit());
		printf("ZFLIP %u\n", readbit());
	}

	printf("End offset: 0x%08X\n", ftell(file));

	printf("\n------------\n\n");
	//read32(); read32(); readnb(24);

	printf("Num of ?: %u\n", nunk = read32());
	printf("0x62: 0x%X\n", read32());
	for(int i = 0; i < nunk; i++)
		{readnb(ngrpbits); readnb(nidbits);}

	for(int z = 0; z < maph+1; z++)
	{
		for(int x = 0; x < mapw+1; x++)
			printf("%02X ", (uchar)read8());
		printf("\n");
	}

	printf("End offset: 0x%08X\n", ftell(file));

	fclose(file);
}