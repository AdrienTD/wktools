// wkanima - Analyses Warrior Kings' anim3 files
// (C) 2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef signed char schar;
typedef signed short sshort;
typedef signed int sint;

FILE *f;
uint nverts;

void ferr(char *str)
{
	printf("Error: %s\n", str);
	exit(-1);
}

uchar read8() {uchar a; fread(&a, 1, 1, f); return a;}
ushort read16() {ushort a; fread(&a, 2, 1, f); return a;}
uint read32() {uint a; fread(&a, 4, 1, f); return a;}
float readfloat() {float a; fread(&a, 4, 1, f); return a;}

uchar br_b, br_c = 8;
void reinitbitread() {br_c = 8;}
uchar readbit()
{
	if(br_c >= 8)
	{
		br_b = read8();
		br_c = 0;
	}
	return br_b & (1 << br_c++);
}
uchar readbits(uint n)
{
	uint c = 0; uint i;
	for(i = 0; i < n; i++)
		if(readbit())
			c |= 1 << i;
	return c;
}

uint al4(uint n)
{
	return (n & (~3)) + ((n & 3) ? 4 : 0);
}

void PrintVertices()
{
	uint i, j; float a, m;
	printf("-----\n");
	printf("Add: %f\n", a = readfloat());
	printf("Mul: %f\n", m = readfloat());
	//for(i = 0; i < nverts; i++)
	//	printf("* %f\n", ((float)readbits(1)/1.0f) * m + a);
	///fseek(f, al4(nverts * 4 / 3), SEEK_CUR);
	for(i = 0; i < al4(nverts * 4 / 3) / 4; i++)
	{
		uint w = read32();
		for(j = 0; j < 3; j++)
			printf("* %f (%u%u)\n", ((float)((w>>(j*11))&1023)/1023.0f) * m + a , (w&1024)?1:0, (w&2097152)?1:0 );
	}
}

void PrintCoordinateStruct()
{
	int i, nf;
	printf("Num. frames: %u\nFrames:", nf = read32());
	for(i = 0; i < nf; i++)
		printf(" %u", read32());
	printf("\n");
	for(i = 0; i < nf; i++)
		{PrintVertices(); /*ferr("Stop here.");*/}
}

int main(int argc, char *argv[])
{
	uchar c; uint i, j, k, nf, nap;

	if(argc < 2) ferr("Please also type the file you want to open as an argument.");
	f = fopen(argv[1], "rb"); if(!f) ferr("File couldn't be opened.");

	printf("Format: ");
	for(i = 0; i < 4; i++) printf("%c", read8());
	printf("\n");
	printf("Version: %u\n", read32());
	printf("Mesh3 file: ");
	while(c = read8()) printf("%c", c);
	printf("\n");
	printf("Duration: %u ms\n", read32());
	printf("Sphere:");
	for(i = 0; i < 4; i++) printf(" %f", readfloat());
	printf("\n");
	printf("Num. vertices: %u\n", nverts = read32());

	for(i = 0; i < 3; i++)
	{
		printf("\n--- Coordinate %c ---\n", 'X' + i);
		PrintCoordinateStruct();
	}

	printf("\n--- Attachment points ---\nNumber of attachment points: %u\n", nap = read32());
	for(k = 0; k < nap; k++)
	{
		printf("--- AP %u ---\n", k);
		printf("Num. frames: %u\nFrames:", nf = read32());
		for(i = 0; i < nf; i++)
			printf(" %u", read32());
		printf("\n");
		for(i = 0; i < nf; i++)
		{
			printf("- Frame %u\n  Offset:", i);
			for(j = 0; j < 3; j++)
				printf(" %f", readfloat());
			printf("\n  Orientation:");
			for(j = 0; j < 4; j++)
				printf(" %f", readfloat());
			printf("\n  On: %u\n", read8());
		}
	}

	printf("\nEnded at: 0x%X\n", ftell(f));
}