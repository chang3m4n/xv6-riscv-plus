#include "kernel/types.h"
#include "user/user.h"

struct proc_info info[64];

// 状态数值到字符串的映射表
static char* state_str[] = {
    "UNUSED",   // 0
    "USED",     // 1  
    "SLEEP",    // 2
    "RUNBLE",   // 3 (RUNNABLE)
    "RUN",      // 4
    "ZOMBIE"    // 5
};

void print_info() {
    int count = getprocinfo(info);
    printf("\nPID\tPRIO\tSTATE\tRUN\tWAIT\tTURN\n");
    for(int i = 0; i < count; i++){
        if(info[i].pid > 3) {  // 跳过init进程
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

int main(void) {
    printf("Priority Scheduler Test\n");
    for (int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {            
            if (i < 2) { 
                volatile long m;
                for (m = 0; m < 30000000000; m++); 
                printf("process %d finished\n", getpid());
            } else if(i<4){
                volatile long m; 
                for (m = 0; m < 3000000000; m++); 
                pause(5);
                printf("process %d finished\n", getpid());
            }else{ 
                for(int m=0; m < 10000000000; m++);
                printf("process %d finished\n", getpid());
            }
            exit(0);
        } else if (pid < 0) {
            fprintf(2, "fork failed\n");
            exit(1);
        }
        printf("Created PID: %d\n", pid);
    }
    
    // 监控进程状态（每20个tick检查一次）
    for (int i = 0; i < 10; i++) {
        pause(20);
        printf("\n=== Check %d (tick: %d) ===\n", i, uptime());
        print_info();
    }
    
    // 等待所有子进程结束
    for (int i = 0; i < 5; i++) {
        int pid = wait(0);
        if (pid > 0) {
           printf("Process %d exited\n", pid);
        }
    }
    
    printf("Test completed\n");
    exit(0);
}
