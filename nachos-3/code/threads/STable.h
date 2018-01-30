// STABLE.h 

#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "bool.h"
#include "synch.h"
#include "Sem.h"
#include "system.h"

#define MAX_SEMAPHORE 10

class Sem;
class STable
{
private:
	BitMap *bm;
	Sem** semTab;
	int size;

public:
	STable(int isize);
	~STable();

	int Create(char *name, int init);
	int Wait(char *name);
	int Signal(char *name);
	int FindFreeSolt();
};

#endif // PCB_H
