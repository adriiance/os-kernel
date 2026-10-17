#include <dos.h>
#include "IVTEntry.h"

#include <iostream.h>




IVTEntry* IVTEntries[256]={0};

IVTEntry::IVTEntry(unsigned char ivtNo,pointerToInterrupt newInterrupt):myEvent(0),myEntry(ivtNo),oldSegment(0),oldOffset(0),previousInterrupt(0),currentInterrupt(newInterrupt){
IVTEntries[myEntry]=this;
asm{cli}
previousInterrupt=getvect(myEntry);
setvect(myEntry,currentInterrupt);
asm{sti}
};

IVTEntry::~IVTEntry(){
	asm{cli}
	setvect(myEntry,previousInterrupt);
	asm{sti}

	myEvent=0;

};

void IVTEntry::doMyInterrupt(){
previousInterrupt();

};
KernelEv* IVTEntry::getMyEvent() {

	return myEvent;};


