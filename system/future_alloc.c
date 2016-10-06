#include <xinu.h>
#include "future.h"

typedef struct futent future ;

//creatng Future
future* future_alloc(int future_flags){
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
  }
  //return the future reference. If memory was not allocated then SYSERR will be return
  return fu;
}

