#define _GNU_SOURCE
#define __NR_sys_proj1_stat_printk 314
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "queue.h"
#include "timer.h"

void sched_fifo(struct queue *proc_queue){
	unsigned cur_time = 0;
	struct proc *cur_proc;
	cur_proc = proc_queue->head;
	while(cur_proc != NULL){
		//the next procss is not ready
		if(cur_proc->ready_time > cur_time){
			Wait_time(cur_proc->ready_time - cur_time);
			cur_time = cur_proc->ready_time;
		}
		//run the procss
		cur_proc->pid = fork();
		if(cur_proc->pid < 0){
			printf("Fork Failed!");
		}
		else if(cur_proc->pid == 0){//child process
			printf("%s %ld\n", cur_proc->name, (long)getpid());
			while(1);
		}
		else{//parent process
			clock_gettime(CLOCK_REALTIME, &cur_proc->stat_start_time);
			Wait_time(cur_proc->exec_time);
			clock_gettime(CLOCK_REALTIME, &cur_proc->stat_end_time);
			kill(cur_proc->pid, SIGKILL);
			proj1_stat_printk(cur_proc->pid, &cur_proc->stat_start_time, &cur_proc->stat_end_time);
			cur_time += cur_proc->exec_time;
//			printf("%ld.%ld $ld.%ld\n", cur_proc->stat_start_time.tv_sec, cur_proc->stat_start_time.tv_nsec, cur_proc->stat_end_time.tv_sec, cur_proc->stat_end_time.tv_nsec);
		}
		cur_proc = cur_proc->next;
	}

}
