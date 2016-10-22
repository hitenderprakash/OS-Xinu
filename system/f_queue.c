#include <xinu.h>
#include "future.h"

proc_queue* init_proc_queue(){
  proc_queue* head=getmem(sizeof(proc_queue));
  head->next=0;
  head->pid=-1;
  return head;
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
  printf("\nENQUEUED : %d",tempProc->pid);
}

pid32 f_dequeue(proc_queue *head){
  if(head->next){
	//printf("\nNot Empty");
	proc_queue *temp=head->next;
	pid32 p=temp->pid;
	head->next=temp->next;
	freemem(temp,sizeof(proc_queue));
	printf("\nreturning proc: %d",p);
	return p;
  }
  //printf("\nEmpty");
  return 0;
}
