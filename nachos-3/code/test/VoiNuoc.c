#include "syscall.h"
#include "copyright.h"

int ConvertToInt(char *s, int len);

int main()
{
	char s[10];
	char temp[10];
	int t;
	int k;
	int i = 0;
	int idFile1;
	int idFile2;
	int tap1 = 0, tap2 = 0;

	idFile1 = OpenF("information.txt", 0);
	idFile2 = OpenF("output.txt", 0);

	ReadF(&s[i], 1, idFile1);
	do
	{
		++i;
		ReadF(&s[i], 1, idFile1);
		
	} while (s[i] != '\n');
	s[i] = '\0';
	t = ConvertToInt(s, i);

	if (t > 1)
	{
		i = 1;
		do
		{
			ReadF(&s[0], 1, idFile2);
			if (s[0] == '\n')
			{
				++i;
			}
		} while (t != i);
	}
	i = 0;
	do
	{
		ReadF(&s[0], 1, idFile1);
		if (s[0] == ' ')
		{
			temp[i] = '\0';
			k = ConvertToInt(temp, i);
			if (tap1 <= tap2)
			{
				tap1 += k;
				WriteF("1 ", 2, idFile2);
			}
			else
			{
				tap2 += k;
				WriteF("2 ", 2, idFile2);
			}
			i = 0;
		}
		else if (s[0] == '\n')
		{
			temp[i] = '\0';
			k = ConvertToInt(temp, i);
			if (tap1 <= tap2)
			{
				tap1 += k;
				WriteF("1\n", 2, idFile2);
			}
			else
			{
				tap2 += k;
				WriteF("2\n", 2, idFile2);
			}
			break;
		}
		else
		{
			temp[i] = s[0];
			++i;
		}
	} while (1);

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
