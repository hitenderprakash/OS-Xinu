#include <xinu.h>
#include "future.h"

typedef struct futent future;


// Code to receive from udp packet

int pack_recv(uint32 slot){
	if(slot == SYSERR){
		kprintf("\nError: No UDP slot");
		return;
	}
	
	int i=0;
	
	while(i<1)
	{
		char incoming_message [IMSG_LEN] = "";
		int recv_status = udp_recv(slot, &incoming_message, IMSG_LEN, RCV_TIMEOUT);
		if(recv_status == TIMEOUT)
			kprintf("\nError: Timeout occured");
		else if(recv_status == SYSERR)
			kprintf("\nError in receiving message %d", recv_status);
		else
		{
			kprintf("\nMessage received %s", incoming_message);
			return atoi(incoming_message);
		}
		i++;	
	}
}
////////////

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

