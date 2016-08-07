// wkm3v - Simple ".mesh3" model viewer
// (C) 2016 Adrien Geets
// Licensed under the MIT license (see license.txt for more information)

// TIP: Look at "Warrior Kings Game Set\Last Resort\LAST_RESORT.MESH", which is in
//      Mesh2 format and in TEXT! Open it with a text editor! The structure of this
//      file is not much different than the .Mesh3 format.

// NOTE: The only difference between Mesh3 (WKO) and "Mesh4" (WKB) is that the
//       Remapper exists in Mesh3 but is removed in Mesh4.

#include <Windows.h>
#include <d3d9.h>
#include <stdio.h>
#include <d3dx9.h>
#include "growbuffer.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

char *title = "WK MESH3 Viewer", *clsname = "AG_wkm3vWinClass";
char texdir[512] = "Textures\\\0";
IDirect3D9 *d3d9 = NULL; IDirect3DDevice9 *ddev = NULL;
D3DPRESENT_PARAMETERS dpp = {0, 0, D3DFMT_UNKNOWN, 0, D3DMULTISAMPLE_NONE, 0, D3DSWAPEFFECT_DISCARD, 0, 
				TRUE, TRUE, D3DFMT_D16, 0, 0, 0};
uint bluei = 0x0080FF;
int frames = 0, fps = 0;
HWND hWnd = 0;
char fpstbuf[256];
int iwtcolor = 0;
float scaling = 1.0f, ytranslation = 0.0f;
int spin = 1; float lastrot = 0;

D3DVERTEXELEMENT9 ddve[] = {
{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
{1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
D3DDECL_END()
};
IDirect3DVertexDeclaration9 *ddvd;

void fErr(char *text, int n)
{
	MessageBox(0, text, title, 48);
	exit(n);
}

uchar readchar(FILE *file) {return fgetc(file);}				// 1 byte
ushort readshort(FILE *file) {ushort a; fread(&a, 2, 1, file); return a;}	// 2 bytes
uint readint(FILE *file) {uint a; fread(&a, 4, 1, file); return a;}		// 4 bytes
float readfloat(FILE *file) {float a; fread(&a, 4, 1, file); return a;}		// 4 bytes
void ignorestr(FILE *file) {while(fgetc(file));}

float *lstverts;
int *lstmatflags;
IDirect3DTexture9 **lstmattex;
float **lstuvlist;
GrowList<GrowList<int>*> lstgroup;

GrowList<float> mverts;
GrowList<GrowList<float>*> muvlist;
uint *mgrpindex;
GrowList<ushort> mindices;
uint *mstartix;

int ngrp;

//#define dbg(a) MessageBox(hWnd, (a), title, 64)
#define dbg(a) 

void LoadMesh3(char *fn)
{
	FILE *file; int i, ver, nparts, nverts, form;
	file = fopen(fn, "rb");
	if(!file) fErr("Cannot open file.", -4);
	form = readint(file);
	if(form == 'minA')			// If file format is .anim3
	{
		// In case of a .anim3 file look for the corresponding
		// .mesh3 file and load this file instead.
		fseek(file, 8, SEEK_SET);
		char amf[512]; char *a = amf;
		while(*(a++) = fgetc(file));
		LoadMesh3(amf); return;
	} else if(form != 'hseM')		// If file format in not mesh3 nor anim3
		fErr("Unknown format.", -5);
	fseek(file, 4, SEEK_SET);
	ver = readint(file);

	// Attachment Points
	// Can also be attached to another Mesh3/Anim3 or even to particles (.PSystem)
	// At the moment ignored.
	fseek(file, 12, SEEK_SET);
	nparts = readshort(file);
	for(i = 0; i < nparts; i++)
	{
		ignorestr(file);
		fseek(file, 0x1D, SEEK_CUR);
		ignorestr(file);
	}

	// Positions
	nverts = readshort(file);
	lstverts = (float*)malloc(nverts*3*sizeof(float));
	int x = 0;
	fread(lstverts, nverts*3*sizeof(float), 1, file);
	dbg("1");

	// Sphere
	fseek(file, 16, SEEK_CUR);

	// Remapper
	// Only exists if version is 3 (WKO), version 4 (WKBattles) removed it.
	// Even if it exists in WKO, it is in most cases useless.
	// Here we simply ignore it.
	if(ver < 4)
	{
		i = (ushort)readshort(file);
		fseek(file, 2 * i, SEEK_CUR);
	}

	// Normals
	// At the moment they are ignored.
	int nnorm = readshort(file);
	fseek(file, readint(file) * 2, SEEK_CUR);

	// Materials
	// A material consists of 2 things:
	//  - a single byte indicating if alpha test should be enabled (1) or not (0)
	//    on this material
	//  - a string of 8-bit characters ending with byte 0 which is the file name
	//    of the texture located in "Warrior Kings Game Set\Textures\"
	int nmat = readshort(file);
	lstmatflags = (int*)malloc(nmat*sizeof(int));
	lstmattex = (IDirect3DTexture9**)malloc(nmat*sizeof(IDirect3DTexture9*));
	for(i = 0; i < nmat; i++)
	{
		char *s = new char[256]; char *p = s;
		lstmatflags[i] = readchar(file);
		strcpy(s, texdir);
		p = s + strlen(s);
		while(*(p++) = fgetc(file));
		IDirect3DTexture9 *tex;
		if(FAILED(D3DXCreateTextureFromFile(ddev, s, &tex)))
			fErr("Cannot load texture. Be sure you have made the \"Textures\" directory in the directory of the executable and you have copied the textures from the game in that directory. Look at \"readme.txt\" for more details.", -700);
		lstmattex[i] = tex;
		delete [] s;
	}
	dbg("2");

	// Texture coordinates
	// Every UV list contains the same amount of texture coordinates.
	// Each UV list correspond to a player color. For example the UV list #0
	// contains UV vertices that map to brown parts of the texture which correspond
	// to the brown player, #1 corresponds to blue, #2 yellow, #3 red, ...
	// As such, if you want to switch the player color, you simply switch to
	// the corresponding UV list.
	int nuvlist = readint(file);
	lstuvlist = (float**)malloc(nuvlist * sizeof(float*));
	for(int l = 0; l < nuvlist; l++)
	{
		int ntexc = readshort(file);
		float *uvl = (float*)malloc(ntexc * 2 * sizeof(float));
		fread(uvl, ntexc * 2 * sizeof(float), 1, file);
		lstuvlist[l] = uvl;
		muvlist.add(new GrowList<float>);
	}
	dbg("3");

	// Groups
	// A group of a certain number corresponds to the material with the
	// same number. Logically there must be as many groups as materials.
	ngrp = readint(file);
	mgrpindex = (uint*)malloc((ngrp+1) * sizeof(uint));
	mgrpindex[0] = 0;
	for(i = 0; i < ngrp; i++)
	{
		int sg = readshort(file);
		for(int j = 0; j < sg; j++)
		{
			int v = readshort(file);
			for(int k = 0; k < 3; k++)
				mverts.add(lstverts[v*3+k]);
			readshort(file); // Index to normal list
			v = readshort(file);
			for(int u = 0; u < nuvlist; u++)
				for(int k = 0; k < 2; k++)
					(muvlist[u])->add((lstuvlist[u])[v*2+k]);
		}
		mgrpindex[i+1] = mgrpindex[i]+sg;
	}
	dbg("4");

	// Polygon Lists
	// One used at the moment.
	int ntrilist = readint(file);

	readshort(file); readshort(file); readshort(file); readfloat(file);
	mstartix = (uint*)malloc((nmat + 1) * sizeof(uint));
	mstartix[0] = 0;
	// For every group/material
	for(int n = 0; n < nmat; n++)
	{
		int np = readshort(file);
		readshort(file);
		readshort(file);
		mstartix[n+1] = mstartix[n] + (np * 3);
		// For every triangle (polygon with 3 points)
		for(i = 0; i < np; i++)
			for(int j = 0; j < 3; j++)
				mindices.add(readshort(file)+mgrpindex[n]);
	}

	// Output some debug file.
	//FILE *df = fopen("debug.txt", "w");
	//for(i = 0; i < mindices.len/3; i++)
	//	fprintf(df, "%u %u %u\n", mindices[i*3], mindices[i*3+1], mindices[i*3+2]);
	//for(i = 0; i < mgrpindex.len; i++)
	//	fprintf(df, "%u\n", mgrpindex[i]);
	//for(i = 0; i < mverts.len/3; i++)
	//	fprintf(df, "%f %f %f\n", mverts[i*3], mverts[i*3+1], mverts[i*3+2]);
	//for(i = 0; i < muvlist[0]->len/2; i++)
	//	fprintf(df, "%f %f\n", muvlist[0]->get(i*2), muvlist[0]->get(i*2+1));
	//fclose(df);

	fclose(file);
}

IDirect3DVertexBuffer9 *dvbverts;
GrowList<IDirect3DVertexBuffer9*> dvbtexc;
IDirect3DIndexBuffer9 *dixbuf;

void InitDraw()
{
	ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	ddev->SetRenderState(D3DRS_LIGHTING, FALSE);
	ddev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	// D3DSAMP_ALPHAFUNC and D3DSAMP_ALPHAREF values taken from original
	// game via PIX/graphics debugger.
	ddev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	ddev->SetRenderState(D3DRS_ALPHAREF, 240);
	ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	ddev->CreateVertexDeclaration(ddve, &ddvd);
	ddev->SetVertexDeclaration(ddvd);

	ddev->CreateVertexBuffer(mverts.len*4, 0, 0, D3DPOOL_DEFAULT, &dvbverts, 0);
	ddev->CreateIndexBuffer(mindices.len*2, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &dixbuf, 0);

	void *buf;
	dvbverts->Lock(0, mverts.len*4, &buf, 0);
	memcpy(buf, mverts.gb.memory, mverts.len * 4);
	dvbverts->Unlock();
	dixbuf->Lock(0, mindices.len*2, &buf, 0);
	memcpy(buf, mindices.gb.memory, mindices.len*2);
	dixbuf->Unlock();

	for(int i = 0; i < muvlist.len; i++)
	{
		IDirect3DVertexBuffer9 *ul;
		ddev->CreateVertexBuffer(muvlist[i]->len*4, 0, 0, D3DPOOL_DEFAULT, &ul, 0);
		ul->Lock(0, muvlist[i]->len*4, &buf, 0);
		memcpy(buf, muvlist[i]->gb.memory, muvlist[i]->len*4);
		ul->Unlock();
		dvbtexc.add(ul);
	}
}

void SetMatrices()
{
	if(spin)
		lastrot = (timeGetTime()%2000)*(2.0f*D3DX_PI)/2000.0f;

	D3DXMATRIX matWorld, rotMatrix, transMatrix, scaleMatrix;
	D3DXMatrixRotationY(&rotMatrix, lastrot);
	D3DXMatrixTranslation(&transMatrix, 0.0f, ytranslation, 0.0f);
	D3DXMatrixScaling(&scaleMatrix, scaling, scaling, scaling);
	D3DXMatrixMultiply(&matWorld, &rotMatrix, &transMatrix);
	D3DXMatrixMultiply(&matWorld, &matWorld, &scaleMatrix);
	ddev->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEyePt( 0.0f, 18.0f,-20.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	ddev->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 4.0f/3.0f, 1.0f, 100.0f );
	ddev->SetTransform( D3DTS_PROJECTION, &matProj );
}

void Render()
{
	ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bluei, 1.0f, 0);
	ddev->BeginScene();

	SetMatrices();

	ddev->SetStreamSource(0, dvbverts, 0, 12);
	ddev->SetStreamSource(1, dvbtexc[iwtcolor], 0, 8);
	ddev->SetIndices(dixbuf);
	for(int g = 0; g < ngrp; g++)
	{
		ddev->SetRenderState(D3DRS_ALPHATESTENABLE, lstmatflags[g]?TRUE:FALSE);
		ddev->SetTexture(0, lstmattex[g]);
		ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, mgrpindex[g], mgrpindex[g+1]-mgrpindex[g]/*mstartix[g+1]-mstartix[g]*/, mstartix[g], (mstartix[g+1]-mstartix[g])/3);
	}

	ddev->EndScene();
	ddev->Present(NULL, NULL, NULL, NULL);
	//bluei++;
	frames++;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CHAR:
			switch(tolower(wParam))
			{
				case 'c':
					if(++iwtcolor >= muvlist.len) iwtcolor = 0;
					break;
				case '*':
					scaling *= 1.5f; break;
				case '/':
					scaling /= 1.5f; break;
				case 'e':
					ytranslation += 0.5f; break;
				case 'd':
					ytranslation -= 0.5f; break;
				case ' ':
					spin = !spin; break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0); break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

void CALLBACK OnSecond(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	fps = frames;
	frames = 0;
	_snprintf(fpstbuf, 255, "%s - FPS: %i", title, fps);
	SetWindowText(hWnd, fpstbuf);
}

void OnExit()
{
	if(ddev) ddev->Release();
	if(d3d9) d3d9->Release();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *cmdArgs, int showMode)
{
	MSG msg; BOOL bRet; RECT rect = {0, 0, 640, 480};
	WNDCLASS wndclass = {0, WndProc, 0, 0, hInstance,
			NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL, clsname};
	atexit(OnExit);

	// Get texture directory path.
	GetModuleFileName(NULL, texdir, sizeof(texdir)); // Get executable path.
	strcpy(strrchr(texdir, '\\'), "\\Textures\\");   // Replaces "\\wkbre.exe" with "\\Textures\\".

	// Creating Window
	if(!RegisterClass(&wndclass)) fErr("Class registration failed.", -1);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(clsname, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, hInstance, NULL);
	if(!hWnd) fErr("Window creation failed.", -2);
	ShowWindow(hWnd, showMode);
	SetTimer(0, 0, 1000, OnSecond);

	// Initializing Direct3D 9
	dpp.hDeviceWindow = hWnd;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d9) fErr("Direct3D 9 init failed.", -300);
	if(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dpp, &ddev) != D3D_OK)
		fErr("Direct3D 9 Device creation failed.", -3);

	// Get first argument and open it directly if available.
	char argfn[256], *s; int fao = 0;
	strcpy_s(argfn, sizeof(argfn)-1, cmdArgs);
	s = argfn;
	while(isspace(*s)) s++;
	if(*s)
	{
		if(*s == '\"')
		{
			char *ad = ++s;
			while(*s != '\"' && *s != 0) s++;
			*s = 0;
			LoadMesh3(ad); fao = 1;
		}
		else	LoadMesh3(s); fao = 1;
	}

	if(!fao) //LoadMesh3("ABADDON_MOVE.MESH3");
	{
		MessageBox(hWnd, "You must specify a Mesh3/Anim3 file path as a command-line argument. You can also drag & drop the file in the executable file. Look at the accompanying \"readme.txt\" for more details.\n\nwkm3v Version 0.1\n(C) 2016 AdrienTD", title, 48);
		exit(-47);
	}
	InitDraw();

	msg.message = 0;
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else	Render();
	}
	exit(0);
}