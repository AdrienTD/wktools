// wkbmee - Enables the Map Editor button in WKB by patching the running process' code.
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

#include <stdio.h>
#include <windows.h>
#include <psapi.h>

//#define myprint(...) 
#define myprint printf

typedef unsigned char uchar;

DWORD pids[256], pts, nprocs;
char pname[256]; char *pfile;
HANDLE h;
void *vpage;
#define cpage ((unsigned char*)vpage)
#define ipage ((unsigned int*)vpage)

uchar bs1[] = {0x8B,0x8E,0x4C,0x01,0x00,0x00,0x53,0x89,0x86,0x54,0x01,0x00,0x00,0xE8};
uchar to1[] = {0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x89,0x86,0x54,0x01,0x00,0x00,0x90,0x90,0x90,0x90,0x90};
uchar bs2[] = "\\data\0\0";
uchar to2[] = "\\saved\0";
wchar_t bs3[] = L"\\maps\\Map_Textures\\%s";
wchar_t to3[] = L"Maps\\Map_Textures\\%s\0";
//wchar_t bs4[] = L"\\data";
//wchar_t to4[] = L"\\saved";

// "Adrien Geets", XOR 0xAD for every character
char secret[] = {0xec,0xc9,0xdf,0xc4,0xc8,0xc3,0x8d,0xea,0xc8,0xc8,0xd9,0xde};

char appname[] = "WKB Map Editor Enabler";

void ferr(int n)
{
	char s[256]; s[255] = 0;
	_snprintf(s, 255, "Error: %i\nOpen \"readme.txt\" for help.", n);
	MessageBox(0, s, appname, 16);
	exit(n);
}

int isout(int a, int b)
{
	__asm mov eax, a
	__asm add eax, b
	__asm setc al
	__asm movzx eax, al
}

char *buffind(char *bb, unsigned int bn, char *fb, unsigned int fn)
{
	char *bp, *fp;
	bp = bb; fp = fb;
	while(bp < (bb + bn))
	{
		if(*bp == *fp)
		{
			fp++;
			if(fp >= (fb + fn))
				return bp-fn+1;
		}
		else if(fp != fb)
			{fp = fb; continue;}
		bp++;
	}
	return 0;
}

void PrintMemBasicInfo(MEMORY_BASIC_INFORMATION *mbi)
{
	myprint("0x%08X, 0x%08X bytes, state: ", mbi->BaseAddress, mbi->RegionSize);
	switch(mbi->State)
	{
		case MEM_COMMIT:	myprint("committed"); break;
		case MEM_FREE:		myprint("free"); break;
		case MEM_RESERVE:	myprint("reserved"); break;
	}
	myprint(", ");
	switch(mbi->Type)
	{
		case MEM_IMAGE:		myprint("image"); break;
		case MEM_MAPPED:	myprint("mapped"); break;
		case MEM_PRIVATE:	myprint("private"); break;
	}
	if(mbi->Protect & 0xF0) myprint(", executable");
	myprint("\n");
}

void FindAndChange(char *sb, int sbn, char *rw, int rwn, int pmask)
{
	char *a = 0; MEMORY_BASIC_INFORMATION mbi; int pgs, bw; char *fp;
	while(1)
	{
		if(!VirtualQueryEx(h, a, &mbi, sizeof(mbi))) break;
		if((mbi.State == MEM_COMMIT) && (mbi.Protect & pmask) && (mbi.Type != MEM_MAPPED))
		{
			PrintMemBasicInfo(&mbi);
			if(mbi.RegionSize >= 64*1024*1024) {printf("TOO BIG!\n"); goto nextrg;}
			// Search for "hide editor button" code in the region and NOP it if found.
			vpage = malloc(mbi.RegionSize); if(!vpage) {myprint("M"); break;}
			pgs = 0;
			if(ReadProcessMemory(h, a, vpage, mbi.RegionSize, &pgs))
			{
				fp = buffind(cpage, pgs, sb, sbn);
				if(fp)
				{
					myprint("Found!\n");
					WriteProcessMemory(h, fp-vpage+a, rw, rwn, &bw);
					free(vpage);
					break;
				}
			}
			free(vpage);
		}
nextrg:		if(isout((unsigned int)mbi.BaseAddress, (unsigned int)mbi.RegionSize)) {myprint("W"); break;}
		a = (char*)mbi.BaseAddress + (unsigned int)mbi.RegionSize;
	}
}

//int main()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char *args, int showMode)
{
	int i; char *a = 0x00000000; int pgs, bw; char *fp;
	MEMORY_BASIC_INFORMATION mbi;

	MessageBox(0, "Welcome to wkbmee v0.2!\nThis trainer will enable the Map Editor button in the main menu.\nBe sure that WKB is running, then click OK.", appname, 64);

	// Search process
	if(!EnumProcesses(pids, sizeof(pids), &pts)) ferr(-1);
	nprocs = pts / sizeof(DWORD);
	//myprint("%i processes.\n", nprocs);
	for(i = 0; i < nprocs; i++)
	{
		h = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, pids[i]);
		if(!h) continue;
		if(!GetProcessImageFileName(h, pname, 255)) {CloseHandle(h); continue;}
		pfile = strrchr(pname, '\\');
		if(!pfile) pfile = pname;
		else pfile++;
		if(!_stricmp(pfile, "Warrior_Kings_Battles.exe"))
			goto found;
		if(!_stricmp(pfile, "Warrior Kings - Battles.exe"))
			goto found;
		CloseHandle(h);
	}
	ferr(-2);

found:	myprint("--- 1 ---\n"); FindAndChange(bs1, sizeof(bs1), to1, sizeof(to1), 0xF0);
	for(i = 0; i < 2; i++)
		{myprint("--- 2 ---\n"); FindAndChange(bs2, sizeof(bs2), to2, sizeof(to2), -1);}
	//myprint("--- 4 ---\n"); FindAndChange((char*)bs4, sizeof(bs4), (char*)to4, sizeof(to4), -1);
	myprint("--- 3 ---\n"); FindAndChange((char*)bs3, sizeof(bs3), (char*)to3, sizeof(to3), 0x0F);
	
	CloseHandle(h);

	MessageBox(0, "The game process has been successfully patched!", appname, 64);

	return 0;
}