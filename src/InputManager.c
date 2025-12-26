#define _BSD_SOURCE 1
#include "InputManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


char* cleanNamePathAndExtension(const char* name) {
    char* cleanedFileName = strdup(name);
    char* slash = strrchr(cleanedFileName, '/');
    cleanedFileName = slash + 1; 
    *(cleanedFileName + strlen(cleanedFileName) - 3) = '\0';
    return cleanedFileName;
}

// assumes that path input is perfect
VMInputSet* manageFiles(const char* path) {
    struct stat s;
    if (stat(path, &s) != 0) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    VMInputSet* set = (VMInputSet *) malloc(sizeof(VMInputSet));
    
    if (S_ISREG(s.st_mode)) {
        set->fpArray = (VMFile *) malloc(sizeof(VMFile));
        char* cleanedFileName = cleanNamePathAndExtension(path);
        set->fpArray->fileName = cleanedFileName;
        set->fpArray->fp = fopen(path, "r");
        set->numFiles = 1;
        return set;
    } else if (S_ISDIR(s.st_mode)) {
        DIR* dir = opendir(path); 
        struct dirent* d;
        struct stat s;
        int vmFiles = 0;
        int i = 0;
        char* pathWithFileName;
        while ((d = readdir(dir)) != NULL) {
            pathWithFileName = (char *) malloc(strlen(path) + strlen(d->d_name) + 2);
            sprintf(pathWithFileName, "%s/%s", path, d->d_name);
            if (stat(pathWithFileName, &s) == 0) {
                if (S_ISREG(s.st_mode)) {
                    char* extensionCmp = strrchr(d->d_name, '.'); 
                    if (!extensionCmp) {   
                        free(pathWithFileName);
                        continue;
                    }
                    if (strcmp(extensionCmp, ".vm") == 0) {
                        vmFiles++;
                    }
                }
            }
            else {
                perror("stat");
            }
            free(pathWithFileName);
        }

        set->numFiles = vmFiles;
        set->fpArray = (VMFile *) malloc(vmFiles * sizeof(VMFile));
        rewinddir(dir);
        while ((d = readdir(dir)) != NULL) {
            pathWithFileName = (char *) malloc(strlen(path) + strlen(d->d_name) + 2);
            sprintf(pathWithFileName, "%s/%s", path, d->d_name);
            if (stat(pathWithFileName, &s) == 0) {
                if (S_ISREG(s.st_mode)) {
                    char* extensionCmp = strrchr(d->d_name, '.'); 
                    if (!extensionCmp) {   
                        free(pathWithFileName);
                        continue;
                    }
                    if (strcmp(extensionCmp, ".vm") == 0) {
                        set->fpArray[i].fileName = d->d_name;
                        set->fpArray[i].fp = fopen(pathWithFileName, "r");
                        i++;
                    }
                }
            }
            else {
                perror("stat");
            }
            free(pathWithFileName);
        } 
        return set;
    }
    else {
        exit(EXIT_FAILURE);
    }
}