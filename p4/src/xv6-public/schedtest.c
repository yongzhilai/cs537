/*
 * schedtest.c
 * A program that creates two loop processes with given timeslices and that
 * sleep for a specified amount of time before running their loops. The program
 * then sleeps and prints out the compticks of both loop processes
 */

#include "types.h"
#include "user.h"
#include "pstat.h"
#include "param.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf(2, "usage: schedtest <sliceA> <sleepA> <sliceB> <sleepB> <sleepParent>\n");
        exit();
    }
    
    int  ticketsA    = atoi(argv[1]);
    int  sleepA      = atoi(argv[2]);
    int  ticketsB    = atoi(argv[3]);
    int  sleepB      = atoi(argv[4]);
    int  sleepParent = atoi(argv[5]);

    if (ticketsA <= 0 || sleepA < 0 || ticketsB <= 0 || sleepB < 0 || sleepParent <= 0) {
        printf(1, "schedtest: arguments must all be positive\n");
        exit();
    }

    int pidA = fork();
    char *argsA[3] = {"loop", argv[2], 0};
    if (pidA == 0) {  // child
        exec(argsA[0], argsA);
        printf(1, "schedtest: exec A failed\n");
        exit();
    } else {
        settickets(pidA, ticketsA);
    }

    // fork B
    int pidB = fork();
    char *argsB[3] = {"loop", argv[4], 0};
    if (pidB == 0) {
        exec(argsB[0], argsB);
        printf(1, "schedtest: exec B failed\n");
        exit();
    } else {
        settickets(pidB, ticketsB);
    }

    // parent
    if (pidA < 0 || pidB < 0) {
        printf(1, "schedtest: forking A or B failed\n");
        if (pidA > 0) kill(pidA);
        if (pidB > 0) kill(pidB);
        exit();
    }

    // sleep for sleepParent ticks
    sleep(sleepParent);

    // get pstat of the two children
    struct pstat pstat;
    int rc = getpinfo(&pstat);
    if (rc != 0) {
        printf(1, "schedtest: getpinfo failed\n");
        kill(pidA);
        kill(pidB);
        exit();
    }

    // prints number of ticks both children were run for
    int runticksA = -1, runticksB = -1;
    for (int i = 0; i < NPROC; ++i) {
        if (pstat.pid[i] == pidA)
            runticksA = pstat.runticks[i];
        if (pstat.pid[i] == pidB)
            runticksB = pstat.runticks[i];
    }

    if (runticksA == -1 || runticksB == -1) {
        printf(1, "schedtest: couldn't find children in pstat\n");
        kill(pidA);
        kill(pidB);
        exit();
    }

    if (runticksB == 0) {
        printf(1, "schedtest: B didn't run\n");
        kill(pidA);
        kill(pidB);
        exit();
    } else {
        printf(1, "%d %d: %d\n", runticksA, runticksB, runticksA/runticksB);
    }

    // wait for children to finish
    int pid_w1 = wait();
    int pid_w2 = wait();
    if ((pid_w1 != pidA && pid_w1 != pidB)
      || (pid_w2 != pidA && pid_w2 != pidB)) {
        printf(1, "schedtest: wait error\n");
        exit();
    }

    exit();  // end program
}
