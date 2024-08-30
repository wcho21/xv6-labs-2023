#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // interface: pgaccess(address, num_pages, result)
  // - address: starting VA of the first user page
  // - num_pages: the number of pages to check
  // - result: a pointer to bitmask data structure to encode accessed pages

  // get call arguments
  uint64 address;
  argaddr(0, &address);

  int num_pages;
  argint(1, &num_pages);

  uint64 result;
  argaddr(2, &result);

  // fail if bad arguments
  if (num_pages <= 0) {
    return -1;
  }
  if (num_pages > 32) { // if too large
    return -1;
  }

  // temp bitmask to encode accessed pages
  unsigned int accessed = 0;

  pagetable_t pagetable = myproc()->pagetable;
  for (int i = 0; i < num_pages; i++) {
    uint64 page_address = address + i*PGSIZE;
    pte_t *pte = walk(pagetable, page_address, 0);

    if ((*pte & PTE_A) == 0) { // if not accessed
      continue;
    }

    accessed |= (1U << i);

    // clear accessed flag
    *pte &= ~(PTE_A);
  }

  // return to user
  copyout(pagetable, result, (char *)&accessed, sizeof(unsigned int));

  return 0;
}
#endif

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
