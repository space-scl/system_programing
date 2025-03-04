#include "monitor.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

int numOflog;
logFileStatus logFiles[MAX_LOG_FILE];

void initMonitor ()
{
    int i;
    for (i = 0; i < MAX_LOG_FILE; i++) {
        logFiles[i].curFileoffset = 0;
        logFiles[i].lastFileOffset = 0;
        memset((void *) (logFiles[i].name), 0, 100);
    }
}

int checkFileTrack (char* fileName)
{
    int i;
    int len = strlen(fileName);
    
    for (i = 0; i < MAX_LOG_FILE; i++) {
        if (memcmp(fileName, logFiles[i].name, len) == 0) {
            return 0;
        }
        if (logFiles[i].name[0] == '\0') {
            memcpy (logFiles[i].name, fileName, len);
            return 0;
        }
    }

    return -1;
}

int monitor (char* dirPath)
{
    DIR *dirP;
    struct dirent* direntP;
    char filePath[100];
    size_t len = strlen(dirPath);

    memcpy(filePath, dirPath, len);
    dirP = opendir(dirPath);

    if (dirP == NULL) {
        return -1;
    }

    while (1) {
        direntP = readdir (dirP);
        // Skip file name start with .
        if (direntP->d_name[0] == '.') {
            continue;
        }

        // Check if the file is tracked
        if (checkFileTrack (direntP->d_name) != 0) {
            return -1;
        }
        memcpy(((char*) filePath) + len, direntP->d_name, strlen(direntP->d_name));

        printf("name: %s\n", filePath);

        if (direntP == NULL) {
            //return 0;
        }
    }
}


