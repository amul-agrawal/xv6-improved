#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "traps.h"

// Interrupt descriptor table (shared by all CPUs).
extern struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
extern struct spinlock tickslock;
extern uint ticks;


extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;


extern int nextpid;
extern void forkret(void);
extern void trapret(void);

void
scheduler(void)
{
  //  cprintf("here\n");
  //  exit();
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  while (1) {
    // Enable interrupts on this processor
    sti();

    acquire(&ptable.lock);

    // cprintf("here\n");
    
    // Age the processes
    for (int i = 1; i < NUM_QUEUES; i++) {
      moveup(&queues[i], &queues[i - 1], AGE_LIMIT);
    }

    // Loop through the queues to find a process to run
    p = 0;
    for (int i = 0; i < NUM_QUEUES; i++) {

      if (size(queues[i]) != 0) {
        p = queues[i]->p;
        queues[i] = pop(queues[i]);
        break;
      }
    }

    if (p == 0 ||  p->killed == 1 || p->state != RUNNABLE) {
      release(&ptable.lock);
      continue;
    }

    p->cur_timeslices++;
    p->n_run++;


    // Switch to chosen process.  It is the process's job
    // to release ptable.lock and then reacquire it
    // before jumping back to us.
    c->proc = p;
    switchuvm(p);
    p->state = RUNNING;

    swtch(&(c->scheduler), p->context);
    switchkvm();

    // Process is done 
    c->proc = 0;

    if (p != 0 && p->state == RUNNABLE) {
      if (p->is_cpu_heavy) {
        if (p->queue != NUM_QUEUES-1) {
          p->queue++;
        }
      }
      p->cur_timeslices = 0;
      p->age_time = ticks;
      p->is_cpu_heavy = 0;
      // queues[p->queue] = push(queues[p->queue], p);
    }

    release(&ptable.lock);
  }
}
