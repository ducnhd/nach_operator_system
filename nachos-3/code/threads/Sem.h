// Sem.h 

#ifndef SEM_H
#define SEM_H

#include "bitmap.h"
#include "bool.h"
#include "synch.h"
#include "system.h"

class Sem
{
private:
	char name[32];
	Semaphore* sem;

public:
	Sem(char* na, int i);
	~Sem();
	void Wait();
	void Signal();
	char *GetName();
};

#endif // PCB_H
