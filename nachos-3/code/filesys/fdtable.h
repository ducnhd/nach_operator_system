// fdtable.h 

#ifndef FDTABLE_H
#define FDTABLE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#define MAX_FILE 10

class FDTable {
public:
	FDTable(int isize);
	~FDTable();
	int FindIdFile(char *name, int type);	
	OpenFile* Open(int idFile); 	// Open a file
	bool isWrite(int idFile);	// Check a Write file
	bool isRead(int idFile);	// Check a Read file
	int Close(int idFile);		// Close a file
private:
	int size;
	BitMap *flags;
	struct FilePtr
	{
		OpenFile *pFile;
		int type;
	} *listFile;
};


#endif // FDTABLE
