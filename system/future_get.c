#include <xinu.h>
#include "future.h"

typedef struct futent future;
//Code which is executed by Consumer
syscall future_get(future *f, int *value){
  
  intmask mask;
  mask=disable();
  pid32 pid=getpid();//getting PID
  if (f->state==FUTURE_EMPTY){//In case consumer is called before producer
	  
    f->pid=pid;
	f->state=FUTURE_WAITING;
	suspend(pid);
	//return OK; omitted the return here so that when consumer resumes it continue with consuming the value 
  }

  if (f->state==FUTURE_WAITING){

	//code should not come here;
	restore(mask);
	return SYSERR;
  }
  
  //after resuming consumer should come here 
  if (f->state==FUTURE_VALID){	

	*value=*(f->value);	
	f->state=FUTURE_EMPTY;		
    // Once the value is consudmed, we should free the future.
	restore(mask);
	return future_free(f);
  }

}



