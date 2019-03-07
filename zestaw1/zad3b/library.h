#ifndef ZAD1_LIBRARY_H
#define ZAD1_LIBRARY_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define MAX_ARRAY_SIZE 10000000

struct Array{
    char ** array;
    int arraySize;
    char * directory;
    char * file;
    char * outputFile;
};


extern struct Array *createArray(int arraySize); // done
extern void addBlock();
extern void setDirectory(struct Array *blockArray, char *directory); //done
extern void setFile(struct Array *blockArray, char * file); //done
extern void setResultFile(struct Array *blockArray, char * file); //done
extern void deleteBlockAtIndex(struct Array *blockArray, int index); //done
extern void deleteArray(struct Array *blockArray); //done
extern void searchForFile(struct Array *blockArray);
extern int saveIntoArray(struct Array *blockArray);

#endif
