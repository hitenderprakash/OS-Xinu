#include <xinu.h>
#include "future.h"

typedef struct futent future;
//This is called by producer
syscall future_set(future *f, int *value){
  if (f->state==FUTURE_VALID){
    //If the state is already VALID, should return SYSERR
	printf("\nProducer Error: The value produced earlier has not been consumed yet");
	return SYSERR;
  }

  if (f->state==FUTURE_EMPTY ){

	*(f->value)=*value;
	f->state=FUTURE_VALID;
    //since no process is waiting therefor no need to wake any process here. simply write the value and change the state
    return OK;
  }

  if (f->state==FUTURE_WAITING){
    
	printf("\nProducer produced the value: %d ",*value);
	*(f->value)=*value;
	f->state=FUTURE_VALID;
	resume(f->pid);
	return OK;
  }

}



