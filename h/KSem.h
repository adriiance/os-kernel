#ifndef KSEM_H_
#define KSEM_H_

#include "semaphor.h"

#include "pcb.h"


extern Semaphore* sem;
void ispis (int h);

class IdleThread: public Thread {
public:
	IdleThread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(stackSize,timeSlice) {
		PCBlist*tek=0;
		        tek=PCB::first;
		        while((tek!=0) && (tek->info->myID!=this->getId())){tek=tek->next;}
		        tek->info->IdleFlag=1;
		        tek->info->myID=-10;
		        tek->info->isFinished=1;
		        tek->info->mySem->signal(0);
	};
	virtual ~IdleThread(){getMyPCB()->isFinished=1;getMyPCB()->mySem->signal(0); waitToComplete();};

	PCB* getMyPCB(){
		PCBlist*tek=PCB::first;
		 while((tek!=0) && (tek->info->myID!=this->getId())){tek=tek->next;}

		return tek->info;
	}

    void run(){

   	 int flag=0;
   while(flag==0){
    int g=0;
   for(g=0;g<32000;g++){

   }

   };
    }


};


struct Semafori{
public:
	KernelSem*info;

	Semafori *next;

	Semafori(KernelSem* el):info(el),next(0){


	};
	~Semafori(){info=0;next=0;};

};


	 struct lista{
	 		PCB*info;
	 		lista* next;
            KernelSem* mySem;

	 		lista(PCB*el,KernelSem* ks):info(el), mySem(ks), next(0){


	 		};
	 		~lista(){
	 			info=0;
	 		    next=0;
	 			mySem=0;
	 		}

	 	};


class KernelSem{

public:

	KernelSem(int init, Semaphore* my);
	~KernelSem();

	int waitKS(Time maxTimeToWait);
	 int signalKS(int n=0);

	 int valKS() const{return value;};



     Time timeToWait() const{return myTime;}
     void setTime(Time t){myTime=t;}
     void decTime();
     int findWaitingTime();

     lista* blockedThreadsHead; //blocked on time
     	lista* blockedThreadsTail;//blocked on time

     	 lista* blockedThreadsHeadWait; //blocked on wait
     	     	lista* blockedThreadsTailWait;//blocked on wait


     	void incVal(){value++;}
         int vrati;
         Semaphore* mySemaphore;
         int isWaitingForTime;

         static Semafori* SemHead;
         static Semafori* SemTail;

         int id;
private:


	int value;
	Time myTime;

};



#endif /* KSEM_H_ */
