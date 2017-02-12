// wkmtext - Analyses Warrior Kings' mesh3 files
// (C) 2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

FILE *fi;
uchar readChar() {return (uchar)fgetc(fi);}
ushort readShort() {ushort t; fread(&t, 2, 1, fi); return t;}
uint readInt() {uint t; fread(&t, 4, 1, fi); return t;}
float readFloat() {float t; fread(&t, 4, 1, fi); return t;}
char *readString(char *o) {char *p = o; while(*(p++) = fgetc(fi)); return o;}

void ferr(char *str)
{
	printf("Error: %s\n", str);
	exit(-1);
}

void readAndPrint3Shorts()
{
	int i;
	for(i = 0; i < 3; i++)
		printf(" %u", readShort());
}

int main(int argc, char *argv[])
{
	char tb[512];
	int i, j, k, n, m, ver, ngrps;

	if(argc < 2) {printf("Usage: wkmtext \"input.mesh3\"\n"); return -2;}
	fi = fopen(argv[1], "rb");
	if(!fi) ferr("cannot open file");

	*(int*)&(tb[0]) = readInt(); tb[4] = 0;
	printf("FORMAT %s\nVERSION %u\n", tb, ver = readInt());
	printf("\nFlags %i\n", readInt());

	printf("Attachment_Points %u\n{\n", n = readShort());
	for(i = 0; i < n; i++)
	{
		printf("Tag \"%s\"\n", readString(tb));
		for(j = 0; j < 0x1D; j++) readChar();
		printf("File \"%s\"\n", readString(tb));
	}
	printf("}\n");

	printf("\nVector_Length %u\n", n = readShort());
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < 3; j++)
			printf(" %f", readFloat());
		printf("\n");
	}

	printf("\nSphere");
	for(i = 0; i < 4; i++)
		printf(" %f", readFloat());
	printf("\n");

	if(ver < 4)
	{
		printf("\nRemapper %i\n", n = readShort());
		for(i = 0; i < n; i++)
			printf("\t%u\n", readShort());
	}

	printf("\nVertex_Normals %u\n", n = readShort());
	printf("\tNum_Indices %u\n", readInt());
	for(i = 0; i < n; i++)
	{
		printf("\t\tSurface_Indices %u", m = readShort());
		for(j = 0; j < m; j++)
			printf(" %u", readShort());
		printf("\n");
	}

	printf("Materials %u\n", n = readShort());
	for(i = 0; i < n; i++)
	{
		printf("\tMaterial %u", readChar());
		printf(" \"%s\"\n", readString(tb));
	}

	printf("\nNum_UV_Lists %u\n", n = readInt());
	for(i = 0; i < n; i++)
	{
		printf("\nVector_Length %u\n", m = readShort());
		for(j = 0; j < m; j++)
		{
			printf(" %f", readFloat());
			printf(" %f\n", readFloat());
		}
	}

	printf("\nNum_Groups %u\n", ngrps = n = readInt());
	for(i = 0; i < n; i++)
	{
		printf("\nVertex_List %u\n", m = readShort());
		for(j = 0; j < m; j++)
			{readAndPrint3Shorts(); printf("\n");}
	}

	printf("\nNum_Polygon_Lists %u\n", n = readInt());
	for(i = 0; i < n; i++)
	{
		printf("\n\tPolygon_List"); readAndPrint3Shorts(); printf("\n");
		printf("\tSample_Distance %f\n", readFloat());
		for(j = 0; j < ngrps; j++)
		{
			printf("\n\t\tGroup_Size %u\n", m = readShort());
			printf("\t\tVertex_List_Length %u\n", readShort());
			printf("\t\tMaterial_Index %u\n", readShort());
			for(k = 0; k < m; k++)
				{printf("\t\t\tVertex_Indices");
				readAndPrint3Shorts(); printf("\n");}
		}
	}

	fclose(fi);
	return 0;
}