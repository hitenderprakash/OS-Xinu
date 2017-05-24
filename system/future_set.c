#include <xinu.h>
#include "future.h"

typedef struct futent future;
//This is called by producer
syscall future_set(future *f, int *value){
  intmask mask;
  if (f->state==FUTURE_VALID){
	  mask=disable();
	//in Case of Exclusive and Shared
    //If the state is already VALID, should return SYSERR
    if((f->flag==FUTURE_EXCLUSIVE)||(f->flag==FUTURE_SHARED)){
	  kprintf("\nProducer Error: The value produced earlier has not been consumed yet");
	  restore(mask);
	  return SYSERR;		
	}
	else if(f->flag==FUTURE_QUEUE){
	  pid32 p=getpid();
	  f_enqueue(p,f->set_queue);
	  if(!f_isempty(f->get_queue)){
	    resume(f_dequeue(f->get_queue));
	    restore(mask);
		suspend(p);
	  }
	  // DO NOT RETURN	
	}
  }

  if (f->state==FUTURE_EMPTY ){
	mask=disable();
    kprintf("\nProducer[pid:%u, Flag:%u] produced the value: %u ",getpid(),f->flag,*value);
    if(futureSelectionFlag ==1){
		*(f->value)=*value;
	}
	if(futureSelectionFlag ==2){
		pack_send(slot,value);
	}
	f->state=FUTURE_VALID;
    //since no process is waiting therefor no need to wake any process here. simply write the value and change the state
    restore(mask);
    return OK;
  }

  if (f->state==FUTURE_WAITING){
	mask=disable();
	kprintf("\nProducer[pid:%u, Flag:%u] produced the value: %u ",getpid(),f->flag,*value);
	if(futureSelectionFlag ==1){
		*(f->value)=*value;
	}
	if(futureSelectionFlag ==2){
		pack_send(slot,value);
	}
	f->state=FUTURE_VALID;   
	if (f->flag==FUTURE_EXCLUSIVE){
	  resume(f->pid);
	}
	else if (f->flag==FUTURE_SHARED){
	  while(!(f_isempty(f->get_queue))){
		resume(f_dequeue(f->get_queue));
	  }
	}
	else if (f->flag==FUTURE_QUEUE){
	  if(!(f_isempty(f->get_queue))){
		resume(f_dequeue(f->get_queue));
	  }
	}
	restore(mask);
	return OK;	
  }

}



