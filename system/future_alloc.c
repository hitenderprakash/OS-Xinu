#include <xinu.h>
#include "future.h"

typedef struct futent future ;

//creatng Future
future* future_alloc(int future_flags){
  intmask mask;
  mask=disable();
  future* fu=getmem(sizeof(future));
  if (fu!=SYSERR){ //if memory is successfully allocated
    //initialize value with 0
	fu->value=0;
	
	fu->flag=future_flags;
    //initialize the state to EMPTY
	fu->state=FUTURE_EMPTY;
	
    //initialize the pid with 0. It will be overwritten by future_get
	fu->pid=0;
	
	//initialize queues with NULL
	fu->get_queue=0;
	fu->set_queue=0;
	
	//inititialize queue properly if required
	if (future_flags==FUTURE_SHARED){
	  fu->get_queue=init_proc_queue();
	  //only get_queue is required
	}
	if (future_flags==FUTURE_QUEUE){
	  fu->get_queue=init_proc_queue();
	  fu->set_queue=init_proc_queue();
	  //initalize both set_queue and get_queue
	}
	

  }
  //return the future reference. If memory was not allocated then SYSERR will be return
  restore(mask);
  return fu;
}

