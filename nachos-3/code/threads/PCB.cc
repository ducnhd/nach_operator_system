// PCB.cc

#include "PCB.h"


void ExecBridge(int id)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	machine->Run();
	ASSERT(FALSE);

	/*AddrSpace *space;
	char* FileName = pTab->GetFileName(id);

	space = new AddrSpace(FileName);
	if (space == NULL)
	{
		printf("\nInsufficient memory!");
		DEBUG('a', "Insufficient memory!");
		return;
	}

	currentThread->space = space;

	space->InitRegisters();
	space->RestoreState();

	machine->Run();
	ASSERT(FALSE);*/

}


PCB::PCB()
{
	joinsem = new Semaphore("joinsem", 0);
	exitsem = new Semaphore("exitsem", 0);
	mutex = new Semaphore("mutex", 1);
	exitcode = 0;
	thread = NULL;
	numwait = 0;
	parentID = currentThread->processID;
	pid = 0;
}

PCB::PCB(int id)
{
	joinsem = new Semaphore("joinsem", 0);
	exitsem = new Semaphore("exitsem", 0);
	mutex = new Semaphore("mutex", 1);
	exitcode = 0;
	thread = NULL;
	numwait = 0;
	parentID = currentThread->processID;
	pid = id;
}

PCB::~PCB()
{
if (joinsem != NULL)
	{
	delete joinsem;
}
if (exitsem != NULL)
	{
	delete exitsem;
}
if (mutex != NULL)
	{
	delete mutex;
}
	if (thread != NULL)
	{
		delete thread->space;
		thread->Finish();
		delete thread;
	}//
}

int 
PCB::Exec(char* filename, int id)
{
	mutex->P();//wait
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return -1;
    	}
	strcpy(fileName, filename);
	
	AddrSpace* space = new AddrSpace(executable);
	if (space == NULL)
	{
		printf("\nInsufficient memory!");
		DEBUG('a', "Insufficient memory!");
		return -1;
	}
	delete executable;
	thread = new Thread(filename);
	if (thread == NULL)
	{
		printf("\nInsufficient memory!");
		DEBUG('a', "Insufficient memory!");
		return -1;
	}
	pid = id;
	thread->processID = id;
	thread->space = space;
	parentID = currentThread->processID;
	//currentThread->space = space;
	thread->Fork(ExecBridge, id);
	currentThread->Yield();
	mutex->V();//signal
	return id;
}

int 
PCB::GetID()
{
	return pid;
}

int 
PCB::GetNumWait()
{
	return numwait;
}


void 
PCB::JoinWait()
{
	joinsem->P();
}

void 
PCB::ExitWait()
{
	exitsem->P();
}

void 
PCB::JoinRelease()
{
	joinsem->V();
}

void 
PCB::ExitRelease()
{
	exitsem->V();
}


void 
PCB::IncNumWait()
{
	mutex->P();
	++numwait;
	mutex->V();
}

void 
PCB::DecNumWait()
{
	mutex->P();
	if (numwait > 0)
		--numwait;
	mutex->V();
}


void 
PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

int
PCB::GetExitCode()
{
	return exitcode;
}


void 
PCB::SetFileName(char* fn)
{
	strcpy(fileName, fn);
}

char* 
PCB::GetFileName()
{
	return fileName;
}
