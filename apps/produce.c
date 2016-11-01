#include <xinu.h>
<<<<<<< HEAD
#include <prodcons.h>
#include <stdio.h>

producer(int count) 
{	
	for(int i=0;i<=count;i++)
	{
		wait(consumed);
		n=i;
		printf("\nProduced: %d",n);
		signal(produced);
	}
=======
//#include <prodcons.h>

void producer(int count) {
>>>>>>> upstream/master
}
