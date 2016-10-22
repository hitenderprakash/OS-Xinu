#include <xinu.h>
#include "future.h"

typedef struct futent future;
//Code which is executed by Consumer
syscall future_get(future *f, int *value){
  
  intmask mask;
  mask=disable();
  pid32 pid=getpid();//getting PID
  if (f->state==FUTURE_EMPTY){//In case consumer is called before producer
	if(f->flag==FUTURE_EXCLUSIVE){
	  f->pid=pid;
	}
	else if(f->flag==FUTURE_SHARED || f->flag==FUTURE_QUEUE){
	  f_enqueue(pid,f->get_queue);
	}
	f->state=FUTURE_WAITING;
	//printf("\nSuspending");
	suspend(pid);
	//return OK; omitted the return here so that when consumer resumes it continue with consuming the value 
  }

  if (f->state==FUTURE_WAITING){

	//code should not come here inn case of FUTURE_EXCLUSIVE;
	if(f->flag==FUTURE_EXCLUSIVE){
	  restore(mask);
	  return SYSERR;		
	}
	else if(f->flag==FUTURE_SHARED){
	  printf("\n Joining other in sleep");
	  f_enqueue(pid,f->get_queue);
	  suspend(pid);
	  //restore(mask);
	  //return OK;	DO NOT RETURN	
	}
	else if(f->flag==FUTURE_QUEUE){
	  //printf("\n Joining other in sleep");
	  f_enqueue(pid,f->get_queue);
	  if(!(f_isempty(f->set_queue))){
		pid32 p=((f->set_queue)->next)->pid;
		//printf("\nresumed setter: %d",p);
		f_dequeue(f->set_queue);
		resume(p);
		//resume(f_dequeue(f->get_queue));
	  }
	  suspend(pid);
	  //restore(mask);
	  //return OK;	DO NOT RETURN	
	}
	
  }
  
  //after resuming consumer should come here 
  if (f->state==FUTURE_VALID){	
	*value=*(f->value);
	//printf("\nResumedwith val %d",*value);
	
	if(f->flag==FUTURE_EXCLUSIVE){	
	  f->state=FUTURE_EMPTY;
	  return future_free(f);		
	  // Make empty as the only consumer has consumed value
	}
	else if (f->flag==FUTURE_SHARED){
	  //check if all consumer has consumed value
	  //make the state back to empty if all consumer are done
	  //free the future memory
	  if(f_isempty(f->get_queue)){
	    f->state=FUTURE_EMPTY;
	    future_free(f);
	  }		
	}
	else if (f->flag==FUTURE_QUEUE){
	  if(!f_isempty(f->get_queue)){
	    f->state=FUTURE_WAITING;
	  }
	  else{
	    f->state=FUTURE_EMPTY;
	  }
	  if(!f_isempty(f->set_queue)){
	    resume(f_dequeue(f->set_queue));
	  }
	}
	restore(mask);
	return OK;
  }

}



