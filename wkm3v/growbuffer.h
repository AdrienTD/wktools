// wkm3v - Simple ".mesh3" model viewer
// (C) 2016 Adrien Geets
// Licensed under the MIT license (see license.txt for more information)

#define GROWBUF_ALLOC_STEP 128
#define ferr(a) 

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

	void addData(char *data, int ds);

	void clear()
	{
		if(!apnt) return;
		free(memory);
		memory = (char*)malloc(memsize = GROWBUF_ALLOC_STEP);
		if(!memory) ferr("Failed to clear GrowBuffer.");
		apnt = 0;
	}
};

template <class T> class GrowList
{
public:
	GrowBuffer gb; unsigned int len;
	GrowList() {len = 0;}
	unsigned int add(T a) {gb.addData((char*)&a, sizeof(T)); return len++;}
	void clear() {gb.clear(); len = 0;}
	T get(unsigned int n)
	{
		if(n >= len) ferr("GrowList subscript overflow.");
		return *(((T*)gb.memory)+n);
	}
	T operator[](unsigned int n) {return this->get(n);}
};

class GrowStringList
{
public:
	GrowBuffer gbData; GrowList<unsigned int> gbPnt; unsigned int len;
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
	char *operator[](unsigned int n) {return this->get(n);}
	char *getdp(unsigned int n) {return gbData.memory + gbPnt.get(n);}
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
};