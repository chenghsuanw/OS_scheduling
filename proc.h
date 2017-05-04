#ifndef PROC_H
#define PROC_H
#define __NR_proj1_stat_printk 314

#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>

enum status{
	pid_not_exist,
	pid_exist

};

struct proc
{
	pid_t pid;
	char name[32];
	int status;
	unsigned ready_time,exec_time;
	struct timespec stat_start_time, stat_end_time;
	struct proc *next;
};

static inline int proj1_stat_printk(pid_t pid,
				    struct timespec *stat_start_time,
				    struct timespec *stat_end_time)
{
	return syscall(__NR_proj1_stat_printk,
		       pid,
		       stat_start_time,
		       stat_end_time);
}
#endif /*PROC_H*/
