#include <xinu.h>
#include "future.h"

typedef struct futent future;

uint future_cons(future *fut) {
  int i, status;
  int flag=fut->flag;
  //this variable is just for debugging.
  //we can print below the future flag as well 
  //we took a separate variable flag as ft->flag will be out of scope here in case..
  //when the last consumer returned after freeing the future.
  status = future_get(fut, &i);
  if (status < 1){
    kprintf("\nfuture_get failed\n");
    return -1;
  }
  kprintf("\nConsumer [PID:%u, Flag:%u] has consumed: %u ",getpid(),flag, i);//fut->flag can print garbage therefor using saved "flag"
  return OK;
}

