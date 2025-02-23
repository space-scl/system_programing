#include "learning.h"

int main (int argc, char* argv[])
{
    if (argc != 3) {
        printf("Need to specify Afile and Bfile\n");
    }

    return copyAtoB(argv[1], argv[2]);
}



