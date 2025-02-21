#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define   READ_WRITE_UNIT_SIZE 1024

int main (int argc, char* argv[])
{
    int fdA;
    int fdB;
    int i;
    char buf[READ_WRITE_UNIT_SIZE];
    ssize_t   readSize;
    ssize_t   writeSize;

    fdA = open("a.c", O_RDONLY);
    if (fdA == -1) {
        return -1;
    }

    // the file offset is set to the end of the file prior to write
    fdB = open("b.c", O_RDWR | O_APPEND);
    if (fdB == -1) {
        return -1;
    }

    while (1) {
        // read from a.c
        readSize = read(fdA, buf, READ_WRITE_UNIT_SIZE);
        if (readSize == -1) {
            return -1;
        }
        // If readSize is 0, it means we have reached the end of the file
        if (readSize == 0) {
            break;
        }
        // write to b.c
        writeSize = write (fdB, buf, readSize);
        if (writeSize == -1) {
            return -1;
        }
    }

    return 0;
}



