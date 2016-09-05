// md2_to_wka
// (C) 2016 Adrien Geets
// Licensed under the MIT license (see LICENSE.TXT file).

#include <stdio.h>
#include <math.h>

#define MD2FPS 8
#define MD2DUR (1000/(MD2FPS))

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
int firstframe = 0, numframes = -1;
float reqytrans = 0, reqscale = 1;
int reqap = 1;

void ferr(char *s)
{
	printf("Error: %s\n", s);
	exit(-1);
}

void write8(char c) {fwrite(&c, 1, 1, fo);}
void write32(int c) {fwrite(&c, 4, 1, fo);}
void writefloat(float c) {fwrite(&c, 4, 1, fo);}

void WriteFrameTimes()
{
	int i;
	write32(numframes+1);
	for(i = 0; i < numframes+1; i++)
		write32(MD2DUR * i);
}

void WritePosFrame(int f, int c, float t, float s)
{
	uint i, n;
	md2_frame *m = (md2_frame*)((char*)md2 + md2->oframes + (md2->framesize*f));
	md2_vertex *v = (md2_vertex*)((char*)m + sizeof(md2_frame));
	writefloat((m->trans[c] + t) * s);
	writefloat(m->scale[c] * 255.0f * s);
	for(i = 0; i < md2->nverts / 3; i++)
	{
		n = 0;
		n |= v->v[c] << 2; v++;
		n |= v->v[c] << 13; v++;
		n |= v->v[c] << 24; v++;
		write32(n);
	}
	if(md2->nverts % 3)
	{
		n = 0;
		for(i = 0; i < md2->nverts % 3; i++)
			{n |= v->v[c] << (11*i+2); v++;}
		write32(n);
	}
}

void WriteCoordinateFrames(int c, float t, float s)
{
	int i;
	WriteFrameTimes();
	for(i = firstframe; i < firstframe+numframes; i++)
		WritePosFrame(i, c, t, s);
	WritePosFrame(firstframe, c, t, s);
}

void WritePositions()
{
	WriteCoordinateFrames(1, 0, reqscale);
	WriteCoordinateFrames(2, reqytrans, reqscale);
	WriteCoordinateFrames(0, 0, -reqscale);
}

void WriteNormalFrame(int f)
{
	int i;
	md2_frame *m = (md2_frame*)((char*)md2 + md2->oframes + (md2->framesize*f));
	md2_vertex *v = (md2_vertex*)((char*)m + sizeof(md2_frame));
	for(i = 0; i < md2->nverts; i++)
		{write8(v->n); v++;}
}

void WriteNormals()
{
	int i;
	WriteFrameTimes();
	write32(0/*md2->nverts*/);
	for(i = firstframe; i < firstframe+numframes; i++)
		WriteNormalFrame(i);
	WriteNormalFrame(firstframe);
}

void WriteAPState(int on)
{
	writefloat(0); writefloat(0); writefloat(0);
	writefloat(1); writefloat(0); writefloat(0); writefloat(0);
	write8(on);
}

int main(int argc, char *argv[])
{
	int i; char *nob;
	if(argc < 4) {printf("md2_to_wka v0.1 by Adrien Geets\nUsage: md2_to_wka input.md2 output.anim3 mesh.mesh3 (first frame number) (number of frames) (y translation number) (scale number)\n"); return -2;}
	if(argc >= 5) firstframe = atoi(argv[4]);
	if(argc >= 6) numframes = atoi(argv[5]);
	if(argc >= 7) reqytrans = atof(argv[6]);
	if(argc >= 8) reqscale = atof(argv[7]);
	//if(argc >= 9) reqap = atoi(argv[8]);

	fi = fopen(argv[1], "rb"); if(!fi) ferr("Failed to open input file.");
	fseek(fi, 0, SEEK_END);
	i = ftell(fi);
	fseek(fi, 0, SEEK_SET);
	md2 = (md2_header*)malloc(i); if(!md2) ferr("Failed to allocate memory for input md2 file.");
	fread(md2, i, 1, fi);
	fclose(fi);
	if(md2->sig != '2PDI') ferr("Input file is not a MD2 file.");
	if((firstframe < 0) || (firstframe >= md2->nframes)) ferr("First frame number doesn't exist.");
	if(numframes < 0) numframes = md2->nframes - firstframe;

	//nob = (char*)malloc(strlen(argv[2]) + 7);
	//strcpy(nob, argv[2]); strcat(nob, ".anim3");
	fo = fopen(argv[2], "wb"); if(!fo) ferr("Failed to open output file.");
	write32('minA'); write32(3);
	//strcpy(nob, argv[2]); strcat(nob, ".mesh3");
	for(i = 0; i < strlen(argv[3]); i++)
		write8((argv[3])[i]);
	write8(0); //free(nob);
	write32(MD2DUR * numframes);
	// Sphere from supply wagon
	writefloat(0.471943f); writefloat(2.353645f); writefloat(-1.298914f); writefloat(3.881023f);
	write32(md2->nverts);

	WritePositions();

	if(!reqap)
		write32(0);	// num. attachment points
	else {
		write32(1);
		write32(3); // 3 frames for ap
		write32(0); write32(MD2DUR * numframes / 2); write32(MD2DUR * numframes);
		WriteAPState(0); WriteAPState(1); WriteAPState(0);
	}

	WriteNormals();
	//write32(0); write32(0);

	fclose(fo);
	free(md2);
}