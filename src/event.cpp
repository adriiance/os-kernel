#include "KEvent.h"
#include "event.h"
Event::Event(IVTNo ivtNo):myImpl(0){
myImpl=new KernelEv(ivtNo);
}

Event::~Event(){
	delete myImpl;
}

void Event::signal(){
	myImpl->signal();
}

void Event::wait(){
	myImpl->wait();
}
