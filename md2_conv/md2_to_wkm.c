// md2_to_wkm
// (C) 2016 Adrien Geets
// Licensed under the MIT license (see LICENSE.TXT file).

#include <stdio.h>
#include <math.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	unsigned int sig, ver;
	unsigned int skinw, skinh;
	unsigned int framesize;
	unsigned int nskins, nverts, ntc, ntris, nglcmds, nframes;
	unsigned int oskins, otc, otris, oframes, oglcmds, oend;
} md2_header;

typedef struct
{
	unsigned short v[3], tc[3];
} md2_tris;

typedef struct
{
	short u, v;
} md2_texcoord;

typedef struct
{
	unsigned char v[3], n;
} md2_vertex;

typedef struct
{
	float scale[3], trans[3];
	char name[16];
	//md2_vertex *verts;
} md2_frame;
#pragma pack(pop)

FILE *fi, *fo;
md2_header *md2;
uint reqframe = 0;
float reqytrans = 0, reqscale = 1;

void ferr(char *s)
{
	printf("Error: %s\n", s);
	exit(-1);
}

void write8(char c) {fwrite(&c, 1, 1, fo);}
void write16(short c) {fwrite(&c, 2, 1, fo);}
void write32(int c) {fwrite(&c, 4, 1, fo);}
void writefloat(float c) {fwrite(&c, 4, 1, fo);}

int main(int argc, char *argv[])
{
	int i, j; char *nob; md2_frame *mf; md2_vertex *mv, *v1; char *tfn; md2_texcoord *mtc, *tc1; md2_tris *mtr, *tr1;
	if(argc < 3) {printf("md2_to_wkm v0.1 by Adrien Geets\nUsage: md2_to_wkm input.md2 output.mesh3 (frame number) (texture file name) (y translation number) (scale number)\n"); return -2;}
	if(argc >= 4) reqframe = atoi(argv[3]);
	tfn = (argc >= 5) ? argv[4] : "Gold.tga";
	if(argc >= 6) reqytrans = atof(argv[5]);
	if(argc >= 7) reqscale = atof(argv[6]);

	fi = fopen(argv[1], "rb"); if(!fi) ferr("Failed to open input file.");
	fseek(fi, 0, SEEK_END);
	i = ftell(fi);
	fseek(fi, 0, SEEK_SET);
	md2 = (md2_header*)malloc(i); if(!md2) ferr("Failed to allocate memory for input md2 file.");
	fread(md2, i, 1, fi);
	fclose(fi);
	if(md2->sig != '2PDI') ferr("Input file is not a MD2 file.");
	if((reqframe < 0) || (reqframe >= md2->nframes)) ferr("Requested frame number doesn't exist.");
	fo = fopen(argv[2], "wb"); if(!fo) ferr("Failed to open output file.");

	mf = (md2_frame*)((char*)md2 + md2->oframes + (md2->framesize*reqframe));
	v1 = (md2_vertex*)((char*)mf + sizeof(md2_frame));
	tc1 = (md2_texcoord*)((char*)md2 + md2->otc);
	tr1 = (md2_tris*)((char*)md2 + md2->otris);

	write32('hseM'); write32(3); write32(1);

	// Attachment point
	write16(1);
	write32('\0_PS');
	writefloat(0); writefloat(0); writefloat(0);
	writefloat(1); writefloat(0); writefloat(0); writefloat(0);
	write8(0);
	write8(0);

	// Positions
	write16(md2->nverts);
	mv = v1;
	for(i = 0; i < md2->nverts; i++)
	{
		//for(j = 0; j < 3; j++)
		//	writefloat(mv->v[j] * mf->scale[j] + mf->trans[j]);
		writefloat((mv->v[1] * mf->scale[1] + mf->trans[1]) * reqscale);
		writefloat((mv->v[2] * mf->scale[2] + mf->trans[2] + reqytrans) * reqscale);
		writefloat(-(mv->v[0] * mf->scale[0] + mf->trans[0]) * reqscale);
		mv++;
	}

	// Sphere (from supply wagon)
	writefloat(0.471943f); writefloat(2.353645f); writefloat(-1.298914f); writefloat(3.881023f);

	// Remapper
	write16(md2->nverts);
	for(i = 0; i < md2->nverts; i++)
		write16(i);

	// Normals
	write16(md2->nverts);
	write32(md2->nverts*2);
	for(i = 0; i < md2->nverts; i++)
		{write16(1); write16(i);}

	// Materials
	write16(1);
	write8(1);
	for(i = 0; i < strlen(tfn); i++)
		write8(tfn[i]);
	write8(0);

	// Texture coordinates
	write32(1);
	write16(md2->ntc);
	mtc = tc1;
	for(i = 0; i < md2->ntc; i++)
	{
		writefloat((float)mtc->u / md2->skinw);
		writefloat((float)mtc->v / md2->skinh);
		mtc++;
	}

	// Group
	write32(1);
	write16(md2->ntris * 3);
	mtr = tr1;
	for(i = 0; i < md2->ntris; i++)
	{
		for(j = 0; j < 3; j++)
			{write16(mtr->v[j]); write16(mtr->v[j]); write16(mtr->tc[j]);}
		mtr++;
	}

	// Polygon list
	write32(1);
	write16(md2->nverts); write16(md2->nverts); write16(md2->ntc);
	writefloat(0);

	write16(md2->ntris);
	write16(md2->ntris * 3);
	write16(0);

	for(i = 0; i < md2->ntris; i++)
		{write16(i*3); write16(i*3+1); write16(i*3+2);}

	fclose(fo);
	return 0;
}