// obj2wkm - OBJ to MESH3 file converter for WK (Battles)
// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#define GROWBUF_ALLOC_STEP 512

class GrowBuffer
{
public:
	char *memory; int memsize, apnt, align;

	GrowBuffer(int a = 1)
	{
		memory = (char*)malloc(memsize = GROWBUF_ALLOC_STEP);
		if(!memory) ferr("Failed to init. GrowBuffer.");
		apnt = 0; align = a;
	}
	~GrowBuffer() {free(memory);}

	char *addSize(int ds);
	void addData(char *data, int ds) {memcpy(addSize(ds), data, ds);}
	void clear();
};

template <class T> class GrowList
{
public:
	GrowBuffer gb; unsigned int len;
	GrowList() {len = 0;}
	T *addp() {len++; return (T*)gb.addSize(sizeof(T));}
	unsigned int add(T a) {gb.addData((char*)&a, sizeof(T)); return len++;}
	void clear() {gb.clear(); len = 0;}
	T get(unsigned int n)
	{
#ifndef WKBRE_RELEASE
		if(n >= len) ferr("GrowList subscript overflow.");
#endif
		return *(((T*)gb.memory)+n);
	}
	T *getpnt(unsigned int n)
	{
#ifndef WKBRE_RELEASE
		if(n >= len) ferr("GrowList::getpnt subscript overflow.");
#endif
		return ((T*)gb.memory)+n;
	}
	T &getref(unsigned int n)
	{
#ifndef WKBRE_RELEASE
		if(n >= len) ferr("GrowList::getref subscript overflow.");
#endif
		return *(((T*)gb.memory)+n);
	}
	unsigned int find(T d)
	{
		for(unsigned int i = 0; i < len; i++)
			if(*(((T*)gb.memory)+i) == d)
				return i;
		return -1;
	}
	unsigned int has(T d)
	{
		for(unsigned int i = 0; i < len; i++)
			if(*(((T*)gb.memory)+i) == d)
				return 1;
		return 0;
	}
	T operator[](unsigned int n) {return this->get(n);}
};

class GrowStringList
{
public:
	GrowBuffer gbData; GrowList<unsigned int> gbPnt; unsigned int len;
	GrowStringList() {len = 0;}
	unsigned int add(char *str)
	{
		gbPnt.add(gbData.apnt);
		gbData.addData(str, strlen(str)+1);
		return len++;
	}
	void clear() {gbData.clear(); gbPnt.clear(); len = 0;}
	char *get(unsigned int n)
	{
		char *s = gbData.memory + gbPnt.get(n);
		char *r = new char[strlen(s)];
		strcpy(r, s);
		return r;
	}
	char *getdp(unsigned int n) {return gbData.memory + gbPnt.get(n);}
	char *operator[](unsigned int n) {return this->getdp(n);}
	int has(char *str)
	{
		for(int i = 0; i < len; i++)
			if(!_stricmp(gbData.memory + gbPnt.get(i), str))
				return 1;
		return 0;
	}
	int find(char *str)
	{
		for(int i = 0; i < len; i++)
			if(!_stricmp(gbData.memory + gbPnt.get(i), str))
				return i;
		return -1;
	}
	int find_cs(char *str)
	{
		for(int i = 0; i < len; i++)
			if(!strcmp(gbData.memory + gbPnt.get(i), str))
				return i;
		return -1;
	}
	int find_backwards(char *str)
	{
		for(int i = len-1; i >= 0; i--)
			if(!_stricmp(gbData.memory + gbPnt.get(i), str))
				return i;
		return -1;
	}
};