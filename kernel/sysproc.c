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
  return 0; // not reached
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
  if (growproc(n) < 0)
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
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 start_va;
  int num_of_pages;
  uint64 buffer[128]; // 128 * 64 is upper limit for num of pages that can be scanned
  uint64 user_buffer_addr;
  pte_t *pte;
  int i;
  struct proc *p = myproc();
  int offset = 0;
  int buffer_index = 0;
  int has_been_accessed = 0;

  argaddr(0, &start_va);
  argint(1, &num_of_pages);
  argaddr(2, &user_buffer_addr);

  if (num_of_pages > 128 * 64)
  {
    return 0; // too many pages
  }

  // zero the buffer
  for (i = 0; i < 128; i++)
  {
    buffer[i] = 0;
  }

  for (i = 0; i < num_of_pages; i++)
  {
    pte = walk(p->pagetable, start_va, 0);

    if (*pte & PTE_A)
    {
      has_been_accessed = 1;
    }
    else
    {
      has_been_accessed = 0;
    }

    buffer[buffer_index] = buffer[buffer_index] | (has_been_accessed << offset);
    offset += 1;

    if (offset == 64)
    {
      offset = 0;
      buffer_index++;
    }

    if (*pte & PTE_A)
    {
      *pte = *pte & (~PTE_A); // zero the PTE_A bit
    }

    start_va += 4096; // move the va by one page
  }

  if (copyout(p->pagetable, user_buffer_addr, (char *)&buffer, (buffer_index * 8) + ((1 + (offset - 1) / 8))) < 0)
  {
    return -1;
  }

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
