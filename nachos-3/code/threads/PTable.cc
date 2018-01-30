// PTABLE.cc
#include "PTable.h"

PTable::PTable(int size)
{
	int i;
	bm = new BitMap(size);
	pcb = new PCB *[size];
	psize = size;
	bmsem = new Semaphore("bmsem", 1);
	for (i = 0; i < size; ++i)
	{
		pcb[i] = NULL;
	}
	bm->Mark(0);
	pcb[0] = new PCB(0);
	pcb[0]->parentID = -1;
}

PTable::~PTable()
{
	int i;

	delete bm;
	for (i = 0; i < psize; ++i)
	{
if (pcb[i] != NULL)
		delete pcb[i];
	}
//if (pcb != NULL);
//	delete []pcb;
	if (bmsem != NULL)
delete bmsem;
}


int 
PTable::ExecUpdate(char *name)
{
	bmsem->P(); //wait
	if (name == NULL || strlen(name) > 32)
	{
		printf("PTable::ExecUpdate:Invalid file!\n");
		DEBUG('a', "PTable::ExecUpdate:Invalid file!\n");
		bmsem->V();
		return -1;
	}

	if (fileSystem->Open(name) == NULL)
	{
		printf("PTable::ExecUpdate:The file doesn't exist!\n");
		DEBUG('a', "PTable::ExecUpdate:The file doesn't exist!\n");
		bmsem->V(); //signal
		return -1;
	}

	if (strcmp(name, currentThread->getName()) == 0)
	{
		printf("PTable::ExecUpdate:ERROR!\n");
		DEBUG('a', "PTable::ExecUpdate:ERROR!\n");
		bmsem->V(); //signal
		return -1;
	}


	int id = GetFreeSlot();

	if (id == -1)
	{
		printf("PTable::ExecUpdate:Full!\n");
		DEBUG('a', "PTable::ExecUpdate:Full!\n");
		bmsem->V(); //signal
		return -1;
	}
	pcb[id] = new PCB(id);
	if (pcb[id] == NULL)
	{
		printf("PTable::ExecUpdate:Insufficient memory!\n");
		DEBUG('a', "PTable::ExecUpdate:Insufficient memory!\n");
		bmsem->V();
		return -1;
	}
	int result = pcb[id]->Exec(name, id); //goi Exec cua class PCB
	bmsem->V();//signal
	return result;
}

int 
PTable::ExitUpdate(int ec)
{
	int id = currentThread->processID;
	if (id == 0)
	{
		interrupt->Halt();
		return 0;
	}
	//currentThread->setStatus();
	pcb[id]->SetExitCode(ec);
	pcb[id]->JoinRelease();

	pcb[id]->ExitWait();

	//currentThread->processID = pcb[id]->parentID;
	pcb[id]->DecNumWait();
	Remove(id);
	return 0;
}

int 
PTable::JoinUpdate(int id)
{
	//IntStatus oldLevel = interrupt->SetLevel(IntOff);
	//printf("%d", id);
	if (id < 0 || id >= psize)
	{
		printf("Insufficient memory!\n");
		DEBUG('a', "Insufficient memory!\n");
	//interrupt->SetLevel(oldLevel);
		return -1;
	}

	if (IsExist(id) == false)
	{
		printf("Don't initialize!\n");
		DEBUG('a', "Don't initialize!\n");
		//interrupt->SetLevel(oldLevel);
		return -1;
	}
	if (pcb[id]->parentID != currentThread->processID)
	{
		printf("Error!\n");
		DEBUG('a', "Error!\n");
		//interrupt->SetLevel(oldLevel);
		return -1;
	}
	//currentThread->setStatus(BLOCKED);
	//bmsem->P();
	pcb[id]->IncNumWait();
	//bmsem->V();
	pcb[id]->JoinWait();
	int exitcode = pcb[id]->GetExitCode();
	//bmsem->P();
	pcb[id]->ExitRelease();
	//bmsem->V();
	//currentThread->setStatus(RUNNING);
	//currentThread->processID = id;
//	interrupt->SetLevel(oldLevel);
	return exitcode;
}

int 
PTable::GetFreeSlot()
{
	return bm->Find();
}

bool 
PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

void 
PTable::Remove(int pid)
{
	if (pcb[pid] != NULL)
	delete pcb[pid];
	pcb[pid] = NULL;
	bm->Clear(pid);
}

char* 
PTable::GetFileName(int id)
{
	return 	pcb[id]->GetFileName();
}
