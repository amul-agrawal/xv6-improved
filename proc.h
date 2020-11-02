// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

// 8c60c451ba0933cf2b4c7e40967bfa38
#define SCHED_RR 0
#define SCHED_FCFS 1
#define SCHED_PBS 2
#define SCHED_MLFQ 3

extern struct cpu cpus[NCPU];
extern int ncpu;
#define NUM_QUEUES (int)5     // 8c60c451ba0933cf2b4c7e40967bfa38
//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)

  int ctime;              // Start time of the process 8c60c451ba0933cf2b4c7e40967bfa38
  int etime;                // End time of the process
  int rtime;                // Run time of the process

  int priority;               // 8c60c451ba0933cf2b4c7e40967bfa38
  int n_run;          
  int q[NUM_QUEUES];

  int n_run_priority;         // number of times it ran on a particular priority

  int time_slices;
  int cur_timeslices;
  int is_cpu_heavy;                 // process used its timespace, and now we need to move it down.
  int queue;                  // queue on which process is waiting
  int age_time;               // entry time in a queue
  int wtime;
};

struct node {
    struct node *next;
    struct proc *p;
    int use;
};
struct node avl_nodes[NPROC];
struct node *queues[NUM_QUEUES];  // lower indices are higher priority

#define TIMESLICE(t) (1<<t)
// int TIMESLICE[] = {1, 2, 4, 8, 16};
#define AGE_LIMIT 25

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

