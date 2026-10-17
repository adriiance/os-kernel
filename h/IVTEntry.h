#ifndef IVTENTRY_H_
#define IVTENTRY_H_
#include "KEvent.h"

void ispis(int h);

typedef void interrupt (* pointerToInterrupt) (...);

class IVTEntry{
public:
	IVTEntry(unsigned char ivtNo,pointerToInterrupt newInterrupt);

	~IVTEntry();
	void setMyEvent(KernelEv* my){myEvent=my;};
	void doMyInterrupt();
	KernelEv* getMyEvent();

private:
	KernelEv* myEvent;
	unsigned char myEntry;
     unsigned oldSegment;
     unsigned oldOffset;
     pointerToInterrupt previousInterrupt;
     pointerToInterrupt currentInterrupt;
};

#define PREPAREENTRY(myEntryNumber, callPrevInterrupt)\
	void interrupt newInterrupt##myEntryNumber(...);\
	IVTEntry EntryNumber##myEntryNumber(myEntryNumber,newInterrupt##myEntryNumber);\
	void interrupt newInterrupt##myEntryNumber(...){\
     EntryNumber##myEntryNumber.getMyEvent()->signal();\
     if(callPrevInterrupt==1){\
    	 EntryNumber##myEntryNumber.doMyInterrupt();       }\
}

#endif /* IVTENTRY_H_ */
