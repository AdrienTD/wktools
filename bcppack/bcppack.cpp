// bcppack - BCP packer
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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
extern "C" {
#include "lzrw_headers.h"
}

#define MEM_REQ ( 4096*sizeof(char*) + 16 )

struct BCPFile
{
	uint id; FILETIME ft;
	char *name;
};

struct BCPDirectory
{
	GrowList<BCPDirectory> *dirs;
	GrowList<BCPFile> *files;
	char *name;
	BCPDirectory *parent;
};

struct fileentry
{
	uint offset, endos, size, unk, form;
};

BCPDirectory bcproot; int bcpver = 0;
GrowList<fileentry> fent;
char *cwork;

char cpot[32*1024*1024];

FILE *fo;
void writeChar(char c) {fwrite(&c, 1, 1, fo);}
void writeShort(short c) {fwrite(&c, 2, 1, fo);}
void writeInt(int c) {fwrite(&c, 4, 1, fo);}
void writeFloat(float c) {fwrite(&c, 4, 1, fo);}
void writeString(char *c)
{
	int l = strlen(c);
	writeChar(l);
	if(bcpver < 2)
	{
		for(int i = 0; i < l; i++)
			writeChar(c[i]);
	}
	else
	{
		for(int i = 0; i < l; i++)
			{writeChar(c[i]); writeChar(0);}
	}
}

void ferr(char *s)
{
	printf("Error: %s\n", s);
	exit(-1);
}

void LookAtDir(BCPDirectory *bd, char *name, char *path)
{
	bd->dirs = new GrowList<BCPDirectory>;
	bd->files = new GrowList<BCPFile>;
	bd->name = strdup(name);

	WIN32_FIND_DATA fd; char fs[1024];
	strcpy(fs, path);
	strcat(fs, "\\*");
	HANDLE hd = FindFirstFile(fs, &fd);
	if(hd == INVALID_HANDLE_VALUE) return;

	do {
		if((!strcmp(fd.cFileName, ".")) || (!strcmp(fd.cFileName, "..")))
			continue;
		char nd[1024];
		strcpy(nd, path); strcat(nd, "\\"); strcat(nd, fd.cFileName);
		printf("%s", fd.cFileName);
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			printf(" (dir)\n");
			BCPDirectory *nb = bd->dirs->addp();
			nb->parent = bd;
			LookAtDir(nb, fd.cFileName, nd);
		}
		else
		{
			BCPFile *bf = bd->files->addp();
			bf->id = fent.len;
			bf->name = strdup(fd.cFileName);
			bf->ft = fd.ftLastWriteTime;

			fileentry *fe = fent.addp();
			fe->offset = ftell(fo);

			FILE *c = fopen(nd, "rb");
			fseek(c, 0, SEEK_END);
			fe->size = ftell(c);
			if(!fe->size)
			{
				fe->offset = -1;
				fe->endos = -1;
				fe->size = 0;
				fe->unk = 0;
				fe->form = 2;
			}
			else
			{
				fseek(c, 0, SEEK_SET);
				char *mr = (char*)malloc(fe->size);
				fread(mr, fe->size, 1, c);
				fclose(c);

				int cpsize = sizeof(cpot);
				compress(COMPRESS_ACTION_COMPRESS, (uchar*)cwork, (uchar*)mr, fe->size, (uchar*)cpot, (uint*)&cpsize);
				if(cpsize < fe->size)
					{fe->form = 4; fwrite(cpot, cpsize, 1, fo);}
				else
					{fe->form = 1; fwrite(mr, fe->size, 1, fo);}
				free(mr);
				fe->endos = ftell(fo);
			}
		}
		printf("\n");
	} while(FindNextFile(hd, &fd));
	printf("(end of LookAtDir)\n");
}

void PrintDir(BCPDirectory *bd)
{
	for(int i = 0; i < bd->files->len; i++)
	{
		BCPFile *f = bd->files->getpnt(i);
		printf("%s\n", f->name);
	}
	for(int i = 0; i < bd->dirs->len; i++)
	{
		BCPDirectory *d = bd->dirs->getpnt(i);
		printf("%s (dir)\n", d->name);
		PrintDir(d);
	}
}

void WriteDir(BCPDirectory *bd, int isroot)
{
	if(!isroot) writeString(bd->name);
	writeInt(bd->files->len);
	for(int i = 0; i < bd->files->len; i++)
	{
		BCPFile *e = bd->files->getpnt(i);
		writeInt(e->id);
		fwrite(&(e->ft), 8, 1, fo);
		writeString(e->name);
	}
	writeInt(bd->dirs->len);
	for(int i = 0; i < bd->dirs->len; i++)
	{
		BCPDirectory *e = bd->dirs->getpnt(i);
		WriteDir(e, 0);
	}
}
	

int main(int argc, char *argv[])
{
	if(argc < 4) {printf("bcppack - BCP packer\n(C) 2016 Adrien Geets\nReleased under the terms of the GPL3. See LICENSE for more information.\n\nUsage: bcppack \"path/to/directory/to/pack\" \"output.bcp\" \"version\"\nVersion can be either 0 for WK 1.0-1.3, or 2 for WK 1.4 and WKBattles\nFor example: bcppack mygame/saved mybcp.bcp 2\n"); return -2;}
	bcpver = atoi(argv[3]);
	cwork = (char*)malloc(MEM_REQ);

	fo = fopen(argv[2], "w+b");
	if(!fo) ferr("Cannot open output file.");
	if(bcpver < 2)
	{
		char h[] = "PAK File 0.06 (c) Black Cactus Games Limited ";
		char *p = h;
		while(*p) writeChar(*(p++));
		writeChar(0);
	}
	else
	{
		char h[] = "PAK File 2.01 (c) Black Cactus Games Limited ";
		char *p = h;
		while(*p) writeChar(*(p++));
		writeChar(-1);
	}
	for(int i = 0; i < 10; i++) writeChar(0);

	bcproot.parent = 0;
	LookAtDir(&bcproot, "(root)", argv[1]);

	printf("-----------\n");
	PrintDir(&bcproot);

	int fentoff = ftell(fo);
	writeInt(fent.len);
	for(int i = 0; i < fent.len; i++)
	{
		fileentry *e = fent.getpnt(i);
		writeInt(e->offset);
		writeInt(e->endos);
		writeInt(e->size);
		writeInt(0);
		writeInt(e->form);
	}
	WriteDir(&bcproot, 1);

	int bcpsize = ftell(fo);
	fseek(fo, 0x30, SEEK_SET);
	writeInt(fentoff);
	writeInt(bcpsize - fentoff);

	return 1;
}