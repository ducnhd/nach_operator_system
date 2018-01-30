#include "syscall.h"
#include "copyright.h"

int ConvertToInt(char *s, int len);
int ConvertToString(int n, char *s);

int main()
{
	char s[10];
	char v[10];
	int t;
	int i = 0;
	int size;
	int idFile1;
	int idFile2;

	idFile1 = OpenF("information.txt", 0);
	ReadF(&s[i], 1, idFile1);
	do
	{
		++i;
		ReadF(&s[i], 1, idFile1);
	} while (s[i] != '\n');
	s[i] = '\0';
	t = ConvertToInt(s, i) + 1;
	size = ConvertToString(t, v);
	v[size] = '\0';
	CloseF(idFile1);
	
	CreateF("information.txt");
	idFile1 = OpenF("information.txt", 0);
	idFile2 = OpenF("input.txt", 0);

	WriteF(v, size, idFile1);
	WriteF("\n", 1, idFile1);
	i = 0;
	do
	{
		ReadF(&s[0], 1, idFile2);
		if (s[0] == '\n')
			++i;
	} while (t != i);
	
	do
	{
		ReadF(&s[0], 1, idFile2);
		WriteF(&s[0], 1, idFile1);
	} while (s[0] != '\n');
	CloseF(idFile1);
	CloseF(idFile2);
	Exit(0);
	return 0;
}

int ConvertToInt(char *s, int len)
{
	int i;
	int num = 0;

	for (i = 0; i < len; ++i)
	{
		num = num * 10 + s[i] - 48;
	}
	return num;
}

int ConvertToString(int n, char *s)
{
	int i = 0;
	int size;
	int mid;
	int tmp;

	do
	{
		s[i] = n % 10 + '0';
		++i;
		n /= 10;
	} while (n > 0);
	size = i;
	mid = size / 2;
	
	while (mid < i)
	{
		tmp = s[size - i];
		s[size - i] = s[i - 1];
		s[i - 1] = tmp;
		--i;
	}
	return size;
}
