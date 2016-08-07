// obj2wkm - OBJ to MESH3 file converter for WK (Battles)
// Copyright (C) 2016 Adrien Geets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "global.h"

struct Position {float x, y, z;};
struct TexCoord {float x, y;};
struct Triangle {int p[3]; int t[3];};
struct Material {/*boolean alpha;*/ char *name, *file;};
struct Group {int mat; GrowList<Triangle> *tri;};

GrowList<Position> lPos;
GrowList<TexCoord> lTex;
GrowList<Material> lMat;
GrowList<Group> lGrp;
int totTri = 0;

FILE *fo;

void writeChar(char c) {fwrite(&c, 1, 1, fo);}
void writeShort(short c) {fwrite(&c, 2, 1, fo);}
void writeInt(int c) {fwrite(&c, 4, 1, fo);}
void writeFloat(float c) {fwrite(&c, 4, 1, fo);}
void writeString(char *c) {fwrite(c, strlen(c)+1, 1, fo);}

void LoadFile(char *name, char **buf, int *size)
{
	FILE *f;
	f = fopen(name, "rb");
	if(!f) ferr("failed to open file");
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*buf = (char*)malloc(*size + 1);
	if(!*buf) ferr("failed to alloc for file");
	fread(*buf, *size, 1, f);
	(*buf)[*size] = 0;
	fclose(f);
}

char *ReadObjString(char **w, int nw)
{
	char fn[1024];
	strcpy(fn, w[0]);
	for(int i = 1; i < nw; i++)
	{
		strcat(fn, " ");
		strcat(fn, w[i]);
	}
	return strdup(fn);
}

void ReadMtlLib(char *name)
{
	char *fcnt; int fsize;
	LoadFile(name, &fcnt, &fsize);
	char *fp = fcnt;
	char wwl[MAX_LINE_SIZE], *word[MAX_WORDS_IN_LINE]; int nwords;
	int curmat = -1;
	while(*fp)
	{
		fp = GetLine(fp, wwl);
		nwords = GetWords(wwl, word);
		if(!nwords) continue;
		if((word[0])[0] == '#') continue;
		else if(!stricmp(word[0], "newmtl"))
		{
			Material *m = lMat.addp();
			curmat = lMat.len - 1;
			m->name = strdup(word[1]);
			m->file = 0;
		}
		else if(!stricmp(word[0], "map_Kd"))
		{
			if(curmat == -1) continue;
			lMat.getpnt(curmat)->file = ReadObjString(word+1, nwords-1);
		}
	}
	free(fcnt);
}

void ReadVertex(char *w, int *tp, int *tt)
{
	char *dc = strdup(w);
	char *p = strchr(dc, '/');
	if(p) *p = 0;
	*tp = atoi(dc) - 1;
	if(!p) return;
	char *o = p + 1;
	p = strchr(o, '/');
	if(p) *p = 0;
	*tt = atoi(o) - 1;
	free(dc);
}

int main(int argc, char *argv[])
{
	if(argc < 3) {printf("obj2wkm v0.1\n(C) 2016 Adrien Geets\nReleased under the terms of the GPL 3 license.\n\nUsage: obj2wkm [input.obj] [output.mesh3]\n"); return 0;}

	// Read the OBJ file.
	char *fcnt; int fsize;
	LoadFile(argv[1], &fcnt, &fsize);
	char *fp = fcnt;
	char wwl[MAX_LINE_SIZE], *word[MAX_WORDS_IN_LINE]; int nwords;
	Group *curgrp = 0;
	while(*fp)
	{
		fp = GetLine(fp, wwl);
		nwords = GetWords(wwl, word);
		if(!nwords) continue;
		if((word[0])[0] == '#') continue;
		else if(!stricmp(word[0], "mtllib"))
			ReadMtlLib(ReadObjString(word+1, nwords-1));
		else if(!stricmp(word[0], "v"))
		{
			Position *p = lPos.addp();
			p->x = atof(word[1]);
			p->y = atof(word[2]);
			p->z = atof(word[3]);
		}
		else if(!stricmp(word[0], "vt"))
		{
			TexCoord *t = lTex.addp();
			t->x = atof(word[1]);
			t->y = 1.0f - atof(word[2]);
		}
		else if(!stricmp(word[0], "f"))
		{
			if(!curgrp) ferr("There are faces without materials at the beginning!");
			if(nwords < 4) continue;
			int p0, p1, t0 = 0, t1 = 1;
			ReadVertex(word[1], &p0, &t0);
			ReadVertex(word[2], &p1, &t1);
			for(int i = 0; i < nwords-3; i++)
			{
				Triangle *t = curgrp->tri->addp();
				int p2, t2 = 2;
				ReadVertex(word[3+i], &p2, &t2);
				t->p[0] = p0; t->t[0] = t0;
				t->p[1] = p1; t->t[1] = t1;
				t->p[2] = p2; t->t[2] = t2;
				p1 = p2; t1 = t2;
			}
		}
		else if(!stricmp(word[0], "usemtl"))
		{
			// Determinate material index
			int mx = -1;
			for(int i = 0; i < lMat.len; i++)
				if(!stricmp(word[1], lMat.getpnt(i)->name))
					{mx = i; break;}
			if(mx == -1) ferr("usemtl with undeclared material");

			// Look if an associated group already exists.
			int ag = -1; Group *g;
			for(int i = 0; i < lGrp.len; i++)
				if(lGrp.getpnt(i)->mat == mx)
					{ag = i; break;}
			if(ag == -1)
			{
				g = lGrp.addp();
				g->mat = mx;
				g->tri = new GrowList<Triangle>;
			}
			else	g = lGrp.getpnt(ag);

			curgrp = g;
		}
	}
	free(fcnt);

	// Write the MESH3 file.
	fo = fopen(argv[2], "wb");
	if(!fo) ferr("failed to open output file");
	writeInt('hseM'); writeInt(3); writeInt(1); writeShort(0);

	// Positions
	writeShort(lPos.len);
	for(int i = 0; i < lPos.len; i++)
	{
		Position *p = lPos.getpnt(i);
		writeFloat(p->x); writeFloat(p->y); writeFloat(p->z);
	}

	// Sphere
	//writeFloat(1); writeFloat(1); writeFloat(1); writeFloat(1);
	writeFloat(0.349460f); writeFloat(6.346513f);
	writeFloat(1.568925f); writeFloat(8.845937f);

	// Remapper
	writeShort(lPos.len);
	for(int i = 0; i < lPos.len; i++)
		writeShort(i);

	// Normals
	writeShort(lPos.len);
	writeInt(lPos.len * 2);
	for(int i = 0; i < lPos.len; i++)
		{writeShort(1); writeShort(i);}

	// Materials
	writeShort(lGrp.len);
	for(int i = 0; i < lGrp.len; i++)
	{
		Material *m = lMat.getpnt(lGrp.getpnt(i)->mat);
		writeChar(1);
		writeString(m->file ? m->file : "Gold.tga");
	}

	// Texture coordinates
	writeInt(1);
	writeShort(lTex.len);
	for(int i = 0; i < lTex.len; i++)
	{
		TexCoord *t = lTex.getpnt(i);
		writeFloat(t->x); writeFloat(t->y);
	}

	// Groups
	writeInt(lGrp.len);
	for(int i = 0; i < lGrp.len; i++)
	{
		Group *g = lGrp.getpnt(i);
		writeShort(g->tri->len * 3);
		for(int j = 0; j < g->tri->len; j++)
		{
			Triangle *t = g->tri->getpnt(j);
			for(int k = 0; k < 3; k++)
				{writeShort(t->p[k]); writeShort(t->p[k]); writeShort(t->t[k]);}
		}
	}

	// Polygon list
	writeInt(1);
	writeShort(lPos.len); writeShort(lPos.len); writeShort(lTex.len);
	writeFloat(0);
	int gstart = 0;
	for(int i = 0; i < lGrp.len; i++)
	{
		Group *g = lGrp.getpnt(i);
		writeShort(g->tri->len);
		writeShort(g->tri->len * 3);
		writeShort(i);
		for(int j = 0; j < g->tri->len; j++)
		{
			writeShort((gstart+j)*3);
			writeShort((gstart+j)*3+1);
			writeShort((gstart+j)*3+2);
		}
		//gstart += g->tri->len;
	}

	fclose(fo);
	return 1;
}