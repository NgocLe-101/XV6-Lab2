#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"
#include "syscall.h"
#include "kalloc.h"


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


// // sysinfotest
// int sys_sysinfo(void) {
//   printf("syscall sysinfo called\n");
//   struct sysinfo info;
//   struct sysinfo *u_info;

//   // lay tham so tu user space
//   if (argstr(0, (char* )&u_info, sizeof(*u_info))<0)
//     return -1;

//   // lay thong tin he thong
//   info.freemem = get_freemem();
//   info.nproc = get_nproc();

//   // copy ket qua tra ve user space
//   if(copyout(myproc()->pagetable, (uint64)u_info, (char *)&info, sizeof(info))<0)
//     return -1;

//   return 0;

// }
uint64
sys_sysinfo(void) {
    struct sysinfo info;

    // Lấy thông tin
    info.freemem = get_freemem();
    info.nproc = get_nproc();

    uint64 dst;
    argaddr(0, &dst); 

    if (dst == 0) {
        //printf("error:  null pointer\n");
        return -1;
    }

    // Sao chép dữ liệu từ kernel sang user space
    if (copyout(myproc()->pagetable, dst, (char *)&info, sizeof(info)) < 0) {
        //printf("error: failed to copyout\n");
        return -1;
    }

    //printf("freemem: %ld\n", info.freemem);
    //printf("nproc: %ld\n", info.nproc);

    return 0;
}


