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
    //initialize flag. We rely on the caller of function to provide exact flag
	//for example FUTURE_EXCLUSIVE in assignment 4. 
	fu->flag=future_flags;
    //initialize the state to EMPTY
	fu->state=FUTURE_EMPTY;
    //initialize the pid with 0. It will be overwritten by future_get
	fu->pid=0;
	//initialize default values of get and set queue
	fu->get_queue=0;
	fu->set_queue=0;
	
	//inititialize queue if required
	if (future_flags==FUTURE_SHARED || future_flags==FUTURE_QUEUE){
	  fu->get_queue=init_proc_queue();
	  //printf("\n******: get Q ID: %d",fu->get_queue);
	  fu->set_queue=init_proc_queue();
	  //printf("\n******: get Q ID: %d",fu->set_queue);
	}
	

  }
  //return the future reference. If memory was not allocated then SYSERR will be return
  restore(mask);
  return fu;
}

