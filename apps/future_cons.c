#include <xinu.h>
#include "future.h"

typedef struct futent future;

uint future_cons(future *fut) {
  int i, status;
  status = future_get(fut, &i);
  if (status < 1){
    printf("\nfuture_get failed\n");
    return -1;
  }
  printf("\nConsumer has consumed: %d ", i);
  return OK;
}

