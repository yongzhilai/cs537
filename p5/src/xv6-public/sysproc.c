#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "wmap.h"

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
    sleep(&ticks, &tickslock);
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

int sys_wmap(void) {
  //cprintf("hi this is wmap!");
  int addr;
  int length;
  int flag;
  if(argint(0, &addr) < 0 || argint(1, &length)<0 || argint(2, &flag)<0) {
    return -1;
  }

  if( length < 1) {// size of memory has be be positive
    return -1;
  }

  length = PGROUNDUP(length);



  if (flag & MAP_FIXED) {
    //cprintf("addr: %d\n",addr);
    int tmp=PGROUNDDOWN(addr);
    if(tmp!=addr) {// check if addr is page aligned
      return -1;
    }
    if( MAPBASE > addr || KERNBASE-1 < addr + length) {// must be within wmap space

      return -1;
    }


  }

  return wmap(addr,length,flag);
}

int sys_wunmap(void) {
  int addr;
  if(argint(0, &addr) < 0) {
    return -1;
  }

  int tmp=PGROUNDDOWN(addr);
  if(tmp!=addr) {// check if addr is page aligned
    return -1;
  }
  if( MAPBASE > addr || KERNBASE-1 < addr) {// must be within wmap space

    return -1;
  }



  return wunmap(addr);
}

int sys_getpgdirinfo(void) {
  struct pgdirinfo *pdinfo;
  if(argptr(0, (void*)&pdinfo, sizeof(*pdinfo)) < 0)
    return -1;




  return getpgdirinfo(pdinfo);
}

int sys_getwmapinfo(void) {
  struct wmapinfo *wminfo;
  if(argptr(0, (void*)&wminfo, sizeof(*wminfo)) < 0)
    return -1;




  return getwmapinfo(wminfo);
}
