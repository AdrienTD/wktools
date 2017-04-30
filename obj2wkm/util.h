// obj2wkm - OBJ to MESH3 file converter for WK (Battles)
// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#define MAX_WORDS_IN_LINE 1024
#define MAX_LINE_SIZE 32768

#ifndef _MSC_VER
#define strcpy_s(a, b, c) strcpy(a, c)
#endif

typedef void (*vfwa)(void*);
typedef void (*voidfunc)();
template <class T> struct couple {T x, y;};

extern int gline;

void ferr(char *str);
int stfind_cs(char **t, int len, char *f);
char *GetLine(char *f, char *str);
int GetWords(char *str, char **list);
