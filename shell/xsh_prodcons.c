#include <xinu.h>
#include <prodcons.h>
#include "future.h"

uint future_cons(future *fut);
uint future_prod(future *fut);

typedef struct futent future;

//Definition for global variable 'n'
int n=0;
//Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce

//define two more golable variables for semaphores
sid32 produced,consumed;


shellcmd xsh_prodcons(int nargs, char *args[]){
  //initialize count to default value.
  //It will be overwritten if supplied in argument in console.
  int count = 2000; 
  
  //initialize semaphores
  produced = semcreate(0); 
  consumed = semcreate(1);
  
  //code to support --help switch
  if (nargs == 2 && strncmp(args[1], "--help", 7) == 0){
    printf("Usage: %s <int> \n\n", args[0]);
    printf("Description:\n");
    printf("\tProgram to show concurrent processes in Xinu without synchronization\n");
    printf("\tRuns two processes in parallel, one produce the value and othere consumes\n");
    return 0;
  }

  //handle if more than required number of srguments are supplied
  if (nargs > 2) {
    fprintf(stderr, "%s: too many arguments\n", args[0]);
	fprintf(stderr, "Try '%s --help' for more information\n",args[0]);
	return 1;
  }

  //handle future
  if (nargs == 2 && strncmp(args[1], "-f", 3) == 0){
	
    future *f1, *f2, *f3;
	
    f1 = future_alloc(FUTURE_EXCLUSIVE);
    f2 = future_alloc(FUTURE_EXCLUSIVE);
    f3 = future_alloc(FUTURE_EXCLUSIVE);
	resume( create(future_cons, 1024, 20, "fcons1", 1, f1) );
	resume( create(future_prod, 1024, 20, "fprod1", 1, f1) );
	resume( create(future_cons, 1024, 20, "fcons2", 1, f2) );
	resume( create(future_prod, 1024, 20, "fprod2", 1, f2) );
	resume( create(future_cons, 1024, 20, "fcons3", 1, f3) );
	resume( create(future_prod, 1024, 20, "fprod3", 1, f3) );
	return 0;
  }

  //if two arguments are supplied and second argument is not '--help'
  if (nargs ==2){
    //overwrite count with user supplied value
	count=atoi(args[1]);
	// create processes and put them in ready queue
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
	return 0;
  }
  //if no argument is supplied. Code will work with default count value
  if (nargs<2){
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
	return (0);
  }

}
