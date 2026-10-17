#include <iostream.h>
#include "KSem.h"

Semaphore::Semaphore(int init):myImpl(0){
myImpl=new KernelSem(init,this);
};

Semaphore::~Semaphore(){
	delete myImpl;
}
;
int Semaphore::wait(Time maxTimeToWait){
int a=myImpl->waitKS(maxTimeToWait);
return a;
};

 int Semaphore::signal(int n){
    return	myImpl->signalKS(n);
    };

    int Semaphore::val() const{
  return myImpl->valKS();
    };
