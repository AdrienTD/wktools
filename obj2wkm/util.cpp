// obj2wkm - OBJ to MESH3 file converter for WK (Battles)
// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

// Misc. useful functions

#include "global.h"

int gline = 0;

void ferr(char *str)
{
	printf("Error: %s\n", str);
	exit(-1);
}

int stfind_cs(char **t, int len, char *f)
{
	for(int i = 0; i < len; i++)
		if(!strcmp(t[i], f))
			return i;
	return -1;
}

char *GetLine(char *f, char *str)
{
	char *o, *frp;
	o = str; frp = f;
	while(1)
	{
		if((*frp == 0) || (*frp == 255)) break;
		if(*frp == '\r') {frp++; continue;}
		if(*frp == '\n') {frp++; break;}
		if(*frp == '/')
		{
			if(*(frp+1) == '*')
			{
				frp += 2;
				while(!((*frp == '*') && (*(frp+1) == '/')))
					{if((*(frp+1) == 0) || (*(frp+1) == 255)) {frp++; goto glend;} frp++;}
				frp += 2;
			}
			if(*(frp+1) == '/')
			{
				frp += 2;
				while(*frp != '\n')
					{if(*frp == 0) goto glend; frp++;}
				frp++; goto glend;
			}
		}
		*o = *frp; o++; frp++;
	}
glend:	*o = 0; gline++;
	return frp;
}

int GetWords(char *str, char **list)
{
	char *p; int n = 0;
	p = str;
	while(*p)
	{
		while(isspace(*p)) p++;
		if(!(*p)) return n;

		if(*p != '\"')
		{
			list[n] = p;
			n++; if(n >= MAX_WORDS_IN_LINE) return n;
			while(!isspace(*p)) {if(!(*p)) return n; p++;}
			if(!(*p)) return n; // Can be removed because isspace(0) = 0
			*p = 0; p++; if(!(*p)) return n;
		} else {
			p++; if(!(*p)) return n;
			list[n] = p;
			n++; if(n >= MAX_WORDS_IN_LINE) return n;
			while(*p != '\"') {if(!(*p)) return n; p++;}
			*p = 0; p++; if(!(*p)) return n;
		}
	}
	return n;
}
