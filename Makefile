all:
	gcc main.c sched_fifo.c sched_rr.c sched_sjf.c sched_psjf.c -o proj1.out -lrt
clean:
	rm -rf proj1.out
run:
		./proj1.out
