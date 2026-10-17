#include <iostream.h>
#include "KSem.h"


volatile int num=0;
volatile int aa=0;
volatile long prek=0;

int userMain(int argc, char** argv);
void restore_previous_timer();
void set_new_timer();



#define radimoUserMain;

void main(int argc,  char* argv[])
{
	set_new_timer();

#ifdef radimoUserMain
			userMain(argc,argv);
#endif

#ifndef radimoUserMain
  const int num=10;






#endif

	 restore_previous_timer();



}


