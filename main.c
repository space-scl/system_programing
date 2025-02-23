#include "learning.h"

int main (int argc, char* argv[])
{
    // copy a.c to b.c by using open, read, write and close file I/O routines
    /*
    if (argc != 3) {
        printf("Need to specify Afile and Bfile\n");
        return -1;
    }

    return copyFile(argv[1], argv[2]);
    */

    if (argc !=3 ) {
        printf("Nedd to specify source directory and target directory\n");
        return -1;
    }

    return copyDir(argv[1], argv[2]);
}



