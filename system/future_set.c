#include <xinu.h>
#include "future.h"

typedef struct futent future;
//This is called by producer
syscall future_set(future *f, int *value){
  intmask mask;
  mask=disable();
  if (f->state==FUTURE_VALID){
    //If the state is already VALID, should return SYSERR
	printf("\nProducer Error: The value produced earlier has not been consumed yet");
	restore(mask);
	return SYSERR;
  }

  if (f->state==FUTURE_EMPTY ){
    printf("\nProducer produced the value: %d ",*value);
	*(f->value)=*value;
	f->state=FUTURE_VALID;
    //since no process is waiting therefor no need to wake any process here. simply write the value and change the state
    restore(mask);
    return OK;
  }

  if (f->state==FUTURE_WAITING){
	printf("\nProducer produced the value: %d ",*value);
	*(f->value)=*value;
	f->state=FUTURE_VALID;   
	if (f->flag==FUTURE_EXCLUSIVE){
	  resume(f->pid);
	}
	else if (f->flag==FUTURE_SHARED){
	  //printf("\nUnfolding queue");
	  //printf("\n IS EMPTY: %d",f_isempty(f->get_queue));
	  //printf("\nresumed: %d",((f->get_queue)->next)->pid);
	  //printf("\dequeued: %d",f_dequeue(f->get_queue));
	  while(!(f_isempty(f->get_queue))){
		pid32 p=((f->get_queue)->next)->pid;
		printf("\nresumed: %d",p);
		f_dequeue(f->get_queue);
		resume(p);
		//resume(f_dequeue(f->get_queue));
	  }
	}
	restore(mask);
	return OK;
  }

}



