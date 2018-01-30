/* cat program */
#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[])
{
	OpenFileId fileId;
	int filesz, i;
	char c;
	char name[255];
	PrintString("Nha^p te^n file ca^`n xem no^i dung:");
	ReadString(name, 255);
	if ( (fileId= OpenFileFunc(name, 1)) == -1)
	{
		PrintString("Kho^ng the^* mo* file");
		return 0;
	}
	filesz = SeekFile(-1, fileId);
	i = 0;
	SeekFile(0, fileId);
	
	for (; i < filesz; ++i)
	{
		ReadFile(&c, 1, fileId);
		PrintChar(c);
	} 
	CloseFile(fileId);
	return 0;
}
