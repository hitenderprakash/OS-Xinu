#include <xinu.h>
#include "future.h"

typedef struct futent future;
//code to receive from udp packet
void pack_send(	uint32 slot, char* message){
	if(slot==SYSERR)
	{
		kprintf("\nError:No UDP register slot.");
		return;
	}
	
	int i = 0;
	
	while(i<1)
	{
		if(strlen(message) >  OMSG_LEN)
		{
			kprintf("\nError: Message length exceeded character limit.");
			return;
		}
		int send_status = udp_send(slot, message, strlen(message));
		kprintf("\nMessage sent to VM ");
		if(send_status == SYSERR)
			kprintf("\nError: Packet send failed");
		
		i++;
	}
	
}

uint future_prod(future *fut){
  int i, j;
  j = (int)fut;
  for (i=0; i<1000; i++){
    j += i;
  }
  //Producer is settng the value
  future_set(fut, &j);
  return OK;
}

