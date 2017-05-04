#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/pid.h>

asmlinkage long sys_proj1_stat_printk (pid_t pid,
					struct timespec *stat_start_time
					struct timespec *stat_end_time)
{
	printk(KERN_INFO "[Project1] %ld %ld.%ld %ld.%ld\n",
		(long) pid,
		stat_start_time->tv_sec,
		stat_start_time->tv_nsec,
		stat_end_time->tv_sec,
		stat_end_time->tv_nsec)

	return 0;
}