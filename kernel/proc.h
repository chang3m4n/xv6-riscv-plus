// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
struct trapframe {
  /*   0 */ uint64 kernel_satp;
  /*   8 */ uint64 kernel_sp;
  /*  16 */ uint64 kernel_trap;
  /*  24 */ uint64 epc;
  /*  32 */ uint64 kernel_hartid;
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;
  
  // 调度统计信息
  int priority;               // 当前优先级 (1-20, 越小越高)
  int create_time;            // 进程创建时间
  int turnaround_time;        // 周转时间
  int cpu_time;               // 当前运行时间段累积的CPU时间
  int wait_time;              // 当前等待时间段累积的等待时间
  int total_cpu_time;         // 总CPU运行时间
  int total_wait_time;        // 总等待时间
  int last_update;            // 最后一次更新时间

  // p->lock must be held when using these:
  enum procstate state;       // Process state
  void *chan;                 // If non-zero, sleeping on chan
  int killed;                 // If non-zero, have been killed
  int xstate;                 // Exit status to be returned to parent's wait
  int pid;                    // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;        // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;              // Virtual address of kernel stack
  uint64 sz;                  // Size of process memory (bytes)
  pagetable_t pagetable;      // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;     // swtch() here to run process
  struct file *ofile[NOFILE]; // Open files
  struct inode *cwd;          // Current directory
  char name[16];              // Process name (debugging)
};

// proc_info for getprocinfo
struct proc_info {
    int pid;
    int priority;
    int current_cpu_time;
    int current_wait_time;
    int total_cpu_time;
    int total_wait_time;
    int turnaround_time;
    int state;
    char name[16];
};

// 函数声明
void update_all_process_times(void);
void on_state_change(struct proc *p, enum procstate old_state, enum procstate new_state);