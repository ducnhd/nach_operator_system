#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[])
{
	char buff[255];
	int isCreate;
	char mess[255];
	OpenFileId open;
        
	PrintString("Nha^p te^n file ca^n tao: \n");
	ReadString(buff, 255);
	isCreate = CreateFile(buff); 
	if (isCreate)
	{
		PrintString("Can not create file \'hello.txt\'\n");
	} 
	else
	{
		PrintString("Successfully create file\n");
		open = OpenFileFunc(buff, 0);
		if (open==-1) PrintString("Can not open file\n");
		WriteFile("123", 3, open);
		SeekFile(0, open);
		ReadFile(mess, 3, open);
	  	PrintString(mess);
		CloseFile(open);
	}
	
	return 0;
}
