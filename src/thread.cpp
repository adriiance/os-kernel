#include "KSem.h"



#include <iostream.h>

extern int aa;
extern long prek;
Semafori* KernelSem::SemHead=0;
Semafori* KernelSem::SemTail=0;
extern int sum;
extern int krugSem;

extern int idleExists;
extern IdleThread* currentIdle;

int removeFromList(KernelSem* mySem) ;

extern PCB*running;








void tick();
extern SignalId blockedSignals[16];

struct signalElement{
 	signalElement* next;
 	SignalHandler myHandler;
 	signalElement(SignalHandler my):next(0),myHandler(my){}
    ~signalElement(){next=0; myHandler=0;}
 };



 struct signalHead{


 int blocked;
 int myNumber;
 int signalsReceived;
 signalElement* first;
 signalElement*last;
  void registerNewHandler(SignalHandler sh);
  void unregisterMyHandlers();
  void signal(){signalsReceived=1; }


  void doMySignals();
  void swapMySignals (SignalHandler hand1, SignalHandler hand2);

  signalHead():blocked(0),signalsReceived(0),first(0),last(0),myNumber(0){}
  ~signalHead(){

	  first=0;
	  last=0;

  }
 };





struct recordedSignals{
	unsigned info;
	recordedSignals* next;


	recordedSignals(unsigned i):info(i),next(0){

	};
    ~recordedSignals(){next=0; }

    static void addSignal(unsigned inf, PCB* my);



};

void resetSignals(PCB* my){
recordedSignals*tek=my->myRecordedSignals;
while(tek!=0){
	my->myRecordedSignals=my->myRecordedSignals->next;

	delete tek;
	tek=my->myRecordedSignals;
}

}


void recordedSignals::addSignal(unsigned inf,PCB*my){
if(my->myRecordedSignals==0){
	my->myRecordedSignals=new recordedSignals(inf);
	return;
}
else{
	recordedSignals*tek=my->myRecordedSignals;
	while(tek->next!=0) tek=tek->next;
	tek->next=new recordedSignals(inf);

	return;

}

}

void killRunningThread(){

	 running->isFinished=1;
	 running->isRunning=0;
     running->wasKilled=1;
     running->mySem->signal(0);

 }



void signalHead::registerNewHandler(SignalHandler sh){

 if(first==0){
 first=new signalElement(sh);
 last=first;
 return;
 }
 else{
 	last->next=new signalElement(sh);
 	last=last->next;
 	return;
 }

 }

 void signalHead::unregisterMyHandlers(){

 	signalElement*tek=first;
 while(tek!=0){
 first=first->next;
delete tek;
 tek=first;
 }

 }


 void signalHead::doMySignals(){

	 if(blocked==0 && blockedSignals[myNumber]==0){
 	signalElement*tek=first;
if(this->myNumber!=0){
        while(tek!=0){
     	   tek->myHandler();
     	   tek=tek->next;
        }
	 }
	 else{
		 tek->myHandler();
	 }
tek=0;
 }
 }
 void signalHead::swapMySignals (SignalHandler hand1, SignalHandler hand2){
	 signalElement* firstHandler=first;
     signalElement* secondHandler=first;

     while(firstHandler!=0 && firstHandler->myHandler!=hand1) firstHandler=firstHandler->next;
     while(secondHandler!=0 && secondHandler->myHandler!=hand2) secondHandler=secondHandler->next;

     if(firstHandler!=0 && secondHandler!=0){
    	 firstHandler->myHandler=hand2;
    	 secondHandler->myHandler=hand1;
     }

 };


 void Thread::blockSignalGlobally(SignalId signal){
	 blockedSignals[signal]=1;
 };

 void Thread::unblockSignalGlobally(SignalId signal){
	 blockedSignals[signal]=0;
 };

 void Thread::signal(SignalId signal){

	 if(signal!=2 && signal!=1 && myPCB->isFinished==0)
	 { myPCB->mySignals[signal]->signal();
     myPCB->wasSignaled++;
	 recordedSignals::addSignal(signal,myPCB);
	 }


	 if(signal==1 && running->parent==this && myPCB->isFinished==1){
		 myPCB->wasSignaled++;

		 myPCB->mySignals[signal]->signal();
		 recordedSignals::addSignal(signal,myPCB);
		}


	 if(signal==2 && running->myThread==this && running->isFinished==1){
		 myPCB->wasSignaled++;
		 myPCB->mySignals[signal]->signal();
		 recordedSignals::addSignal(signal,myPCB);
		}

 };

 void Thread::registerHandler(SignalId signal, SignalHandler handler){
	if(myPCB->isFinished==0) myPCB->mySignals[signal]->registerNewHandler(handler);
 };

 void Thread::unregisterAllHandlers(SignalId signal){
	 myPCB->mySignals[signal]->unregisterMyHandlers();
 };

 void Thread::swap (SignalId signal, SignalHandler hand1, SignalHandler hand2){
	 if(myPCB->isFinished==0) myPCB->mySignals[signal]->swapMySignals(hand1,hand2);
 };

 void Thread::blockSignal(SignalId signal){
	 if(myPCB->isFinished==0) myPCB->mySignals[signal]->blocked=1;
 };

 void Thread::unblockSignal(SignalId signal){
	 if(myPCB->isFinished==0) myPCB->mySignals[signal]->blocked=0;
  };





Thread::Thread (StackSize stackSize ,Time timeSlice) :myPCB(0){
	if(stackSize > (defaultStackSize/2)){stackSize=defaultStackSize/2;}


if(PCB::creatingMainThread==0)	myPCB=new PCB(stackSize, timeSlice,this);
else{
	myPCB=running;
}

	myPCB->mySignals=new signalHead*[16];
	for(int u=0;u<16;u++){
		myPCB->mySignals[u]=new signalHead();
		myPCB->mySignals[u]->myNumber=u;
	}


  this->registerHandler(0,killRunningThread);
   myPCB->setMyThread(this);
   myPCB->setParent(Thread::getThreadById(Thread::getRunningId()));
    myPCB->mySem=new Semaphore(0);
}


ID Thread::getId(){
	return myPCB->myID;
}

ID Thread::getRunningId(){
 if(running)
   return running->myID;
 else return -1;

}
Thread* Thread::getThreadById(ID id){
	PCBlist*tek= running->first;
    while(tek!=0){
    	if(tek->info->myID==id){
    		return tek->info->myThread;
    	}
    	tek=tek->next;
    }
   tek=0;
    return 0;
}

void Thread::start(){


Scheduler::put(myPCB);
myPCB->isFinished=0;
myPCB->isRunning=1;


}



void Thread::waitToComplete(){

if(myPCB->isFinished==0)	myPCB->mySem->wait(0);

	PCB::numberOfThreadsFinished++;
}

 Thread::~Thread(){
	waitToComplete();

				  for(int i=0;i<16;i++){
					  myPCB->mySignals[i]->unregisterMyHandlers();
				  }

				  for(int j=0;j<16;j++){
					 delete  myPCB->mySignals[j];
				  }

				  delete []  myPCB->mySignals;


	 	delete myPCB;

 }




int explicit_dispatch=0;
volatile unsigned helpStackSegment;
volatile unsigned helpStackOffset;
volatile Time clock=0;
void SemaphoreTimePast();
void createRunning();

volatile int foundRunning=0;
volatile int threadWasKilled=0;




void getNewRunning(){
 foundRunning=0;

 while(foundRunning==0){

running=Scheduler::get();
if(running==0){
	foundRunning=1;
	running=currentIdle->getMyPCB();
	    		 running->isRunning=0;
	    		    	idleExists=1;


}
else{
	idleExists=0;
 if(running->wasSignaled>0){
	           recordedSignals*tek=running->myRecordedSignals;
	 	    while(tek!=0 && running->isFinished==0){
	 	    	running->mySignals[tek->info]->doMySignals();

	 	    	 if( running->isFinished==0) tek=tek->next;
	 	    	 else tek=0;
	 	    }
	 	   resetSignals(running);
	 	    tek=0;


if(running->isFinished==0) foundRunning=1;
if(running->isFinished==1){delete[] running->myStack; running->myStack=0;}

running->wasSignaled=0;
               }
       else{
     	foundRunning=1;
           }
    } //ne idle



 } //trazenje



	}


volatile PCB* previousRunning=0;
volatile int ubacen=0;
volatile PCB* pomoc=0;
PCB* schedulerHelp();
PCB* findPCB();

 void interrupt timer(){


   if((explicit_dispatch==0)&&(running!=0)){
	   clock=clock-1;
    tick();
   SemaphoreTimePast();

   }
   if(((clock<=0)&&(running==0))||((clock<=0)&&(running->infinite>0))||(explicit_dispatch==1)){

   asm{

	   mov dx, sp
	   mov helpStackOffset, dx
	   mov dx, ss
	   mov helpStackSegment, dx
   }



    running->mySP= helpStackOffset;
    running->stack_segment=helpStackSegment;



    if((running->isFinished==0)&&(currentIdle->getMyPCB()!=running)&&(running->isRunning==1)){

    if(running!=0)	Scheduler::put(running);

    }

    getNewRunning();


   if(currentIdle->getMyPCB()!=running) running->isRunning=1;

    helpStackOffset=running->mySP;
    helpStackSegment=running->stack_segment;
    clock=running->myTime;



    asm{
    	mov dx, helpStackOffset
		mov sp, dx
		mov dx, helpStackSegment
		mov ss, dx


    }

   }

   if(explicit_dispatch==0){

	   asm{
		   int 60h
	   }
   }



   explicit_dispatch=0;



 };


 unsigned previousTimerOffset=0;
 unsigned previousTimerSegment=0;

 void set_new_timer(){
asm{
	cli
}


asm{
     cli
     push es
	 push dx

	 mov dx,0
	 mov es, dx



     mov dx, word ptr es:0022h
	 mov word ptr previousTimerSegment, dx
     mov dx, word ptr es:0020h
	 mov word ptr previousTimerOffset, dx

	mov word ptr es:0022h, seg timer
	mov word ptr es:0020h, offset timer


    mov dx, previousTimerSegment
	mov word ptr es:0182h, dx
	mov dx, previousTimerOffset
	mov word ptr es:0180h, dx

	pop dx
	pop es


}
createRunning();


asm{sti}
 }

void restore_previous_timer(){

asm{
	cli
	push dx
	push es

	mov dx,0
	mov es,dx

	mov dx, word ptr es:0182h
	mov word ptr previousTimerSegment, dx
	mov dx, word ptr es:0180h
    mov word ptr previousTimerOffset, dx



	mov dx, word ptr previousTimerSegment
	mov word ptr es:0022h, dx
	mov dx, word ptr previousTimerOffset
	mov word ptr es:0020h,dx

	pop es
	pop dx

};

if(currentIdle!=0) { delete currentIdle;}
if(KernelSem::SemHead==0) cout<<endl<<endl<<"NULL"<<endl<<endl;

delete Thread::getThreadById(Thread::getRunningId());
asm{sti}

};


void dispatch(){

	asm{
		cli
	}
	explicit_dispatch=1;

	timer();

	asm{
		sti
		}

};


PCB* ubaci;


void SemaphoreTimePast(){

	Semafori*tek1 = KernelSem::SemHead;

	  while(tek1!=0){
	    	tek1->info->decTime();
	     int br= removeFromList(tek1->info);
	    tek1=tek1->next;
	   };
}

void createRunning(){
	   PCB::creatingMainThread=1;
		   running=new PCB(defaultStackSize, defaultTimeSlice, 0);
		   PCB::callThreadConstructor();
	     PCB::creatingMainThread=0;
	     running->isRunning=1;
	   clock=defaultTimeSlice;
	   running->isFinished=0;
}







