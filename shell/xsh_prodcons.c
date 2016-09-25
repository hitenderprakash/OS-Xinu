#include <xinu.h>
#include <prodcons.h>

//Definition for global variable 'n'
int n=0;

//define two more golable variables for semaphores
sid32 produced,consumed;

/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int nargs, char *args[])

{
  //initialize count to default value.
  //It will be overwritten if supplied in argument in console.
  int count = 2000; 
  //initialize semaphores
  produced = semcreate(0); 
  consumed = semcreate(1);

  //code to support --help switch
  if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) 
  {

		printf("Usage: %s <int> \n\n", args[0]);
		printf("Description:\n");
		printf("\tProgram to show concurrent processes in Xinu without synchronization\n");
		printf("\tRuns two processes in parallel, one produce the value and othere consumes\n");

		return 0;

	}

    //handle if more than required number of srguments are supplied
	if (nargs > 2) 
	{

		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",args[0]);
		return 1;
	}

	//if two arguments are supplied and second argument is not '--help'
	if (nargs ==2) 
	{
        //overwrite count with user supplied value
		count=atoi(args[1]);
		// create processes and put them in ready queue

		resume( create(producer, 1024, 20, "producer", 1, count));
		resume( create(consumer, 1024, 20, "consumer", 1, count));
		return 0;
	}

	//if no argument is supplied. Code will work with default count value
	if (nargs<2)
	{
		resume( create(producer, 1024, 20, "producer", 1, count));
		resume( create(consumer, 1024, 20, "consumer", 1, count));
		return (0);
	}

}