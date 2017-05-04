//#define _POSIX_C_SOURCE >= 199309L
#define _GNU_SOURCE
#define __NR_sys_proj1_stat_printk 314
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>

#include "proc.h"
#include "queue.h"
#include "limits.h"
#include "timer.h"
//12


int sched_psjf(struct queue *input_queue, int proc_number){
		
        struct proc *proc;
		struct proc *proc_ready[proc_number];
		struct proc P[proc_number];
		//process waiting list(ready to run)
		int k = 0;
		while(k < proc_number){
			P[k] = dequeue(input_queue);
			k++;
		}
		

		cpu_set_t cpu_mask;
	    CPU_ZERO(&cpu_mask);
	    CPU_SET(0, &cpu_mask); //add CPU to set, number is 0
    
    	if(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask) != 0){
        	perror("sched_setaffinity error");
 	       exit(EXIT_FAILURE);
 	    }

	    const int priorityH = 80;
	    const int priorityL = 10;

	    struct sched_param param;
	    param.sched_priority = 50;
	    
	    pid_t pidP = getpid();
	    if(sched_setscheduler(pidP, SCHED_FIFO, &param) != 0) {
	        return -1;
	    }

	    int numP_now = 0;
		int numP_finish = 0;
		int time_to_process = 0;
		int nextP_readyT = INT_MAX;
	    int current_time = 0;
    	int process_created = 0;
    	struct proc *processing = NULL;
    	int numP_finish_prev = 0;
    	
    	struct timespec tss;
    	if (clock_gettime(CLOCK_REALTIME, &tss) < 0)
            return -1;
        printf("start %ld.%ld\n", tss.tv_sec, tss.tv_nsec);

        Wait_time(500);

        struct timespec tse;
    	if (clock_gettime(CLOCK_REALTIME, &tse) < 0)
            return -1;
        printf("end %ld.%ld\n", tse.tv_sec, tse.tv_nsec);





	while(numP_finish != proc_number)
    {
        while(P[process_created].ready_time <= current_time && process_created < proc_number)//forkcount == numP_now
        {
        	
            printf("%-6d : %s fork \n", current_time, P[process_created].name);

            
            pid_t pid = fork();
            
            if(pid < 0)   
                return -1;   
            else if(pid == 0) {
                while(1);
                //if(execlp("./process", "process",  (char *)NULL) < 0){
                  //  return -1;
                //}
            }  
            
            struct proc *tempT;
            tempT = (P + process_created);
            if (clock_gettime(CLOCK_REALTIME, &tempT->stat_start_time) < 0)
            return -1;

            cpu_set_t cpu_mask;
            CPU_ZERO(&cpu_mask);
            CPU_SET(1, &cpu_mask);

            if(sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_mask) != 0){
                return -1;
            }
            
            P[process_created].pid = pid; 
            //insertP to waiting list
            		numP_now++;
            		int n = numP_now - 1;
					proc_ready[numP_now - 1] = (P + process_created );
					while(n > 0 && proc_ready[n]->exec_time < proc_ready[n - 1]->exec_time){
						struct proc *tempp = proc_ready[n];
						proc_ready[n] = proc_ready[n - 1];
						proc_ready[n - 1] = tempp;
						n--;
					}

            process_created++;
            nextP_readyT = P[process_created].ready_time;
            
        }
        
            
        processing = proc_ready[0];

        if (process_created != proc_number){
         	int t = nextP_readyT - current_time;	
           		if (t > proc_ready[0]->exec_time){
           			time_to_process = proc_ready[0]->exec_time;
           		}else{
           			time_to_process = t;
           		}
         }else{
         	time_to_process = processing->exec_time;
        }
                	proc_ready[0]->exec_time -= time_to_process;
      	  			if(proc_ready[0]->exec_time == 0){
            			numP_now--;
            			numP_finish++;
            			proc_ready[0] = NULL;
            			int m = 1;
            			while(m <= numP_now)
            			{
            				struct proc *temp = proc_ready[m];
		                	proc_ready[m] = proc_ready[m - 1];
	                		proc_ready[m - 1] = temp;
	                		m++;
            			}
        			}
        
                
					printf("%-6d : %s running for %d timeunits \n", current_time, processing->name, time_to_process);
                    pid_t pid = processing->pid;
                    param.sched_priority = priorityH;
                    if(sched_setscheduler(pid, SCHED_RR, &param) != 0) {
                        return -1;
                    }
					
            current_time += time_to_process;
            Wait_time(time_to_process);
            
            
                if(numP_finish == numP_finish_prev + 1){
                    printf("%-6d : %s end \n", current_time, processing->name);
                    int status;
                    if (kill(processing->pid, SIGKILL) < 0){
                    	return -1;
                    }
                    if (clock_gettime(CLOCK_REALTIME, &processing->stat_end_time) < 0)
                        return -1;
                }
                numP_finish_prev = numP_finish;
            
    }
    int j = 0;
    while(j < proc_number){
    	//printf("%s %d\n",P[j].name, P[j].pid);
    	printf("[project1] %ld %ld.%ld %ld.%ld\n", (long)P[j].pid, P[j].stat_start_time.tv_sec, P[j].stat_start_time.tv_nsec, P[j].stat_end_time.tv_sec, P[j].stat_end_time.tv_nsec);
    	struct proc temp = P[j];
    	if (proj1_stat_printk(temp.pid, &temp.stat_start_time, &temp.stat_end_time))
    		return -1;	
    	
    	j++;
    }
    
}
		