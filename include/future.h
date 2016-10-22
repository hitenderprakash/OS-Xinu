#ifndef _FUTURE_H_
#define _FUTURE_H_
/* define states */

#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE	1	
#define FUTURE_SHARED		2
#define FUTURE_QUEUE		3 

//Added proc_queue to handle future_SHARED and future_QUEUE
typedef struct proc_queue{
  pid32 pid;
  struct proc_queue *next;
}proc_queue;


proc_queue* init_proc_queue();
int f_isempty(proc_queue *head);
f_enqueue (pid32 pid, proc_queue *head);
pid32 f_dequeue(proc_queue *head);


//Define Future structure 
typedef struct futent{
   int *value;		
   int flag;		
   int state;         	
   pid32 pid;
   proc_queue *set_queue;
   proc_queue *get_queue;
} future;

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

uint future_cons(future *fut);
uint future_prod(future *fut);


#endif /* _FUTURE_H_ */
