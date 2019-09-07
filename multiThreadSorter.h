#ifndef MULTITHREADSORTER_H
#define MULTITHREADSORTER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <pthread.h>


struct Movie{
	int totalClmns;
	char** columns;
	char** categories;
	char** trimmed;
	struct Movie* next;
};

struct Thread{
	int tid;
	struct Thread* next;
};

struct Arg{
	int status;
	char fname[256];
	char path[PATH_MAX+1];
};

struct Movie* createObject(char* string, int totalClmns);

struct Movie* mergeSort(struct Movie* head, int sortclmn);

struct Movie* merge(struct Movie* h1, struct Movie* h2, int sortclmn);

int checkIfDigit(char* h1, char* h2);

char* trim(char* string);

char** createColumns(char* string, int* totalClmns);

struct Movie* objDump(FILE* file);

void* runnable(void* arg);

#endif