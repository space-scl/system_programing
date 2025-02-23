#ifndef LEARNING_H_
#define LEARNING_H_

#include <stdio.h>

// Read/Write 1024 bytes most at once and do not exceed 1024 repetitions
#define   READ_WRITE_UNIT_SIZE 1024
#define   MAX_RDWR_REPETITION  1024

int copyFile(char* Afile, char* Bfile);

int copyDir (char* pathA, char* pathB);

#endif
