#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "pstat.h"

void sleep_wake() {
    int pid=fork();
    if (pid == 0) {
        printf(1,"pid %d will sleep for 5 seconds\n",getpid());
        sleep(5);

    }
    else {
        wait();
    }



}
void set_ticket_test() {
        int result=settickets(0,5);
        printf(1,"settickets() returned %d\n",result);

}

void srand_test() {
    srand(0);
    //settickets(0,5);



}
void getpinfo_test() {
    // test method might have issues since pid!='\0' is very sketchy
    struct pstat st;
    int result=getpinfo(&st);
    
    printf(1,"getpinfo() returned %d\n",result);
    int i;
    /*int count = 0;
    for(i=0; st.pid[i]!='\0'; i++)
    {
        count++;
    }*/

    for(i=0;i < 4; i++) {
        printf(1,"inuse: %d ",st.inuse[i]);
        printf(1,"pid: %d ",st.pid[i]);
        printf(1,"tickets: %d ",st.tickets[i]);
        printf(1,"runticks: %d ",st.runticks[i]);
        printf(1,"boostsleft: %d ",st.boostsleft[i]);
        printf(1,"\n");



    }

}

void lottery_test () {


    
}
int main (int rag, char * argv[]){
        char * greetings="this is mytest!";
        printf(1,"%s\n",greetings);
        //sleep_wake();
        //set_ticket_test();
        //srand_test();
        getpinfo_test();
        exit();


}
