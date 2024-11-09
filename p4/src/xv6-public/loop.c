/*
 * loop.c
 * A program that sleeps for a certain amount of
 * time, then runs a very large workload
 */

#include "types.h"
#include "user.h"

/*
 * Main function of the program. Puts the program to sleep for a given amount
 * time, then runs a very large loop.
 */
int main (int argc, char *argv[]) {
    int sleepT;

    if (argc != 2) {
        printf(2, "usage: loop <sleepT>\n");
        exit();
    }

    sleepT = atoi(argv[1]);  // get the number of sleep ticks

    if (sleepT < 0) {
        printf(2, "loop: sleepT needs to be positive\n");
        exit();
    }

    sleep(sleepT);  // go to sleep for sleepT ticks

    volatile int i = 0, j = 0;

    // very big loop that runs for a long time
    while (i < 800000000) {
        j = i * j + 1;
        i++;
    }

    exit();  // end program
}
