/* echo program */

#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[])
{
	char mess[255];
	unsigned int i ;
	OpenFileId inConsole = OpenFileFunc("stdin", 2);
	OpenFileId outConsole = OpenFileFunc("stdout", 3);
	if (inConsole == -1 || outConsole == -1)
	{
		PrintString("Console bi lo^~i\n");
		return 0;
	}
	ReadFile(mess, 255, inConsole); 
	
	WriteFile(mess, 255, outConsole);
	CloseFile(inConsole);
	CloseFile(outConsole);
	return 0;
}
