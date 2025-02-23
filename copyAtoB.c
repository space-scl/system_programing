#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <stdio.h>
#include <string.h>
#include "learning.h"

int copyFile(char* Afile, char* Bfile)
{
    int fdA;
    int fdB;
    int i = 0;;
    char buf[READ_WRITE_UNIT_SIZE];
    ssize_t   readSize;
    ssize_t   writeSize;

    if (Bfile[strlen(Bfile) - 1] == '.') {
        return -1;
    }

    // open a.c for reading and file offset is set to the beginning of the file
    fdA = open(Afile, O_RDONLY);
    if (fdA == -1) {
        return -1;
    }

    // the file offset is set to the end of the file prior to write
    fdB = open(Bfile, O_RDWR | O_APPEND | O_CREAT, 0666);
    if (fdB == -1) {
        close(fdA);
        return -1;
    }

    while (1) {
        // read from a.c
        readSize = read(fdA, buf, READ_WRITE_UNIT_SIZE);
        if (readSize == -1) {
            break;
        }
        // If readSize is 0, it means we have reached the end of the file
        if (readSize == 0) {
            break;
        }
        // write to b.c
        writeSize = write (fdB, buf, readSize);
        if (writeSize == -1) {
            break;
        }

        if (++i > MAX_RDWR_REPETITION) {
            break;
        }
    }

    close(fdA);
    close(fdB);

    return 0;
}

char* concatenateStr (char* buff, char* str1, char* str2)
{
    int len = strlen(str1);
    memcpy(buff, str1, strlen(str1));

    if (str1[strlen(str1) - 1] != '/') {
        buff[strlen(str1)] = '/';
        len += 1;
    }

    memcpy(buff + len, str2, strlen(str2) + 1);

    return buff;
}

int copyDir (char* pathA, char* pathB)
{
    DIR* dirP;
    struct dirent* dirEnt;
    char srcDir[100];
    char destDir[100];
    int status;

    dirP = opendir(pathA);

    if (dirP == NULL) {
        return -1;
    }

    if (opendir(pathB) == NULL) {
        if (mkdir(pathB, 0777) == -1) {
            printf("Fail to create directory %s\n", pathB);
            closedir(dirP);
            return -1;
        }
    }

    while (1) {
        dirEnt = readdir(dirP);

        if (dirEnt == NULL) {
            closedir (dirP);
            return -1;
        }

        concatenateStr (srcDir, pathA, dirEnt->d_name);
        concatenateStr (destDir, pathB, dirEnt->d_name);
        status = copyFile(srcDir, destDir);
        if (status == -1) {
            continue;
        }
        printf(" %s --> %s\n", srcDir, destDir);

        if (status != 0) {
            printf("Fail to copy file\n");
            closedir(dirP);
            return -1;
        }

    }

    return 0;
}


