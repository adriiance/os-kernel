#ifndef KEVENT_H_
#define KEVENT_H_

class Event;
class PCB;



class KernelEv{

public:
	KernelEv(unsigned char ivtNo);
   ~KernelEv();
   void signal();
   void wait();

   PCB* myThread;

private:

int value;
PCB* blockedThread;
unsigned char myInterruptEntry;
}
;


#endif /* KEVENT_H_ */
