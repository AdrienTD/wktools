// bigcpp - Puts linked WK game set files in one same file.
// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

#include <stdio.h>
#include <Windows.h>
#define MAX_LINE_SIZE 1024
#define MAX_WORDS_IN_LINE 256
#define MAX_LINKED_FILES 1024
#define MAX_BLUEPRINTS 1024

char *alf[MAX_LINKED_FILES]; unsigned int numalf = 0;
int flalf[MAX_LINKED_FILES], outline = 1;

char *blp[MAX_BLUEPRINTS]; unsigned int numblp = 0;
int npblp[MAX_BLUEPRINTS];

void ferr(int n)
{
	int i;
	printf("// Error %i\n", n);
	for(i = 0; i < numalf; i++)
		if(alf[i]) free(alf[i]);
	exit(n);
}

int VerifyLF(char *str)
{
	int i;
	for(i = 0; i < numalf; i++)
		if(!_stricmp(str, alf[i]))
			return 1;
	return 0;
}

void AddLF(char *str)
{
	alf[numalf] = (char*)malloc(strlen(str)+1);
	if(!alf[numalf]) ferr(-3);
	strcpy(alf[numalf], str);
	flalf[numalf] = outline;
	numalf++;
}

int VerifyBLP(char *str)
{
	int i;
	for(i = 0; i < numblp; i++)
		if(!strcmp(str, blp[i]))
			return i;
	return -1;
}

void AddBLP(char *str)
{
	blp[numblp] = (char*)malloc(strlen(str)+1);
	if(!blp[numblp]) ferr(-3);
	strcpy(blp[numblp], str);
	numblp++;
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

void GetString(char *str, char *out)
{
	char *p, *o;
	p = str + 1; o = out;
	while((*p != '\"') && (*p != 0)) {*(o++) = *(p++);}
	*o = 0;
}

void RemoveNewline(char *str)
{
	int i;
	i = strlen(str);
	if(i >= 1)
	if(str[i-1] == '\n')
		str[i-1] = 0;
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
glend:	*o = 0;
	return frp;
}

char *classes[13] = {
"USER",
"LEVEL",
"PLAYER",
"CITY",
"TOWN",
"BUILDING",
"CHARACTER",
"CONTAINER",
"FORMATION",
"MARKER",
"MISSILE",
"PROP",
"ARMY",
};

int isobjdef(char *s)
{
	int i;
	for(i = 0; i < 13; i++)
		if(!strcmp(s, classes[i]))
			return 1;
	return 0;
}

void LookAt(char *filename)
{
	FILE *file; char *fcnt, *fp; int fsize, nwords, i; WIN32_FIND_DATA wfd; HANDLE hfind;
	char line[MAX_LINE_SIZE], wwl[MAX_LINE_SIZE], *word[MAX_WORDS_IN_LINE], sarg[MAX_LINE_SIZE];
	char dirpath[MAX_LINE_SIZE]; char *lbs; int ladder = 0;
	char *lkfile[512]; int nlkf = 0;

	printf("// From %s\n", filename); /*fflush(stdout);*/ AddLF(filename); outline++;

	strcpy(dirpath, filename);
	lbs = strrchr(dirpath, '\\');
	if(!lbs) dirpath[0] = 0;
	else lbs[1] = 0;

	file = fopen(filename, "rb"); // Binary mode is important!
	if(!file) {printf("// Cannot load %s\n", filename); ferr(-2);}
	fseek(file, 0, SEEK_END); fsize = ftell(file); fseek(file, 0, SEEK_SET);
	fcnt = (char*)malloc(fsize+1); if(!fcnt) {printf("// No memory left for loading the file."); ferr(-3);}
	fread(fcnt, fsize, 1, file);
	fcnt[fsize] = 0;
	fp = fcnt;
	while(*fp)
	{
		fp = GetLine(fp, line);
		strcpy(wwl, line);
		nwords = GetWords(wwl, word);

		if(nwords < 1) {printf("\n"); outline++; continue;}

		if(nwords >= 1)
		{
			i = VerifyBLP(word[0]);
			if(i == -1)
				{npblp[numblp] = 1; AddBLP(word[0]);}
			else
				{npblp[i]++;}
		}

		if(nwords >= 2) if(!strcmp(word[0], "LINK_GAME_SET"))
		{
			strcpy(sarg, word[1]);
			if(!strchr(sarg, '*'))
			{
				//if(!VerifyLF(sarg))
				//	LookAt(sarg);
				lkfile[nlkf++] = strdup(sarg);
			}
			else
			{
				/*hfind = FindFirstFile(sarg, &wfd);
				if(hfind == INVALID_HANDLE_VALUE)
					continue;
				do {
					if(!VerifyLF(wfd.cFileName))
						LookAt(wfd.cFileName);
				} while(FindNextFile(hfind, &wfd));
				FindClose(hfind);*/
			}
			continue;
		}
		if(!strcmp(word[0], "CHARACTER_LADDER"))
			ladder = 1;
		else if(!strcmp(word[0], "END_CHARACTER_LADDER"))
			ladder = 0;
		else if(!ladder)
		 if(isobjdef(word[0]))
		{
			printf("%s \"%s\" \"%s%s\"\n", word[0], word[1], dirpath, (nwords>=3)?word[2]:"");
			/*fflush(stdout);*/ outline++; continue;
		}
		fputs(line, stdout); printf("\n"); /*fflush(stdout);*/ outline++;
	}
	for(i = 0; i < nlkf; i++)
		if(!VerifyLF(lkfile[i]))
			LookAt(lkfile[i]);
	free(fcnt);
	fclose(file);
}

int main(int argc, char *argv[])
{
	int i;
	if(argc < 2) {printf(
		"bigcpp version 0.2\n"
		"(C) 2016 AdrienTD\n\n"
		"Usage: bigcpp <file.cpp>\n"
		"The current/working directory must be the \"Warrior Kings Game Set\" directory.\n"
		"Please read the supplied \"readme.txt\" for more details.\n"); return -1;}

	LookAt(argv[1]);

	// Print summary of linked files.
	printf("\n/* %i linked files:\n", numalf);
	for(i = 0; i < numalf; i++)
		printf("Line %i: %s\n", flalf[i], alf[i]);

	// Print summary of used blueprints.
	printf("----------\n%i types of blueprints used:\n", numblp);
	for(i = 0; i < numblp; i++)
		printf("%s (%ix)\n", blp[i], npblp[i]);
	printf("*/\n");

	for(i = 0; i < numalf; i++)
		if(alf[i]) free(alf[i]);

	return 0;
}