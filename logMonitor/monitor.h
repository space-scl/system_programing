#ifndef MONITOR_H_
#define MONITOR_H_

#include <stdio.h>

#define  MAX_LOG_FILE   10


typedef struct {
    char name[100];
    int  lastFileOffset;
    int  curFileoffset;
} logFileStatus;

void initMonitor ();
int monitor (char* dirPath);

#endif  // MONITOR_H_
