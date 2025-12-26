#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <stdio.h>

typedef struct VMFile {
    FILE* fp;
    char* fileName;
    // char* filePath;
} VMFile;

typedef struct VMInputSet {
    VMFile *fpArray;
    int numFiles;
} VMInputSet;

// The function here will detect if name is a path or a filename.
VMInputSet* manageFiles(const char* name);

#endif