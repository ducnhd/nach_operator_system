// Sem.cc 

#include "Sem.h"

Sem::Sem(char* na, int i)
{
	strcpy(name, na);
	sem = new Semaphore(name, i);
}

Sem::~Sem()
{
	delete sem;
}

void Sem::Wait()
{
	sem->P();
}

void Sem::Signal()
{
	sem->V();
}

char* Sem::GetName()
{
	return name;
}
