#ifndef ZAD1_LIBRARY_H
#define ZAD1_LIBRARY_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>



struct Array{
    char ** array;
    int arraySize;
    char * directory;
    char * file;
    char * outputFile;
};


extern struct Array *createArray(int arraySize);
extern void addBlock();
extern void setDirectory(struct Array *blockArray, char *directory);
extern void setFile(struct Array *blockArray, char * file);
extern void setResultFile(struct Array *blockArray, char * file);
extern void deleteBlockAtIndex(struct Array *blockArray, int index);
extern void deleteArray(struct Array *blockArray);
extern void searchForFile(struct Array *blockArray);
extern int saveIntoArray(struct Array *blockArray);

#endif
