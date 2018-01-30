// STable.cc 

#include "STable.h"

STable::STable(int isize)
{
	if (isize <= 0)
	{
		printf("isize <= 0\n");
		DEBUG('a', "isize <= 0\n");
		return;
	}
	size = isize;
	bm = new BitMap(size);
	semTab = new Sem *[size];
	for (int i = 0; i < size; ++i)
	{
		semTab[i] = NULL;
	}
}

STable::~STable()
{
	delete bm;
	for (int i = 0; i < size; ++i)
		delete semTab[i];
	delete[]semTab;
}

int STable::Create(char *name, int init)
{
	for (int i = 0; i < size; ++i)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				printf("Error!\n");
				DEBUG('a', "Error!\n");
				return -1;
			}
		}
	}
	int solt = FindFreeSolt();
	if (solt == -1)
	{
		printf("Error!\n");
		DEBUG('a', "Error!\n");
		return -1;
	}
	semTab[solt] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name)
{
	for (int i = 0; i < size; ++i)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				semTab[i]->Wait();
				return 0;
			}
		}
	}
	printf("Error!\n");
	DEBUG('a', "Error!\n");
	return -1;
}

int STable::Signal(char *name)
{
	for (int i = 0; i < size; ++i)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				semTab[i]->Signal();
				return 0;
			}
		}
	}
	printf("Error!\n");
	DEBUG('a', "Error!\n");
	return -1;
}

int STable::FindFreeSolt()
{
	return bm->Find();
}
