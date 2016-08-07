// BCP Viewer
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

#define VERSION "0.1.0.0"
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"
#include "lzrw_headers.h"

#define MAX_DIR_LEV 16
typedef unsigned int uint;
typedef struct
{
	uint offset, endos, size, unk, form;
} fileentry;

// GUI
char title[] = "BCP Viewer", clsname[] = "AG_BCPViewWinClass";
HINSTANCE hinst; HWND hwnd;
HWND htree;
HIMAGELIST himl;
HWND hstatus; int statusheight;
char ofnname[256];
OPENFILENAME ofn = {sizeof(OPENFILENAME), /**/0, /**/0, "Black Cactus Pack file\0*.BCP\0All files\0*.*\0", NULL, 0, 0, ofnname, 256, NULL, 0, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR, 0, 0, NULL, (LPARAM)NULL, NULL, NULL, NULL, 0, 0};
char cfname[256];
char ubuf[256];
char pathbuf[384];
char fdbuf[256];
char argfn[256];

char secret[] = {0xec,0xc9,0xdf,0xc4,0xc8,0xc3,0x8d,0xea,0xc8,0xc8,0xd9,0xde};

// File loading
FILE *file = 0;
wchar_t wbuf[256]; char abuf[256];
uint fentof;
uint nfiles = 0;
fileentry *fent;
int dirstack[MAX_DIR_LEV]; HTREEITEM htistack[MAX_DIR_LEV];
int dirlev = 0; int *dirsp = dirstack; HTREEITEM *htisp = htistack;
TVINSERTSTRUCT idti;
TVITEM fti;

void fErr(int n, char *text)
{
	MessageBox(hwnd, text, title, 48);
	exit(n);
}

void afstr()
{
	unsigned int c;
	c = (unsigned char)fgetc(file);
	fread(abuf, c, 1, file);
	abuf[c] = 0;
}

void cfstr()
{
	unsigned int c;
	c = (unsigned char)fgetc(file);
	fread(wbuf, c, 2, file);
	wbuf[c] = 0;
}

void bufwtoa()
{
	int i;
	for(i = 0; i < 255; i++)
	{
		abuf[i] = wbuf[i];
		if(!wbuf[i]) return;
	}
}

void CloseBCP()
{
	if(!file) return;
	free(fent);
	fclose(file); file = 0; nfiles = 0;
	TreeView_DeleteAllItems(htree);
}

int LoadBCP(char *fn)
{
	int i, f; uint numfiles, root = 1, id; HTREEITEM pti = TVI_ROOT; int ver, rp;
	if(file) CloseBCP();
	file = fopen(fn, "rb");
	if(!file) fErr(-1, "File not found.");

	idti.hParent = TVI_ROOT;
	idti.hInsertAfter = TVI_LAST;
	idti.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	idti.item.pszText = abuf;
	idti.item.cchTextMax = 3;

	while(1)
	{
		i = fgetc(file);
		if(feof(file)) fErr(-3, "Where is the 0xFF byte?");
		if(i == 0xFF || i == 0) break;
	}
	rp = ftell(file);
	fseek(file, 9, SEEK_SET);
	ver = fgetc(file) - '0';
	fseek(file, rp, SEEK_SET);

	fseek(file, 2, SEEK_CUR);
	fseek(file, fentof = _getw(file), SEEK_SET);
	nfiles = _getw(file);
	fent = (fileentry*)malloc(nfiles*20);
	if(!fent) fErr(-2, "Failed to allocate memory for the file position/size table.");
	fread(fent, nfiles*20, 1, file);

	fseek(file, fentof+4+nfiles*20, SEEK_SET);
	*dirsp = 1000; *htisp = TVI_ROOT;
	while(!feof(file))
	{
		if(root) {/*printf("ROOT\n");*/ root = 0;}
		else
		{
			//printf("DIRECTORY\n");
			/*printnum(*/ i = _getw(file); //);
			if(i)
			{
				// Next level
				dirsp++; // Push
				htisp++;
				*dirsp = i+1;				
				dirlev++;
			}
			while(!(--(*dirsp)))
			{
				dirsp--; // Pop
				htisp--;
				dirlev--;
			}
			if(ver == 2) {cfstr(); bufwtoa();}
			else afstr();
			if(feof(file)) goto flend;

			idti.hParent = *(htisp-1);
			idti.item.cchTextMax = strlen(abuf);
			idti.item.lParam = -1;
			idti.item.iImage = idti.item.iSelectedImage = 0;
			*htisp = TreeView_InsertItem(htree, &idti);

			//printdirlev();
			//printf(" (%i)\n", i);
		}
		numfiles = _getw(file);

		// Files in directory listing.
		for(f = 0; f < numfiles; f++)
		{
			if(feof(file)) goto flend;
			id = _getw(file); // printnum
			_getw(file); // printnum
			_getw(file); // printnum
			//fputc(' ', stdout);
			//printdirlev();
			if(ver == 2) {cfstr(); bufwtoa();}
			else afstr();
			idti.hParent = *htisp;
			idti.item.cchTextMax = strlen(abuf);
			idti.item.lParam = id;
			idti.item.iImage = idti.item.iSelectedImage = fent[id].form + 1;
			TreeView_InsertItem(htree, &idti);
			//fputc('\n', stdout);
			//printf("\nOffset: 0x%08X\nSize:   %i\nFormat: %i\n\n", fent[id].offset, fent[id].size, fent[id].form);
		}
	}
flend:	//fclose(file);
	return 0;
}

void ExtractFile(int id, char *fs)
{
	FILE *out; uint i, s; char fn[256] = "bcpxtract\\\0";
	char *min, *mout, *ws; uint os, sws;
	if(!file) return;
	strcat_s(fn, 255, fs);
	out = fopen(fn, "wb");
	if(!out) return;
	fseek(file, fent[id].offset, SEEK_SET);
	s = fent[id].endos - fent[id].offset;
	switch(fent[id].form)
	{
		case 3: // Patch file (?)
		case 1: // Uncompressed
			for(i = 0; i < s; i++)
				fputc(fgetc(file), out);
			break;
		case 2: // Zero-sized file
			break;
		case 4: // LZRW3-compressed file
			ws = (char*)malloc(16400);
			min = (char*)malloc(s);
			mout = (char*)malloc(os = fent[id].size);
			fread(min, s, 1, file);
			compress(COMPRESS_ACTION_DECOMPRESS, ws, min, s, mout, &os);
			if(fent[id].size != os)
				MessageBox(hwnd, "The decompressed sizes in the file table and from the decompression algorithm are different.", title, 48);
			fwrite(mout, fent[id].size, 1, out);
			free(ws); free(min); free(mout);
			break;
	}
	fclose(out);
}

void CopyText(char *i)
{
	HGLOBAL hg; char *o; int sz;
	sz = strlen(i);
	hg = GlobalAlloc(GMEM_MOVEABLE, sz+1);
	o = GlobalLock(hg);
	memcpy(o, i, sz+1); // Including 0 terminator.
	GlobalUnlock(hg);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
}

char *GetItemPath(HTREEITEM hti)
{
	char *s; int i;
	s = pathbuf + 383;
	pathbuf[383] = 0;
	fdbuf[255] = 0;
	fti.mask = TVIF_TEXT;
	fti.pszText = fdbuf;
	fti.cchTextMax = 255;

	do
	{
		fti.hItem = hti;
		TreeView_GetItem(htree, &fti);
		i = strnlen(fdbuf, 255);
		s -= i+1;
		if(s < pathbuf) return 0;
		memcpy(s+1, fdbuf, i);
		s[0] = '\\';
	} while(hti = TreeView_GetParent(htree, hti));

	return s + 1;
}

void EnsureDirectoriesArePresent(char *s)
{
	char pd[256], *p, *n; int i = 0;
	p = s;
	GetCurrentDirectory(255, pd);
	_chdir("bcpxtract");
	while(1)
	{
		n = strchr(p, '\\');
		if(!n) break;
		*n = 0;
		_mkdir(p);
		_chdir(p);
		*n = '\\';
		p = n + 1;
		if(++i >= 16) fErr(-2332, "EnsureDirectoriesArePresent is buggy.");
	}
	_chdir(pd);
}

void ExtractAll()
{
	TVITEM t; int i, nl; char *s; HTREEITEM hn, hm; FILE *log;
	//log = fopen("exalltest.txt", "w");
	if(!file) return;
	nl = TreeView_GetCount(htree);
	t.mask = TVIF_PARAM;
	t.hItem = TreeView_GetChild(htree, TVI_ROOT); // First item
	for(i = 0; i < nl; i++)
	{
		TreeView_GetItem(htree, &t);
		s = GetItemPath(t.hItem);
		if(!s) {MessageBox(hwnd, "Error in extracting all files.", title, 16); break;}
		if(t.lParam != -1)
		{
			EnsureDirectoriesArePresent(s);
			ExtractFile(t.lParam, s);
			//fprintf(log, "FILE ");
		}//else	fprintf(log, " DIR ");
		//fprintf(log, "%i: %s\n", i, s);
		hn = TreeView_GetChild(htree, t.hItem);
		if(!hn) hn = TreeView_GetNextSibling(htree, t.hItem);
		if(!hn)
		{
			hn = t.hItem;
gns:			hn = TreeView_GetParent(htree, hn);
			if(!hn) break;
			hm = TreeView_GetNextSibling(htree, hn);
			if(!hm) goto gns;
			hn = hm;
		}
		t.hItem = hn;
	}
	//fclose(log);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TVINSERTSTRUCT tins; HTREEITEM hti; RECT rect; int i; char *s;
	switch(uMsg)
	{
		case WM_CREATE:
			hstatus = CreateWindowEx(0, STATUSCLASSNAME, "Click on File then on Open... to open a file.", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, (HMENU)1000, hinst, 0);
			GetClientRect(hstatus, &rect);
			statusheight = rect.bottom;
			GetClientRect(hWnd, &rect);
			htree = CreateWindowEx(0, WC_TREEVIEW, "Tree View", WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
					0, 0, rect.right, rect.bottom-statusheight, hWnd, NULL, hinst, NULL);
			himl = ImageList_LoadImage(hinst, MAKEINTRESOURCE(IDB_ICONS), 16, 5, 0xFF00FF, IMAGE_BITMAP, LR_CREATEDIBSECTION);
			TreeView_SetImageList(htree, himl, TVSIL_NORMAL);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_OPEN:
					if(GetOpenFileName(&ofn))
						LoadBCP(ofnname);
					break;
				case IDM_CLOSE:
					CloseBCP(); break;
				case IDM_EXTRACTALL:
					i = MessageBox(hwnd, "Are you sure you want to extract all the files from this archive to the bcpxtract directory?\nThis can take a lot of time depending on the size of the archive.", title, 48 | MB_YESNO);
					if(i == IDYES)
					{
						ExtractAll();
						MessageBox(hwnd, "All files were extracted to the bcpxtract directory.", title, 64);
					}
					break;
				case IDM_ABOUT:
					MessageBox(hWnd, "BCP Viewer\nVersion: " VERSION "\nBy AdrienTD", title, 64); break;
				case IDM_QUIT:
					DestroyWindow(hWnd); break;
			} break;
		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code)
			{
				case NM_DBLCLK:
					if(((LPNMHDR)lParam)->hwndFrom == htree)
					{
						fti.mask = TVIF_PARAM | TVIF_TEXT;
						fti.hItem = TreeView_GetSelection(htree);
						if(!fti.hItem) break;
						fti.pszText = cfname; fti.cchTextMax = 255;
						TreeView_GetItem(htree, &fti);
						if(fti.lParam == -1) break;
						//MessageBox(hwnd, fti.pszText, title, 64);
						ExtractFile(fti.lParam, fti.pszText);
						MessageBox(hWnd, "File extracted!", title, 64);
					}
					break;
				case TVN_SELCHANGED:
					if(((LPNMHDR)lParam)->hwndFrom == htree)
					{
						i = ((LPNMTREEVIEW)lParam)->itemNew.lParam;
						if(i == -1) {SetWindowText(hstatus, "Directory"); break;}
						_snprintf(ubuf, 256, "Offset: 0x%08X, Archived size: %u, Uncompressed size: %u, Format: %u", fent[i].offset, fent[i].endos - fent[i].offset, fent[i].size, fent[i].form);
						i = (i + '0') & 0xFFFFFF;
						SetWindowText(hstatus, ubuf);
					}
					break;
				case NM_RCLICK:
					if(((LPNMHDR)lParam)->hwndFrom == htree)
					{
						hti = TreeView_GetSelection(htree);
						if(!hti) break;
						s = GetItemPath(hti);
						if(s) CopyText(s);
					}
					break;
			}
			break;
#ifdef VIP_SHORTCUT
		case WM_CHAR:
			if(tolower(wParam) == 'l')
				LoadBCP("C:\\Program Files (x86)\\Empire Interactive\\Warrior Kings - Battles\\data_o.bcp");
			if(tolower(wParam) == 'x')
				ExtractAll();
			break;
#endif
		case WM_SIZE:
			MoveWindow(htree, 0, 0, LOWORD(lParam), HIWORD(lParam)-statusheight, FALSE);
			SendMessage(hstatus, WM_SIZE, 0, 0);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *cmdArgs, int showMode)
{
	HWND hWnd; MSG msg; BOOL bRet; char *s, *ad;
	WNDCLASS wndclass = {CS_OWNDC | CS_VREDRAW | CS_HREDRAW, WndProc, 0, 0, hInstance,
			0, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL, clsname};

	ofn.hInstance = hinst = hInstance;
	wndclass.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_MAINICON));
	InitCommonControls();
	_mkdir("bcpxtract");

	if(!RegisterClass(&wndclass)) fErr(-1, "Class registration failed.");
	ofn.hwndOwner = hwnd = hWnd = CreateWindow(clsname, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, LoadMenu(hinst, MAKEINTRESOURCE(IDR_MAINMENU)), hInstance, NULL);
	if(!hWnd) fErr(-2, "Window creation failed.");
	ShowWindow(hWnd, showMode);

	// Get first argument and open it directly if available.
	strcpy_s(argfn, sizeof(argfn)-1, cmdArgs);
	s = argfn;
	while(isspace(*s)) s++;
	if(*s)
	{
		if(*s == '\"')
		{
			ad = ++s;
			while(*s != '\"' && *s != 0) s++;
			*s = 0;
			LoadBCP(ad);
		}
		else	LoadBCP(s);
	}

	while(bRet = GetMessage(&msg, NULL, 0, 0))
	if(bRet != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CloseBCP();
	return 0;
}