// PCB.h 

#ifndef PCB_H
#define PCB_H

#include "copyright.h"
#include "bool.h"
#include "thread.h"
#include "synch.h"
#include "system.h"

class PCB
{
private:
	Semaphore* joinsem; // semaphore cho qua trinh join
	Semaphore* exitsem; // semaphore cho qua trinh exit
	Semaphore* mutex;
	int exitcode;
	Thread *thread;
	int numwait; // So tien trinh da join
	char fileName[32];
	int pid;
public:
	int parentID; // ID cua tien trinh cha
	PCB();
	PCB(int id);
	~PCB();

	int Exec(char* filename, int id); // Nap chuong trinh co ten luu trong bien filename va processID se la pid
	int GetID();
	int GetNumWait();
	
	void JoinWait();
	void ExitWait();
	void JoinRelease();
	void ExitRelease();

	void IncNumWait();
	void DecNumWait();

	void SetExitCode(int ec);
	int GetExitCode();

	void SetFileName(char* fn);
	char* GetFileName();
};

#endif // PCB_H
