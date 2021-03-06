// obj2wkm - OBJ to MESH3 file converter for WK (Battles)
// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

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
Position normaltable[256];

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
	if(argc < 3) {printf("obj2wkm v0.1\n(C) 2016 Adrien Geets\nReleased under the terms of the MIT license.\n\nUsage: obj2wkm [input.obj] [output.mesh3]\n"); return 0;}

	char *fcnt; int fsize;

	// Read wkNormalTable.bin
	LoadFile("wkNormalTable.bin", &fcnt, &fsize);
	memcpy(normaltable, fcnt, 256*12);
	free(fcnt);

	// Read the OBJ file.
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

	// Position remapper
	writeShort(lPos.len);
	for(int i = 0; i < lPos.len; i++)
		writeShort(i);

	// Normals
	GrowList<short> sl;
	for(int i = 0; i < lPos.len; i++)
	{
		int tind = -1;
		GrowList<int> ct;
		for(int j = 0; j < lGrp.len; j++)
		{
			Group *g = lGrp.getpnt(j);
			for(int k = 0; k < g->tri->len; k++)
			{
				tind++;
				Triangle *t = g->tri->getpnt(k);
				for(int l = 0; l < 3; l++)
					if(t->p[l] == i)
						{ct.add(tind); break;}
			}
		}
		if(ct.len == 0)
			ct.add(0);
			//ferr("There is a vertex that is not connected to any face. Please remove them.");
		sl.add(ct.len);
		for(int j = 0; j < ct.len; j++)
			sl.add(ct[j]);
	}
	writeShort(lPos.len);
	writeInt(sl.len);
	for(int i = 0; i < sl.len; i++)
		writeShort(sl[i]);

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

/*
	// Normals again
	for(int i = 0; i < lPos.len; i++)
		writeChar(0);
*/
	for(int i = 0; i < lPos.len; i++)
	{
		int ntf = 0;
		Vector3 nm(0, 0, 0);
		for(int j = 0; j < lGrp.len; j++)
		{
			Group *g = lGrp.getpnt(j);
			for(int k = 0; k < g->tri->len; k++)
			{
				Triangle *t = g->tri->getpnt(k);
				for(int l = 0; l < 3; l++)
					if(t->p[l] == i)
					{
						ntf++;
						Position *p0 = lPos.getpnt(t->p[0]);
						Position *p1 = lPos.getpnt(t->p[1]);
						Position *p2 = lPos.getpnt(t->p[2]);
						Vector3 v0(p0->x, p0->y, p0->z);
						Vector3 v1(p1->x, p1->y, p1->z);
						Vector3 v2(p2->x, p2->y, p2->z);
						Vector3 a = v1 - v0, b = v2 - v0;
						Vector3 cp, un;
						Vec3Cross(&cp, &a, &b);
						nm += cp.normal();
						break;
					}
			}
		}

		if(ntf == 0)
			writeChar(0);
		else
		{
			nm /= ntf;
			float s = -100; int best = 0;
			for(int j = 0; j < 256; j++)
			{
				Vector3 a(normaltable[j].x, normaltable[j].y, normaltable[j].z);
				float t = nm.dot(a);
				if(t > s) {s = t; best = j;}
			}
			writeChar(best);
		}
	}

	// Normal remapper
	writeShort(lPos.len);
	for(int i = 0; i < lPos.len; i++)
		writeShort(i);

	fclose(fo);
	return 1;
}