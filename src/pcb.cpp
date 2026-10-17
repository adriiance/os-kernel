#include <dos.h>


#include "KSem.h"
#include <iostream.h>

void resetSignals(PCB* my);

 PCBlist* PCB::first=0;
 PCBlist* PCB::last=0;
 ID PCB::PCBid=0;
 int PCB::numberOfThreadsCreated=0;
 	int PCB::numberOfThreadsFinished=0;

volatile PCB*running=0;
int PCB::creatingMainThread=0;


volatile int idleExists=0;
volatile IdleThread* currentIdle=new IdleThread();;

extern int num;



StackSize PCB::getmySize(){
	return mySize;
}

void PCB::finishThread(){
	isFinished=1;
}


  void PCB::call_help_run(){
if(running->myThread){
	running->myThread->run();
	running->isFinished=1;
	running->isRunning=0;
	num++;
	if(running->parent!=0)running->parent->signal(1);
	if(running->myThread!=0)running->myThread->signal(2);
   running-> mySem->signal(0);
	dispatch();
}
  };

  void PCB::callThreadConstructor(){
	  new Thread();
	  return;
  };

  void PCB::setParent(Thread* myParent){
	  parent=myParent;
  };
  PCB::~PCB(){



	  int number=-1;
	  		  if(PCB::first->info==this)number=1;
	  		  if(PCB::last->info==this)number=2;
	  		  if(PCB::last->info!=this &&  PCB::first->info!=this)number=3;
	  		  if(PCB::first==PCB::last)number=0;


          if(number==0){
        	  PCBlist*del=PCB::first;
		  PCB::first=0;
		  PCB::last=0;
		  delete del;
	       }
		  if(number==1){PCBlist*del=PCB::first;
			  PCB::first=PCB::first->next;
		  delete first;}
          if(number==2){
        	  PCBlist*find=PCB::first;
        	  while(find->next->info!=this){find=find->next;}
        	  PCB::last=find;
        	  PCB::last->next=0;
        	  find=find->next;
        	  delete find;


          }
         if(number==3){
        	  PCBlist*find=PCB::first;
        	  PCBlist*del=find;
        	         	  while(find->next->info!=this){find=find->next;}
        	         	  del=find->next;
        	         	  find->next=del->next;
        	         	  delete del;


          }



	  next=0;
	  resetSignals(this);

	if(!wasKilled)  delete [] myStack;

		  myThread=0;
		  parent=0;
delete mySem;


  }

 PCB::PCB(StackSize n, Time timeslice, Thread* myThread1):myTime(timeslice),myThread(myThread1),mySP(0),stack_segment(0),next(0),myID(0),
  			myStack(0),isFinished(0),isRunning(0), mySize(n),infinite(myTime),semaphoreTime(0),semaphoreRet(0),isWaitingOnSemTime(0),parent(0),
			wasSignaled(0),IdleFlag(0),mySignals(0),myRecordedSignals(0),wasKilled(0),mySem(0)
			{

	 myStack= new unsigned[n];

	      if(myThread1!=0)  myID=++PCBid;
	      else myID=0;
	        PCB::numberOfThreadsCreated++;

	  #ifndef BCC_BLOCK_IGNORE
	        myStack[n-1]=512;
	       myStack[n-2]=FP_SEG(call_help_run);
	        myStack[n-3]=FP_OFF(call_help_run);

	       mySP=FP_OFF(myStack+n-12);
	       stack_segment=FP_SEG(myStack+n-12);
	  #endif

	       if(PCB::first==0){
	    	   PCB::first=new PCBlist(this);
	    	   PCB::last=PCB::first;}
	       else{ PCB::last->next=new PCBlist(this);  }
	       PCB:: last=PCB::last->next;

  	}

 SignalId blockedSignals[16]={0};

