#include <scheduler.h>
#include <linkedlist.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[MAXTHREAD];
QUEUE waitinginevent[MAXTHREAD];
int count = 1;

LinkedList *queues;
int flag = 0;
int size = 0;
int pos = 0;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;
	

	if (!flag) {
		size = _initList();
		printf("list created with %d queues\n");
		flag = 1;
	}

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}
	
	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);
		printf("%d is blocking\n",callingthread);
		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}

	if(event==TIMER)
	{	
		printf("%d\n",_queue_size(&ready[pos]));
			

		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
		changethread = 1;

		
	}
	
	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}


int  _initList() {

	queues = list_create();

	for(int i=0; i<MAXTHREAD; i++) {
		list_push_back(queues,&ready[i]);
	}

	return list_size(queues);
}