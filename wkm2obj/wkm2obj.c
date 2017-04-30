// wkm2obj - Converts a ".mesh3" file to ".obj" and ".mtl" files.
// (C) 2016 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>

#define printo(...) {fprintf(fobj, __VA_ARGS__); fflush(fobj);}
#define printm(...) fprintf(fmtl, __VA_ARGS__)

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#pragma pack(push, 1)
typedef struct
{
	ushort pos, norm, texc;
} mvertex;
#pragma pack(pop)

FILE *file = 0, *fobj = 0, *fmtl = 0; uint meshver;
uint nverts, nparts, ntexc, nuvlist, nnorm, nmat, ntrilist, ngrp, ngvertex, sgrp;
mvertex *verts = 0, *dpverts;
uint *tbgo;
uint reqtrilist = 0;
uint requvlist = 0;

char secret[] = {0xec,0xc9,0xdf,0xc4,0xc8,0xc3,0x8d,0xea,0xc8,0xc8,0xd9,0xde};

void cexit(int n)
{
	int i;
	if(verts) free(verts);
	if(fobj) fclose(fobj);
	if(fmtl) fclose(fmtl);
	if(file) fclose(file);
	exit(n);
}

void ferr(int n)
{
	printf("Error %i\n", n);
	cexit(n);
}

void warn(int n)
{
	printf("Warning (code %i), model may not be converted correctly.\n", n);
}

uchar readchar() {return fgetc(file);}
ushort readshort() {ushort a; fread(&a, 2, 1, file); return a;}
uint readint() {uint a; fread(&a, 4, 1, file); return a;}
void ignorestr() {while(fgetc(file));}
void readprintstr(FILE *out) {char a; while(a = fgetc(file)) fputc(a, out);}

char fno[256], fnm[256];
int main(int argc, char *argv[])
{
	int i, j, a, b, nni, n, m; uint f = 0.1f; float af;
	int dbga;

	f += 0.5f;
	if(argc < 3) {printf("mesh3 to obj+mtl converter by AdrienTD\nUsage: wkm2obj <input file.mesh3> <output name> [-u <number>] [-p <number>]\n-u: Use particular UV list (model color)\n-p: Use particular polygon list\nExamples: - wkm2obj input.mesh3 output\n            converts input.mesh3 to files output.obj and output.mtl.\n          - wkm2obj abc.mesh3 def -u 1\n            converts abc.mesh3 to files def.obj and def.mtl with blue color.\n"); return -1;}

	a = 0; b = 0;
	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'p':
					if((i + 1) >= argc) break;
					reqtrilist = atoi(argv[i+1]);
					i++; break;
				case 'u':
					if((i + 1) >= argc) break;
					requvlist = atoi(argv[i+1]);
					i++; break;
				default:
					printf("Unknown parameter %c, ignoring it.\n", argv[i][1]); break;
			}
		}
		else
		{
			if(!a) a = i;
			else   b = i;
		}
	}
	if(!(a && b)) ferr(-7);

	file = fopen(argv[a], "rb");
	if(!file) ferr(-2);
	if(readint() != 'hseM') ferr(-1);
	strcpy(fno, argv[b]); strcpy(fnm, argv[b]);
	strcat(fno, ".obj"); strcat(fnm, ".mtl");
	fobj = fopen(fno, "w"); if(!fobj) ferr(-4);
	fmtl = fopen(fnm, "w"); if(!fmtl) ferr(-5);
	printo("mtllib %s\no wkobj\n", fnm);

	fseek(file, 4, SEEK_SET);
	meshver = readint();
	fseek(file, 0xC, SEEK_SET);
	nparts = readshort();
	printo("# %u particles:\n", nparts);
	for(i = 0; i < nparts; i++)
	{
		printo("# \"");
		readprintstr(fobj);
		printo("\", \"");
		fseek(file, 0x1D, SEEK_CUR);
		readprintstr(fobj);
		printo("\"\n");
	}
	//printo("# nverts @ 0x%X\n", ftell(file));

	nverts = readshort();
	for(i = 0; i < nverts; i++)
	{
		printo("\nv");
		for(j = 0; j < 3; j++)
		{
			f = readint();
			printo(" %f", *(float*)(&f));
		}
	}

	printo("\n# Sphere:");
	for(i = 0; i < 4; i++)
	{
		f = readint();
		printo(" %f", *(float*)(&f));
	}

	// Remapper
	if(meshver < 4)
	{
		i = (ushort)readshort();
		fseek(file, 2 * i, SEEK_CUR);
	}

	nnorm = readshort();
	fseek(file, readint() * 2, SEEK_CUR);

	nmat = readshort();
	printo("\n# %u materials:", nmat);
	for(i = 0; i < nmat; i++)
	{
		readchar();
		printm("\n"
"newmtl wkmat%u\n"
"Ns 96.078431\n"
"Ka 0.000000 0.000000 0.000000\n"
"Kd 0.640000 0.640000 0.640000\n"
"Ks 0.500000 0.500000 0.500000\n"
"Ni 1.000000\n"
"d 1.000000\n"
"illum 2\n"
"map_Kd ", i);
		readprintstr(fmtl);
	}

	nuvlist = readint();
	if(requvlist >= nuvlist) ferr(-8);
	for(i = 0; i < requvlist; i++)
		fseek(file, readshort()*4*2, SEEK_CUR);
	ntexc = readshort();
	for(i = 0; i < ntexc; i++)
	{
		printo("\nvt");
		for(j = 0; j < 2; j++)
		{
			f = readint();
			af = *(float*)(&f);
			if(j == 1) af = 1.0f - af; // Flip vertically (Y axis).
			printo(" %f", af);
		}
	}
	for(i = requvlist+1; i < nuvlist; i++)
		fseek(file, readshort()*4*2, SEEK_CUR);

	//printo("\n# ngrp @ 0x%X", ftell(file));
	ngrp = readint();
	if(ngrp != nmat) warn(2);

	tbgo = (uint*)malloc(ngrp * sizeof(uint));
	if(!tbgo) ferr(-6);
	a = ftell(file);
	ngvertex = 0;
	for(i = 0; i < ngrp; i++)
	{
		tbgo[i] = ngvertex;
		//printf("tbgo[i] = %i\n", tbgo[i]);
		j = readshort();
		//printf("Grp %i: size = %i\n", i, j);
		ngvertex += j;
		fseek(file, j*6, SEEK_CUR);
	}
	fseek(file, a, SEEK_SET); fflush(stdout);
	//printf("ngvertex = %i\n", ngvertex);
	dpverts = verts = (mvertex*)malloc(6*ngvertex);
	if(!verts) ferr(-3);
	for(i = 0; i < ngrp; i++)
	{
		j = readshort();
		//printf("Grp %i: size = %i\n", i, j); fflush(stdout);
		fread(dpverts, 6*j, 1, file);
		dpverts = (mvertex*)(((char*)dpverts) + 6*j);
	}

	//printo("\n# ntrilist @ 0x%X", ftell(file));
	ntrilist = readint();
	if(requvlist >= ntrilist) ferr(-9);

	//fseek(file, dbga, SEEK_SET);
	for(a = 0; a < ntrilist; a++)
	{
		//printo("\n# PL #%i @ 0x%X:", a, ftell(file));
		i = readshort(); j = readshort(); b = readshort();	// Polygon_List
		//printo(" Polygon_List: %u %u %u", i, j, b);
		f = readint();				// Sample_Distance (float)
		//printo(", Sample_Distance: %f", *(float*)(&f));
		for(n = 0; n < nmat; n++)
		{
			sgrp = readshort();			// Group_Size
			//printo(", Group_Size: %u", sgrp);
			readshort();				// Vertex_List_Length (num. vertices used)
			i = readshort();			// Material_Index
			//printo(", Material_Index: %u", i);
			if((n == 0) && (i != 0)) warn(1);
			if(a != reqtrilist)
				fseek(file, 3*2*sgrp, SEEK_CUR);
			else
			{
				printo(/*"\n# @ 0x%X"*/ "\nusemtl wkmat%u", /*ftell(file),*/ n);
				for(i = 0; i < sgrp; i++)
				{
					printo("\nf");
					for(j = 0; j < 3; j++)
					{
						m = readshort();
						printo(" %u/%u", verts[tbgo[n]+m].pos+1, verts[tbgo[n]+m].texc+1);
					}
				}
			}

			//if(n != nmat - 1) printo("\n#\t");
		}
	}

	cexit(0);
}