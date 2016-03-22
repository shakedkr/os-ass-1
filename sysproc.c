#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
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
int sys_history(void)
{
    char *buffer;
    int historyId;
     if(argstr(0, &buffer) < 0 || argint(1,&historyId) < 0)
         return -1;
    return history(buffer,historyId);
}

int 
sys_wait2(void){
  /*
    int retime, rutime, stime;
    if ( argint(0,&retime)<0 || argint(1,&rutime)<0 || argint(2,&stime)<0  )
        return -1;
    return wait2(&retime,&rutime,&stime);
*/
    
    char *retime, *rutime, *stime;
    if ( argptr(0,&retime,4)<0 || argptr(1,&rutime,4)<0 || argptr(2,&stime,4)<0  )
        return -1;
    return wait2((int *)retime,(int *)rutime,(int*)stime);
    
    
}

int sys_yield(){
    yield();
    return 0;
}


int sys_bla (void){
  /* 
    char* buffer;
    if (argptr(0,&buffer,128)<0)
        return -1;
    return bla(buffer);
*/
    return 0;
}





