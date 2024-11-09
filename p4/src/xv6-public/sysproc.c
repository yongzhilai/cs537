#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }

    myproc()-> to_sleep=n-1;// if asked to sleep 0 round code breaks
    sleep(&ticks, &tickslock);
    /*if (myproc()->pid>3){
      cprintf("pid %d slept %d tick\n",myproc()->pid, ticks-ticks0);
    }*/

  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int sys_settickets(void) {
  int pid;
  int tickets;
  if(argint(0, &pid) < 0 || argint(1, &tickets)<0) {
    return -1;
  }

  if( tickets < 1) {
    return -1;
  }


  return settickets(pid,tickets);
}


int sys_srand(void) {
  int seed;
  if(argint(0, &seed) < 0) {
    return -1;
  }
  
  extern uint rseed;
  rseed=(uint)seed;
  return 0;
}

int sys_getpinfo(void) {

  struct pstat *st;
  if(argptr(0, (void*)&st, sizeof(struct pstat*)) < 0) {
    return -1;

  }

  return getpinfo(st);
}


