#include <xinu.h>
#include "future.h"

proc_queue* init_proc_queue(){
  proc_queue* head=getmem(sizeof(proc_queue));
  head->next=0;
  head->pid=-1;
  return head;
  //queue will always have a dummy inital node "head" once it as been initialized. We dont store the actual process on head
  //if we store the process at head, and when it will be dequeued , we will loose the queue.
  //any process who want to join the queue after that will not be successful and cause unwanted behavior.
}

int f_isempty(proc_queue *head){
  return (!(head->next));
}
f_enqueue (pid32 pid, proc_queue *head){
  proc_queue *tempProc=getmem(sizeof(proc_queue));
  tempProc->pid=pid;
  tempProc->next=0;
  proc_queue *cur;
  cur=head;
  while(cur->next!=0){
    cur=cur->next;
  }
  cur->next=tempProc; 
}

pid32 f_dequeue(proc_queue *head){
  if(head->next){
	proc_queue *temp=head->next;
	pid32 p=temp->pid;
	head->next=temp->next;
	freemem(temp,sizeof(proc_queue));
	return p;
  }
  return 0;
}
