#ifndef PCB_H_
#define PCB_H_


#include "thread.h"

#include "SCHEDULE.h"


class Semaphore;
struct signalElement;

struct signalHead;
struct recordedSignals;
class PCB;


struct PCBlist{
	PCB*info;
	PCBlist* next;

	PCBlist(PCB*in):info(in),next(0){}
	~PCBlist(){info=0;
	next=0;}
};


class PCB{
public:
	unsigned mySP;
	unsigned stack_segment;
	static ID PCBid;

	static PCBlist* first;
	static PCBlist* last;
	PCB* next;
	ID myID;
	Time myTime;
	unsigned* myStack;
	int isFinished;
	int isRunning;
	void finishThread();
	Thread* myThread;
	PCB(StackSize n, Time timeslice, Thread* myThread1);
	~PCB();
  void help_run();
  static void call_help_run();
    StackSize getmySize();
	friend class Thread;
	int infinite;
	static int creatingMainThread;
	int IdleFlag;

	int semaphoreTime;
	static int numberOfThreadsCreated;
	static int numberOfThreadsFinished;
	int semaphoreRet;
	int isWaitingOnSemTime;
	Thread* parent;
	void setParent(Thread* myParent);
	void setMyThread(Thread* my){myThread=my;}
	static void callThreadConstructor();



	signalHead** mySignals;
	recordedSignals* myRecordedSignals;

	 int wasSignaled;
	 int wasKilled;
	 Semaphore* mySem;

private:
	StackSize mySize;


};




#endif /* PCB_H_ */
