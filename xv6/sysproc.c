#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "processInfo.h"

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
int
sys_hello(void)
{
    cprintf("helloxv6\n");

    return 0;
}
int
sys_hello_name(void)
{
    char *name;

    if(argstr(0, &name) < 0)
        return -1;

    cprintf("hello %s\n", name);

    return 0;
}
int
sys_get_num_proc(void)
{
    int count = get_num_proc();

    cprintf("Total Number of Active Processes: %d\n", count);
    
    return count;
}
int
sys_get_max_pid(void)
{
    int max_pid = get_max_pid();

    cprintf("Maximum PID: %d\n", max_pid);

    return max_pid;
}
int
sys_get_proc_info(void)
{
    int max_pid = get_max_pid();

    cprintf("PID   PPID  SIZE   Number of Context Switch\n");

    for(int pid = 1; pid <= max_pid; pid++) {
        struct processInfo pinfo;

        if(get_proc_info(pid, &pinfo) == -1)
            continue;
        
        cprintf("%d     %d     %d  %d\n", 
                pid, pinfo.ppid, pinfo.psize, pinfo.numberContextSwitches);
    }

    return 0;
}
int
sys_set_prio(void)
{
    struct proc *p = myproc();
    int priority;
    
    if(argint(0, &priority) < 0)
        return -1;

    p->priority = priority;

    return 0;
}
int
sys_get_prio(void)
{
    struct proc *p = myproc();
    int priority = p->priority;

    return priority;
}

