#include "syscall.h"

int main()
{
	int i;
	PrintString("****Ba*ng ma~ AscII****\n");
	for(i = 0; i < 256; i++)
	{
		char temp = i - '0';
		PrintChar(temp);
	}
	PrintString("\n");
}
