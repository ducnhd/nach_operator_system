// fdtable.h 
#include "fdtable.h"
#include "system.h"

FDTable::FDTable(int isize)
{
	size = isize;
	flags = new BitMap(size);
	listFile = new FilePtr[size];
}

FDTable::~FDTable()
{
	for (int i = 2; i < size; ++i)
	{
		if (flags->Test(i))
			delete listFile[i].pFile;
	}
	delete flags;
	delete[]listFile;
}

int 
FDTable::FindIdFile(char *name, int type)
{
	int id;

	if (type < 0 || type > 1)
	{
		printf("\n Error: type < 0 or type > 1");
		DEBUG('f', "\n Error: type < 0 or type > 1");
		return -1;
	}

	if (strcmp(name, "cin") == 0)
	{
		printf("\n Console input");
		DEBUG('f', "\n Console input");
		flags->Mark(0);
		return 0;
	}

	if (strcmp(name, "cout") == 0)
	{
		printf("\n Console output");
		DEBUG('f', "\n Console output");
		flags->Mark(1);
		return 1;
	}

	id = flags->Find(2, size - 1);
	
	if (id == -1)
	{
		printf("\n Full");
		DEBUG('f', "\n Full!");
		return -1;
	}
	listFile[id].pFile = fileSystem->Open(name);
	listFile[id].type = type;
	if (listFile[id].pFile == NULL)
	{
		printf("\n Can't find the file!");
		DEBUG('f', "\n Can't find the file!");
		flags->Clear(id);
		return -1;
	}
	return id;
}

OpenFile* 
FDTable::Open(int idFile)
{
	if (idFile >= 2 && idFile < 10)
	{
		return listFile[idFile].pFile;
	}
	return NULL;
}

bool 
FDTable::isWrite(int idFile)
{
	if (idFile >= 0 && idFile < 10)
	{
		if (flags->Test(idFile))
		{
			if (idFile == 1 || listFile[idFile].type == 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool 
FDTable::isRead(int idFile)
{
	if (idFile >= 0 && idFile < 10)
	{
		if (flags->Test(idFile))
		{
			if (idFile == 0 || listFile[idFile].type == 0 || listFile[idFile].type == 1)
			{
				return true;
			}
		}
	}
	return false;
}

int 
FDTable::Close(int idFile)
{
	if (idFile >= 0 && idFile < 2)
	{
		flags->Clear(idFile);
		return 0;
	}
	if (idFile >= 2 && idFile < 10)
	{
		delete listFile[idFile].pFile;
		flags->Clear(idFile);
		return 0;
	}
	return -1;
}
