/* reverse program */

#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[])
{
	OpenFileId srcId;
	OpenFileId dstId;
	char source[255], dest[255];
	int filesz, i;
	char c;
	
	PrintString("Nha^p te^n file ca^`n reverse:");
	ReadString(source, 255);
	PrintString("Nha^p te^n file dich:");
	ReadString(dest, 255);

	srcId = OpenFileFunc(source, 1);
	CreateFile(dest);
	dstId = OpenFileFunc(dest, 0);
	if (srcId == -1 || dstId == -1)
	{
		int errorId = srcId == 0 ? 1 : 2;
		PrintString("Kho^ng th^e mo file duoc \n");
		PrintString("Thoat chuong tri`nh\n");
		return 0;
	}
	filesz = SeekFile(-1, srcId);
	SeekFile(0, dstId);
	for (i = filesz - 1; i >= 0; --i)
	{
		SeekFile(i, srcId);
		ReadFile(&c, 1, srcId);
		WriteFile(&c, 1, dstId);
	}
	CloseFile(srcId);
	CloseFile(dstId);
	return 0;
}
