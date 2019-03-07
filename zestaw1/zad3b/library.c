#include "library.h"


struct Array *createArray(int arraySize){
    //TODO(1) : check conditions of arraySize and blockSize, smaller than max and larger than 0
    struct Array *blockArray = calloc(1, sizeof(struct Array));
    blockArray->array = (char **)calloc((size_t)arraySize, sizeof(char*));

    for(int i = 0; i < arraySize; i++)
        blockArray->array[i] = NULL;

    blockArray->arraySize = arraySize;
    blockArray->directory = NULL;
    blockArray->file = NULL;
    return blockArray;
}

// setting directory to search
void setDirectory(struct Array * blockArray, char * directory){
    blockArray->directory = directory;
}
// setting name of file to search
void setFile(struct Array * blockArray, char *file){
    blockArray->file = file;
}

// deleting block on given position
void deleteBlockAtIndex(struct Array *blockArray, int index){
    if(index >= 0 && index < blockArray->arraySize)
        if(blockArray->array != NULL && blockArray->array[index] != NULL){
            free(blockArray->array[index]);
            blockArray->array[index] = NULL;
        }
}

void deleteArray(struct Array* blockArray){
    if(blockArray == NULL)
        return;
    for (int i = 0; i < blockArray->arraySize; i++)
        deleteBlockAtIndex(blockArray, i);

    free(blockArray->array);
    blockArray->array = NULL;
}


/// before searching for a file you need to set directory and file you want to search
void searchForFile(struct Array * blockArray){

    char command[1000] = "find ";

    if(blockArray->directory != NULL){
        strcat(command, blockArray->directory);
    }

    if(blockArray->file != NULL){
        strcat(command, " -name ");
        strcat(command, blockArray->file);
    }

    if(blockArray->outputFile != NULL){
        strcat(command, " >> ");
        strcat(command, blockArray->outputFile);
    }

    system(command);
    saveIntoArray(blockArray);
}


void setResultFile(struct Array * blockArray, char * resultFile){
    blockArray->outputFile = resultFile;
}

extern int saveIntoArray(struct Array *blockArray){
    FILE *fp;
    long lSize;


    fp = fopen ( blockArray->outputFile , "rb" );
    if( !fp ) {
        printf("Error. Result file cannot be open.");
        return 1;
    }
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    int firstEmptyBlock = 0;
    while(blockArray->array[firstEmptyBlock] != NULL){
        firstEmptyBlock++;
        if(firstEmptyBlock == blockArray->arraySize){
            printf("Array is full.");
            exit(1);
        }
    }

    blockArray->array[firstEmptyBlock] = (char *)calloc(1, lSize+1);

    if(!blockArray->array[firstEmptyBlock])
        fclose(fp),fputs("memory alloc fails ",stderr),exit(1);

/* copy the file into the buffer */

    if( 1 != fread( blockArray->array[firstEmptyBlock] , lSize, 1 , fp) )
        fclose(fp),free(blockArray->array[firstEmptyBlock]),fputs("entire read fails",stderr),exit(1);

/* do your work here, buffer is a string contains the whole text */

    fclose(fp);
    return firstEmptyBlock;
}
