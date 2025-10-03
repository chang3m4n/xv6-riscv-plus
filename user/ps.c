// ps.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char* state_str[] = {
    "UNUSED",   // 0
    "USED",     // 1  
    "SLEEP",    // 2
    "RUNNABLE", // 3
    "RUN",      // 4
    "ZOMBIE"    // 5
};

int
main(void)
{
  struct proc_info info[64];
  int count = getprocinfo(info);
  
  printf("\nPID\tPRIO\tSTATE\tRUN\tWAIT\tTURN\n");
   for(int i = 0; i < count; i++){
        if(info[i].pid > 1) {  // 跳过init进程
            const char* state = (info[i].state >= 0 && info[i].state < 6) 
                ? state_str[info[i].state] 
                : "UNKNOWN";
            
            printf("%d\t%d\t%s\t%d\t%d\t%d\n", 
                info[i].pid, 
                info[i].priority, 
                state,
                info[i].total_cpu_time, 
                info[i].total_wait_time,
                info[i].turnaround_time);
        }
    }
  exit(0);
}