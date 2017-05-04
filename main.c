#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sched.h>
#include "queue.h"

extern void sched_fifo(struct queue* proc_queue);
extern void sched_rr(struct queue* proc_queue);
extern void sched_sjf(struct queue* proc_queue, size_t proc_number);
extern void sched_psjf(struct queue* proc_queue, size_t proc_number);

int main(){
	struct sched_param sched_param;
    cpu_set_t cpu_set;

    char sched_policy_buf[1024];
    char proc_number_buf[1024];
    char proc_info_buf[1024];
    size_t proc_number;
    //set parent to FIFO
    sched_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
    sched_setscheduler(0, SCHED_FIFO, &sched_param) < 0;
    //set parent to the first CPU
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);

    //get the scheduling policy
    fgets(sched_policy_buf, sizeof(sched_policy_buf), stdin);
    strtok(sched_policy_buf, " \r\n");
    //get the number of processes
    fgets(proc_number_buf, sizeof(proc_number_buf), stdin);
    strtok(proc_number_buf, " \r\n");
    sscanf(proc_number_buf, "%zu", &proc_number);
    //construct the process queue
    struct queue proc_queue;
    size_t i;
    unsigned max_ready_time = 0;
    for(i = 0; i < proc_number; i++){
        struct proc *new_proc;
        new_proc = malloc(sizeof(struct proc));
        char* tmp;
        fgets(proc_info_buf, sizeof(proc_info_buf), stdin);
        //name
        tmp = strtok(proc_info_buf, " \r\n");
        strncpy(new_proc->name, tmp, sizeof(new_proc->name));
        //ready time
        tmp = strtok(NULL, " \r\n");
        sscanf(tmp, "%u", &new_proc->ready_time);
        //execution time
        tmp = strtok(NULL, " \r\n");
        sscanf(tmp, "%u", &new_proc->exec_time);
        if(new_proc->ready_time >= max_ready_time){
            enqueue(&proc_queue, new_proc);
            max_ready_time = new_proc->ready_time;
        } else{
            enqueue_by_ready_time(&proc_queue, new_proc, new_proc->ready_time);
        }
    }

    //test process queue
/*    for(i = 0; i < proc_number; i++){
        printf("%s,%u,%u\n", proc_queue.head->name, proc_queue.head->ready_time, proc_queue.head->exec_time);
        dequeue(&proc_queue);
    }*/
    //call corresponding scheduling function
    if(strncmp(sched_policy_buf, "FIFO", sizeof(sched_policy_buf)) == 0){
        sched_fifo(&proc_queue);
    }
    else if(strncmp(sched_policy_buf, "RR", sizeof(sched_policy_buf)) == 0){
        sched_rr(&proc_queue);
    }
    else if(strncmp(sched_policy_buf, "SJF", sizeof(sched_policy_buf)) == 0){
        sched_sjf(&proc_queue, proc_number);
    }
    else{
        sched_psjf(&proc_queue, proc_number);
    }

    //destroy_queue(&proc_queue);
    return 0;
}
