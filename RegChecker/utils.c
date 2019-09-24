#include "utils.h"

int StrIndex(char* str, char ch)
{
	char* ptr = strstr(str, &ch);
	if (ptr == NULL)
	{
		return -1;
	}
	else
	{
		return (ptr - str);
	}
}

char* SubString(char* str, char startChar, char endChar, int* endIndex)
{
	int start = -1;
	int end = -1;
	for (int i = 0; i < strlen(str); i++)
	{
		if (start == -1 && str[i] == startChar)
		{
			start = i;
		}
		else
		{
			if (end == -1 && str[i] == endChar)
			{
				end = i;
			}
		}
	}

	if (start == -1 || end == -1)
	{
		return NULL;
	}

	char* ret = (char*)malloc(sizeof(char) * ((end - start) + 1)); //fix this
	ret[end - start] = '\0';
	strcpy_s(ret, sizeof(char) * (end - start), (str + start));
	if (endIndex != NULL)
	{
		int index = start + (end - start);
		*endIndex = index;
	}
	return ret;
}


LPCSTR getLine(FILE* stream) {
	int c = -1;
	int index = 0;
	char* str = (char*)malloc(sizeof(char));

	while ((c = fgetc(stream)) != EOF && c != 10) {
		str[index] = (char)c;
		str = (char*)realloc(str, sizeof(char) * (index + 2));
		index++;
	}

	str[index] = '\0';
	return str;
}