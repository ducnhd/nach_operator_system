#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main()
{
	int id;

	CreateF("input.txt");


	id = OpenF("input.txt", 0);
	WriteF("3\n", 2, id);
	WriteF("4 2 1\n", 6, id);
	WriteF("5 3 4 1\n", 8, id);
	WriteF("8 5 2\n", 6, id);
	CloseF(id);
	
	
	
	Halt();
	return 0;
}

