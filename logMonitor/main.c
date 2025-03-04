#include "monitor.h"


int main (int argc, char* argv[])
{
    if (argc < 2) {
        printf ("Need to specify log path\n");
    }
    initMonitor ();
    monitor (argv[1]);

    return 0;
}



