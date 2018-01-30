#include "syscall.h"
#include "copyright.h"
#define maxlen 32

void ascii()
{
	int i = 32;
	for (;i <= 255; ++i)
	{
		PrintInt(i);
		PrintString(": ");
		PrintChar((char)i);
		PrintString("\n");
	}
}

int main()
{
	ascii();
	Halt();
	return 0;
}