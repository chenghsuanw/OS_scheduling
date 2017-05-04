#define _GNU_SOURCE
#define __NR_sys_proj1_stat_printk 314
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "proc.h"
#include "timer.h"
#include "queue.h"

int sched_sjf(struct queue *input_queue, int proc_number){

  size_t  i,j;
  unsigned cur_time;
  struct proc *proc[proc_number+2];
  struct proc *temp_proc;
  struct proc *selected_proc;
	struct proc *cur_proc;
  struct queue *ready_queue;

  //Initialization
  for(i=0;i<=proc_number+1;i++){
    if(i==1){
      proc[i] = input_queue->head;
    }
    else if(i==0 || i==proc_number+1){
      proc[i] = NULL;
    }
    else{
      proc[i] = proc[i-1]->next;
    }
  }

  struct proc *head;
  head = proc[1];
  //Sort to get ready queue in array
  cur_time = 0;
  for(i=1;i<=proc_number;i++){
    for(j=i+1;j<=proc_number;j++){
      if(proc[i]->ready_time <= cur_time && proc[j]->ready_time <= cur_time){
        if(proc[i]->exec_time > proc[j]->exec_time){
          if(i == 1){
            if(j == 2){
              proc[1]->next = proc[3];
              proc[2]->next = proc[1];
              head = proc[2];
            }
            else{
              proc[1]->next = proc[j+1];
              proc[j]->next = proc[2];
              head = proc[j];
            }
          }
          else if(j == i+1){
            proc[i-1]->next = proc[j];
            proc[j]->next = proc[i];
            proc[i]->next = proc[j+1];
          }
          else{
            proc[i-1]->next = proc[j];
            proc[j]->next = proc[i+1];
            proc[j-1]->next = proc[i];
            proc[i]->next = proc[j+1];
          }
          temp_proc = proc[j];
          proc[j] = proc[i];
          proc[i] = temp_proc;
        }
      }
      else{
        break;
      }
    }
    if(i != 1){
      cur_time += proc[i-1]->next->exec_time;
    }
    else{
      cur_time = head->exec_time;
    }
  }

  //running processes
  cur_time = 0;
  cur_proc = head;
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
			//printf("%ld.%ld ", cur_proc->stat_start_time.tv_sec, cur_proc->stat_start_time.tv_nsec);
      //printf("%ld.%ld\n", cur_proc->stat_end_time.tv_sec, cur_proc->stat_end_time.tv_nsec);
		}
		cur_proc = cur_proc->next;
	}
  return 0;
}
