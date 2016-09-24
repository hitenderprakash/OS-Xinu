#include <xinu.h>
#include <prodcons.h>
#include <stdio.h>

producer(int count) 
{
	
	for(int i=0;i<=count;i++)
	{
		n=i;
		printf("\nProduced: %d",n);
	}
}