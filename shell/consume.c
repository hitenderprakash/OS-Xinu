#include <xinu.h>
#include <prodcons.h>
#include <stdio.h>

consumer(int count) 
{
  for(int i=0;i<=count;i++)
  {
	  wait(produced);
	  printf("\nConsumed:  %d",n);
	  signal(consumed);
  }
}
