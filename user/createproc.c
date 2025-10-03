#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct proc_info info[64];
static char* state_str[] = {
    "UNUSED",   // 0
    "USED",     // 1  
    "SLEEP",    // 2
    "RUNNABLE", // 3
    "RUN",      // 4
    "ZOMBIE"    // 5
};

void printinfo(){
    int count = getprocinfo(info);
    printf("\nPID\tPRIO\tSTATE\tRUN\tWAIT\tTURN\n");
    for(int i = 0; i < count; i++){
        if(info[i].pid > 1) { 
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
}

int main(int argc, char *argv[])
{
    int priority = atoi(argv[1]);
    // 创建子进程
    int pid = fork();
    if (pid == 0) {
        // 子进程：设置优先级，运行循环，然后打印信息
        setpriority(getpid(), priority);  
        for (unsigned long long i = 0; i < 100000000; i++);
        exit(0);
    } else if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else {
        printf("Parent process created child %d with priority %d\n", pid, priority);
        pause(1);
        printinfo();
        wait(0);
    }
    exit(0);
}
    