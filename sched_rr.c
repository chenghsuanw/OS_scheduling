#define _GNU_SOURCE
#define __NR_sys_proj1_stat_printk 314
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include "queue.h"
#include "timer.h"

// Time quantum of RR scheduler is defined as 500 time units.
#define TIMESLICE_TIME_UNITS 500

int sched_rr(struct queue *proc_queue){
	unsigned current_time = 0;
	unsigned process_num = 0;
	struct proc *cur_proc;
	struct queue *ready_queue;
	struct sched_param sched_param;
	ready_queue = malloc(sizeof(struct queue));
	cur_proc = proc_queue->head;

	while(cur_proc != NULL){
		struct proc* enqueue_proc;
		enqueue_proc = malloc(sizeof(struct proc));		
		strncpy(enqueue_proc->name, cur_proc->name,sizeof(enqueue_proc->name));
		enqueue_proc->status = pid_not_exist;
		enqueue_proc->ready_time = cur_proc->ready_time;
		enqueue_proc->exec_time = cur_proc->exec_time;
		enqueue(ready_queue,enqueue_proc);
		process_num++;
		cur_proc = cur_proc->next;
		while(process_num){
			struct proc temp;
        	temp = dequeue(ready_queue);
        	process_num--;

        	int is_more_than_time_quantum = 0;
			int actual_time_spent;

			//if the process doesn't arrive
			if(temp.ready_time > current_time)
			{
				Wait_time(temp.ready_time - current_time);
				current_time += (temp.ready_time - current_time);
			}

			if(temp.exec_time > TIMESLICE_TIME_UNITS){
				actual_time_spent = TIMESLICE_TIME_UNITS;
				is_more_than_time_quantum = 1;
			}
			else{
				actual_time_spent = temp.exec_time;
			}
			// enqueue the following processes,
			// which the ready_time is smaller than the time when "temp" finishes.
			while(cur_proc!=NULL && (cur_proc->ready_time < current_time + actual_time_spent)){
				
				struct proc* enqueue_proc;
				enqueue_proc = malloc(sizeof(struct proc));
				strncpy(enqueue_proc->name, cur_proc->name,sizeof(enqueue_proc->name));
				enqueue_proc->status = pid_not_exist;
				enqueue_proc->ready_time = cur_proc->ready_time;
				enqueue_proc->exec_time = cur_proc->exec_time;
				enqueue_proc->stat_start_time = temp.stat_start_time;
				enqueue(ready_queue,enqueue_proc);
				cur_proc = cur_proc->next;
				process_num++;
			}

			current_time += actual_time_spent;

			if(temp.status == pid_not_exist){
				temp.pid = fork();
				temp.status = pid_exist;

				if(temp.pid < 0){
					printf("Fork Failed");
					return -1;
				} else if(temp.pid == 0){
					// Child process
					printf("%s %ld\n",temp.name,(long)getpid());
					while(1);
					// busy waiting til the parent process kill it.
				}
				else{
					//Parent process
					if (clock_gettime(CLOCK_REALTIME,&temp.stat_start_time) < 0){
						printf("clock_gettime Failed\n");
                    	return -1;
             		} 

					Wait_time(actual_time_spent);
				
					if(is_more_than_time_quantum){
						struct proc* enqueue_proc;
						enqueue_proc = malloc(sizeof(struct proc));
						strncpy(enqueue_proc->name, temp.name,sizeof(enqueue_proc->name));
						enqueue_proc->pid = temp.pid;
						enqueue_proc->status = pid_exist;
						enqueue_proc->ready_time = temp.ready_time;
						enqueue_proc->exec_time = temp.exec_time - TIMESLICE_TIME_UNITS;
						enqueue_proc->stat_start_time = temp.stat_start_time;
						enqueue(ready_queue,enqueue_proc);
						process_num++;
					} else{
						//kill child process
						if (clock_gettime(CLOCK_REALTIME,&temp.stat_end_time) < 0){
							printf("clock_gettime Failed\n");
            	        	return -1;
            	        }
						//printf("[Project1] %d %ld.%ld %ld.%ld\n",temp.pid, temp.stat_start_time.tv_sec, temp.stat_start_time.tv_nsec
             			//, temp.stat_end_time.tv_sec, temp.stat_end_time.tv_nsec );
						if(kill(temp.pid,SIGKILL) < 0)
							return -1;
						if(waitpid(temp.pid,NULL,0) < 0)
							return -1;
            	        //print statistics of temp to kernel
				      //  if (proj1_stat_printk(temp.pid,
                        //      &temp.stat_start_time,
                          //    &temp.stat_end_time) < 0)
                		//	return -1;
             			 
					}
				}
			}
			//if pid exists
			else if(temp.status == pid_exist){
				if(temp.pid > 0){
					Wait_time(actual_time_spent);
					if(is_more_than_time_quantum){
						struct proc* enqueue_proc;
						enqueue_proc = malloc(sizeof(struct proc));
						strncpy(enqueue_proc->name, temp.name,sizeof(enqueue_proc->name));
						enqueue_proc->pid = temp.pid;
						enqueue_proc->status = pid_exist;
						enqueue_proc->ready_time = temp.ready_time;
						enqueue_proc->exec_time = temp.exec_time - TIMESLICE_TIME_UNITS;
						enqueue_proc->stat_start_time = temp.stat_start_time;
						enqueue(ready_queue,enqueue_proc);
						process_num++;
					} else{
						//kill child process
						if (clock_gettime(CLOCK_REALTIME,&temp.stat_end_time) < 0){
							printf("clock_gettime Failed\n");
                    		return -1;
             			}
						//printf("[Project1] %d %ld.%ld %ld.%ld\n",temp.pid, temp.stat_start_time.tv_sec, temp.stat_start_time.tv_nsec
             			//, temp.stat_end_time.tv_sec, temp.stat_end_time.tv_nsec );
						if(kill(temp.pid,SIGKILL) < 0)
							return -1;
						if(waitpid(temp.pid,NULL,0) < 0)
							return -1;
             			
             			//print statistics of temp to kernel
				        //if (proj1_stat_printk(temp.pid,
                         //     &temp.stat_start_time,
                          //    &temp.stat_end_time) < 0)
                		//	return -1;
					}
				}
			}
		}	
	}
	return 0;
}
