#include <xinu.h>
<<<<<<< HEAD
#include <prodcons.h>
#include <stdio.h>
=======
//#include <prodcons.h>
>>>>>>> upstream/master

consumer(int count) 
{
  for(int i=0;i<=count;i++)
  {
	  wait(produced);
	  printf("\nConsumed:  %d",n);
	  signal(consumed);
  }
}
