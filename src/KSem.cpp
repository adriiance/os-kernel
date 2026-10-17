#include <iostream.h>
#include "KSem.h"

extern PCB*running;
static int semCnt = 0;
extern int sum;
void addToList(KernelSem* mySem);
void addToWaitList(KernelSem* mySem);
int removeFromWaitList(KernelSem* mySem);
int removeFromList(KernelSem* mySem);
int removeFromListBySignal(KernelSem* mySem);
int krugSem=0;

KernelSem::KernelSem(int init, Semaphore* my) :value(init), myTime(0), blockedThreadsHead(0), blockedThreadsTail(0), vrati(0), mySemaphore(my), isWaitingForTime(0), id(0),
		blockedThreadsHeadWait(0), blockedThreadsTailWait(0){
	semCnt++;
	id = semCnt;

	this->blockedThreadsHead = 0;
	this->blockedThreadsTail = 0;
	this->blockedThreadsHeadWait = 0;
		this->blockedThreadsTailWait = 0;

	if (KernelSem::SemHead == 0) {

		KernelSem::SemHead = new Semafori(this);
		KernelSem::SemTail = KernelSem::SemHead;
	}
	else {

		KernelSem::SemTail->next = new Semafori(this);
		KernelSem::SemTail = KernelSem::SemTail->next;
	}

};



lista* deleteBlockedThreads(lista* head){
lista*tek=head;
lista* tekpom;
while (tek != 0) {
	head=head->next;

		tek = head;
		delete tekpom;

	}
 return 0;
}



KernelSem::~KernelSem() {

	this->blockedThreadsHead= deleteBlockedThreads(this->blockedThreadsHead);
	this->blockedThreadsHeadWait=deleteBlockedThreads(this->blockedThreadsHeadWait);

	int number=0;

		if(KernelSem::SemHead->info==this)number=1;
		if(KernelSem::SemTail->info==this) number=2;
		if(KernelSem::SemTail->info!=this && KernelSem::SemHead->info!=this) number=3;
		if(KernelSem::SemHead->info==this && KernelSem::SemTail->info==this)number=0;

		if(number==0){
			Semafori* first=KernelSem::SemHead;
			KernelSem::SemHead=0;
	        delete first;
		}



		if(number==1) {
			Semafori*first=KernelSem::SemHead;
			KernelSem::SemHead=KernelSem::SemHead->next;

			delete first;
		}

			if(number==2){

			Semafori*find=KernelSem::SemHead;
			while(find->next!=KernelSem::SemTail) {find=find->next;}
			KernelSem::SemTail=find;
			find=find->next;
			KernelSem::SemTail->next=0;


			delete find;


		}

		if(number==3){

			Semafori*find=KernelSem::SemHead;
			Semafori*find2=KernelSem::SemHead;
						while(find->next->info!=this) {find=find->next;}

                             find2=find->next;
                             find->next=find2->next;

                             delete find2;

		}


mySemaphore=0;

};

int KernelSem::signalKS(int n) {


	int vrati = 0;
	asm{ cli }


		if (n == 0) {
		  vrati= removeFromWaitList(this);
		  if(vrati==0) removeFromListBySignal(this);
		  value++;
			asm{ sti }
			return 0;

		}

	if (n < 0){
		asm{sti}
		return n;
	}

	while ((vrati < n) && (blockedThreadsHeadWait != 0)) {
			vrati+=removeFromWaitList(this);
		}
	while ((vrati < n) && (blockedThreadsHead != 0)) {
				vrati+=removeFromListBySignal(this);
			}


     value+=n;
     value-=vrati;
	asm{ sti };
	return vrati;
};

int KernelSem::waitKS(Time maxTimeToWait) {

	asm{ cli }

		if (value <= 0) {

			if (maxTimeToWait > 0) {
				running->semaphoreTime = maxTimeToWait;
				addToList(this);
				value--;
				dispatch();

				asm{ sti };
				return running->semaphoreRet;
			}

			else {
				value--;
				running->semaphoreTime =0;
				addToWaitList(this);
				dispatch();
				asm{ sti };
				return running->semaphoreRet;
			}
		}
		else {
			value--;
			asm{ sti }
			return 1;

		}

};

int KernelSem::findWaitingTime() {
	int vr = 0;
	int min = -1;
	for (lista*tek = blockedThreadsHead; tek != 0; tek = tek->next) {
		if (tek->info->semaphoreTime > 0) {
			if ((min == -1) || (min > tek->info->semaphoreTime)) { min = tek->info->semaphoreTime; }

		}
	}
	if (min > 0) vr = min;
	return vr;

};

void addToList(KernelSem* mySem) {
	running->isFinished=0;
	running->isRunning=0;

    lista* elem=new lista(running,mySem);
	if (mySem->blockedThreadsHead == 0) {
		mySem->blockedThreadsHead = elem;
		mySem->blockedThreadsTail = elem;

	}
	else {
		mySem->blockedThreadsTail->next = elem;
		mySem->blockedThreadsTail = elem;
	}

 return;
}
void addToWaitList(KernelSem* mySem) {
	running->isFinished=0;
	running->isRunning=0;

    lista* elem=new lista(running,mySem);
	if (mySem->blockedThreadsHeadWait == 0) {

		mySem->blockedThreadsHeadWait = elem;
		mySem->blockedThreadsTailWait = elem;
        return;
	}
	else {

		mySem->blockedThreadsTailWait->next = elem;
		mySem->blockedThreadsTailWait = elem;
	}

 return;
}


int removeFromList(KernelSem* mySem) {
    int vrati=0;
	lista*pret = 0;
	lista*tek = mySem->blockedThreadsHead;

	while (tek != 0) {

		if (tek->info!=0 && tek->info->semaphoreTime <= 0 ) {

			mySem->incVal();
            vrati++;

			if(tek->info->isFinished==0)tek->info->isRunning = 1;
			if(tek->info->isFinished==0)tek->info->semaphoreRet = 0;
			if(tek->info->isFinished==0) Scheduler::put(tek->info);


			int number=-1;
				if (tek == mySem->blockedThreadsHead)number=1;
			    if(tek==mySem->blockedThreadsTail) number=2;
				if(tek!=mySem->blockedThreadsHead && tek!=mySem->blockedThreadsTail)number=3;
				if ( mySem->blockedThreadsHead == tek && tek==mySem->blockedThreadsTail)number=0;


			if ( number==0) {
				lista*first=mySem->blockedThreadsHead;
				mySem->blockedThreadsHead=0;
				mySem->blockedThreadsTail=0;
				delete first;

				return vrati;
			}

				if (number==1){
					mySem->blockedThreadsHead=mySem->blockedThreadsHead->next;
					lista*find=tek;
					  tek=mySem->blockedThreadsHead;
					delete find;
				}


				if(number==2){
					lista* find=mySem->blockedThreadsHead;
					while(find->next!=tek){ find=find->next;}

					mySem->blockedThreadsTail->info=0;
					mySem->blockedThreadsTail->next=0;

					mySem->blockedThreadsTail=find;
					find=find->next;
					mySem->blockedThreadsTail->next=0;

					delete find;
					return vrati;

				}
				if(number==3){
					lista* find=mySem->blockedThreadsHead;
					while(find->next!=tek){ find=find->next;}
					find->next=tek->next;
					find=tek;
					tek=tek->next;
					delete find;
				}



		}



		else {
			tek = tek->next;
		}




	}
return vrati;

}

int removeFromWaitList(KernelSem* mySem) {

if(mySem->blockedThreadsHeadWait!=0){

    lista*rem=mySem->blockedThreadsHeadWait;
    mySem->blockedThreadsHeadWait=mySem->blockedThreadsHeadWait->next;
    if(mySem->blockedThreadsHeadWait==0) mySem->blockedThreadsTailWait=0;

    rem->info->isRunning=1;
    rem->info->semaphoreRet=1;

    mySem->incVal();
   if(rem->info!=0 && rem->info->isFinished==0) Scheduler::put(rem->info);

   rem->info=0;
   rem->next=0;
   delete rem;
    return 1;
}
return 0;
}

int removeFromListBySignal(KernelSem* mySem) {
if(mySem->blockedThreadsHead!=0){
    lista*rem=mySem->blockedThreadsHead;
    mySem->blockedThreadsHead=mySem->blockedThreadsHead->next;
    if(mySem->blockedThreadsHead==0) mySem->blockedThreadsTail=0;

    rem->info->isRunning=1;
    rem->info->semaphoreRet=1;

    mySem->incVal();
   if(rem->info!=0 && rem->info->isFinished==0) Scheduler::put(rem->info);

   rem->info=0;
      rem->next=0;
      delete rem;
    return 1;
}
return 0;
}

void ispis(int h) {
	cout << "\n g= " << h << '\n';
}


void KernelSem::decTime() {

	for (lista*tek = blockedThreadsHead; tek != 0; tek = tek->next) {
			tek->info->semaphoreTime--;
	}

}



