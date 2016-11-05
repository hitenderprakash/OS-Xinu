#include <xinu.h>
#include "future.h"

typedef struct futent future;
//Code which is executed by Consumer
syscall future_get(future *f, int *value){
  
  intmask mask;
  pid32 pid=getpid();//getting PID
  if (f->state==FUTURE_EMPTY){//In case consumer is called before producer
	mask=disable();
	if(f->flag==FUTURE_EXCLUSIVE){
	  f->pid=pid;
	}
	else if(f->flag==FUTURE_SHARED || f->flag==FUTURE_QUEUE){
	  f_enqueue(pid,f->get_queue);
	}
	f->state=FUTURE_WAITING;
	restore(mask);//restore mask, do not hold it while waiting
	suspend(pid);
	//DO NOT RETURN. Should Continue with consuming the value after resume 
  }

  if (f->state==FUTURE_WAITING){
     mask=disable();
	//code should not come here inn case of FUTURE_EXCLUSIVE;
	if(f->flag==FUTURE_EXCLUSIVE){
	  restore(mask);
	  return SYSERR;		
	}
	else if(f->flag==FUTURE_SHARED){
	  f_enqueue(pid,f->get_queue);
	  restore(mask);
	  suspend(pid);
	  //DO NOT RETURN	
	}
	else if(f->flag==FUTURE_QUEUE){
	  f_enqueue(pid,f->get_queue);
	  if(!(f_isempty(f->set_queue))){
		resume(f_dequeue(f->set_queue));
	  }
	  restore(mask);
	  suspend(pid);
	  //DO NOT RETURN	
	}
	
  }
  
  //after resuming consumer should come here 
  if (f->state==FUTURE_VALID){	
	mask=disable();
	*value=*(f->value);
	
	if(f->flag==FUTURE_EXCLUSIVE){	
	  f->state=FUTURE_EMPTY;
	  restore(mask);
	  future_free(f);	
	  return OK;	
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
	  restore(mask);
	  return OK;
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
	  restore(mask);
	  return OK;
	}
  }

}



