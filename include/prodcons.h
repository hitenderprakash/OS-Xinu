/*Global variable for producer consumer*/
extern int n; 
//Added two more vaiales for synchronization 
extern sid32 produced, consumed;

/*function Prototype*/

void consumer(int count);
void producer(int count);

