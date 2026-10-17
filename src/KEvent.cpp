#include "KEvent.h"
extern PCB*running;
#include "IVTEntry.h"
#include "pcb.h"
#include <iostream.h>


extern IVTEntry* IVTEntries[256];

KernelEv::KernelEv(unsigned char ivtNo): myThread(running),blockedThread(0),myInterruptEntry(ivtNo),value(0){
IVTEntries[myInterruptEntry]->setMyEvent(this);
};


KernelEv::~KernelEv(){
	myThread=0;
	blockedThread=0;

}

void KernelEv::signal(){
if(blockedThread!=0){
	Scheduler::put(blockedThread);
	blockedThread->isRunning=1;
	blockedThread=0;
}
else{
	value=1;
}
dispatch();
}

void KernelEv::wait(){
	if(running!=myThread) return;
	if(value==1){value=0;
	return;
	}
	value=0;
    blockedThread=running;
    running->isRunning=0;
    dispatch();
}
