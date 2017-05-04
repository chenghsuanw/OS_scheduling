#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "proc.h"

struct queue
{
	struct proc *head;
	struct proc *end;
};

static int queue_isempty(struct queue *queue){
	if(queue->head==NULL && queue->end ==NULL){
		return 1;
	}
	return 0;
}

static void enqueue(struct queue *queue,struct proc *process){
	struct proc *in;
	in = malloc(sizeof(struct proc));
	in = process;

	in->next = NULL;

	if(queue->end == NULL){
		queue->head = in;
		queue->end = in;
	} else{
		queue->end->next = in;
		queue->end = in;
	}
}

static void enqueue_by_ready_time(struct queue *queue, struct proc *process, unsigned ready_time){
	struct proc *in;
	in = malloc(sizeof(struct proc));
	in = process;

	struct proc *cur;
	cur = queue->head;
	if(ready_time < cur->ready_time){
		in->next = queue->head;
		queue->head = in;
	} else{
		while(cur->next != NULL){
			struct proc *next_proc;
			next_proc = cur->next;
			if(ready_time < next_proc->ready_time){
				cur->next = in;
				in->next = next_proc;
				break;
			}
			cur = next_proc;
		}
	}
}
static struct proc dequeue(struct queue *queue){
	if(!queue_isempty(queue)){
		struct proc *out;
		out = queue->head;

		if(queue->head == queue->end){
			queue->head = NULL;
			queue->end = NULL;
		} else {
			queue->head = queue->head->next;
		}

		struct proc value = *out;
		free(out);
		return value;
	}
}

static void destroy_queue(struct queue *queue){
	while(!queue_isempty(queue)){
		dequeue(queue);
	}
}

#endif /*QUEUE_H*/