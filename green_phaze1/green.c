#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "green.h"
#include "queue.h"


#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4*1024

static ucontext_t main_cntx = {0};

static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, NULL, FALSE};

static green_t * running = &main_green;


queue_t * ready_queue;
green_t * threads_head;	//new

// this function is called once program loads into memory regurdless of what function is called!
static void init( ) __attribute__((constructor));
void init(){
	getcontext(&main_cntx);
	ready_queue = createQueue();
	threads_head = (green_t*)malloc(sizeof(green_t));	//new
	threads_head = &main_green;    //new
}
// this functions is mapped to every Context
void green_thread(){
	green_t * this=running;
	// call target function and save its result
	void * variable = this->fun(this->arg);    //new
	// save result of execution and zombie status
	this->retval = variable;    //new
	this->zombie = TRUE;    //new
	// place waiting (joining) thread in ready queue
	green_t * temp;    //new  
	for(temp = threads_head; temp != NULL; temp = temp->next)    //new
	{    //new
		if(temp->join == this)     //new   //new
			enQueue(ready_queue, temp);    //new
	}    //new
	// find the next thread to run and write its address to next variable
	green_t* next = deQueue(ready_queue);    //new

	running = next;    //new
	setcontext(next->context);    //new
}

// will create a new green thread
int green_create(green_t *new ,void *(*fun)(void *),void *arg) {
	ucontext_t * cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
	// intialize cntx
	getcontext(cntx);	//new
	void * stack = malloc(STACK_SIZE);
	// assign allocated stack to cntx
	cntx->uc_stack.ss_sp = stack;	//new
	cntx->uc_stack.ss_size = STACK_SIZE;    //new
	// assign green_thread function to cntx
	makecontext(cntx, (void (*) (void))green_thread, 0);    //new

	new->context = cntx ;
	new->fun = fun ;
	new->arg = arg ;
	new->next = NULL;
	new->join = NULL;
	new->retval = NULL;
	new->zombie = FALSE;
	// add this new thread to end of linked list
	green_t * t;
	for(t = threads_head ; t->next != NULL ; t = t->next);
	t->next = new;
	// add new thread to the ready queue
	enQueue(ready_queue, new);	//new
	return 0 ;
}

// will give other green thread opportunity of having CPU!
int green_yield(){
	green_t * susp = running ;
	// add susp to ready queue
	enQueue(ready_queue, susp);    //new
	// select the next thread for execution
	green_t *next = deQueue(ready_queue);	//new
	running = next ;	//new
	// save current state into susp->context and switch to next->context
	swapcontext(susp->context, running->context);	//new
	return 0 ;
}

// waits for specefied thread till it finishes and get result value
int green_join(green_t * thread ,void ** res) {
	green_t * susp = running ;
	
	// check if target thread has finished
	if(thread->zombie == FALSE)
	{
		// add as joining thread
		susp->join = thread;	//new
		// select the next thread for execution
		green_t *next = deQueue(ready_queue);	//new
		running = next ;
		// save current state into susp->context and switch to next->context
		swapcontext(susp->context, running->context);	//new
	}
	// collect result
	*res = thread->retval;
	// free context
	free(thread->context);
	return 0 ;
}
