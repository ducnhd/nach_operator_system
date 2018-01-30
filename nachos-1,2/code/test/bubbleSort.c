#include "syscall.h"
#include "copyright.h"




int main()
{
	int N;
	int array[100]; //du`ng le^nh ca^'p pha't kho^ng duo*c
	int i, m, n;

	PrintString("****Chuo*ng tri`nh BubbleSort****\n");
	PrintString("Nha^p vao so nguye^n ca^`n sort: ");
	
	N = ReadInt();

	PrintString("\n");
	PrintString("Nha^p gia' tri cho ma*ng: \n");
	

	for(i = 0 ; i < N; i++)
	{
		PrintString("Gia' tri thu*' ");
		PrintInt(i + 1);
		PrintString(" la`: ");
		array[i] = ReadInt();
		PrintString("\n");
	}
	
	
	for(m = 0; m < N; m++)
	{
		for(n = N - 1; n > m; n--)
		{
			if(array[n] < array[m])
			{
				int temp = array[n];
				array[n] = array[m];
				array[m] = temp;
			}
		}
	}
	
	PrintString("\n");
	PrintString("Ma*ng sau khi sap xe^'p: \n");
	
	for(i = 0; i < N; i++)
	{
		PrintInt(array[i]);
		PrintChar(' ');
	}
	PrintString("\n");
}
