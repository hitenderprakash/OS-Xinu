#include <xinu.h>
#include "future.h"

typedef struct futent future;
//To free the dynamically allocated memory to Future.
syscall future_free(future* f){
  return freemem(f,sizeof(future));
  //freemem() also return syscall, therefor returning as it is.
}

