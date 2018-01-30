#include "syscall.h"
#include "copyright.h"
#define maxlen 32

void sortAscend(int* buffer, int size)
{
	int tmp;
	int i, j;

	for (i = 0; i < size; i++)
	{
		for (j = i + 1; j < size; j++)
		{
			if (buffer[j] < buffer[i])
			{
				tmp = buffer[i];
				buffer[i] = buffer[j];
				buffer[j] = tmp;
			}
		}
	}
}

void sortDescend(int* buffer, int size)
{
	int tmp;
	int i, j;

	for (i = 0; i < size; i++)
	{
		for (j = i + 1; j < size; j++)
		{
			if (buffer[j] > buffer[i])
			{
				tmp = buffer[i];
				buffer[i] = buffer[j];
				buffer[j] = tmp;
			}
		}
	}
}


int main()
{
	int s[100];
	int n = 0;
	int i,j;
	int type;

	do{
		PrintString("n = ");
		n = ReadInt();
	} while (n > 100 || n <= 0);

	for (i = 0; i < n; ++i)
	{
		PrintInt(i);
		PrintString(": ");
		s[i] = ReadInt();
	}

	PrintString("Sort (1/Ascending | 2/Descending): ");
	
	do
	{
		type = ReadInt();
		switch (type)
		{
		case 1:
			sortAscend(s, n);
			break;
		case 2:
			sortDescend(s, n);
			break;
		default:
			type = 0;
		}
	} while (type == 0);
	
	for (i = 0; i < n; ++i)
	{
		PrintInt(s[i]);
		PrintChar('\n');
	}
	Halt();
	return 0;
}