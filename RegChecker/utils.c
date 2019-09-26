#include "utils.h"

static int hexadecimalToDecimal(char* hexVal);

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


//seems to be an issue with this function here.
char * getLine(FILE* stream) {
	int c = -1;
	int index = 0;
	char* str = (char*)malloc(sizeof(char));
	if (str == 0)
	{
		printf("NULL PTR - GETLINE\n");
		return NULL;
	}

	/*while ((c = fgetc(stream)) != EOF && c != 10) {
		printf("HIIIII\n");
		str[index] = (char)c;
		str = (char*)realloc(str, sizeof(char) * (index + 2));
		index++;
	}*/

	//str[index] = '\0';
	//printf("returning %d\n", strlen(str));

	while (!feof(stream))
	{
		char* line = NULL;
		c = fgetc(stream);
		if (c == EOF || c == '\n' || '\0')
		{
			break;
		}
		str[index] = (char)c;
		index++;
		size_t size = (size_t)index;
		size++;
		str = (char*)realloc(str, sizeof(char) * (size));
		if (str == 0)
		{
			printf("STR NULL\n");
			return NULL;
		}
	}
	str[index] = '\0';
	return str;
}

char* getLineWchar(FILE* stream) 
{
	setlocale(LC_ALL, "en_US.UTF-16");
	wint_t c = -1;
	int index = 0;
	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t));
	if (str == 0)
	{
		printf("NULL PTR - GETLINE\n");
		return NULL;
	}

	while (!feof(stream))
	{
		wchar_t* line = NULL;
		c = fgetwc(stream);
		if (c == WEOF) // newline is different w this file, use CRLF
		{
			break;
		}

		//newline fix
		if (c == 0xD)
		{
			wint_t c1 = fgetwc(stream);
			if (c1 == 0xA)
			{
				break;
			}
			else
			{
				//set the stream back by 1 (on windows a wchar is 2 bytes, probably shouldnt make this constant but whatever)
				fseek(stream, -2L, SEEK_CUR);
			}
		}
		str[index] = (wchar_t)c;
		index++;
		str = (wchar_t*)realloc(str, sizeof(wchar_t) * (index + 1));
		if (str == 0)
		{
			printf("STR NULL\n");
			return NULL;
		}
	}
	str[index] = L'\0';
	char* asciiStr = (char*)malloc(sizeof(char) * (wcslen(str) + 1));
	size_t size = (size_t)wcslen(str);
	size++;
	size = size * sizeof(wchar_t);
	size_t result = wcstombs(asciiStr, str, size);
	setlocale(LC_ALL, "en_US");
	return asciiStr;
}

int getAmountOfCharsNot(char* str, char ch)
{
	int amt = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] != ch)
		{
			amt++;
		}
	}

	return amt;
}

short strtos(char* string)
{
	int val = hexadecimalToDecimal(string);
	if (val < 128 && val >= -128)
	{
		return (short)val;
	}
	else
	{
		return -1;
	}
}

//geeksforgeeks lmao
static int hexadecimalToDecimal(char * hexVal)
{
	int len = strlen(hexVal);

	// Initializing base value to 1, i.e 16^0 
	int base = 1;

	int dec_val = 0;

	// Extracting characters as digits from last character 
	for (int i = len - 1; i >= 0; i--)
	{
		// if character lies in '0'-'9', converting  
		// it to integral 0-9 by subtracting 48 from 
		// ASCII value. 
		if (hexVal[i] >= '0' && hexVal[i] <= '9')
		{
			dec_val += (hexVal[i] - 48) * base;

			// incrementing base by power 
			base = base * 16;
		}

		// if character lies in 'A'-'F' , converting  
		// it to integral 10 - 15 by subtracting 55  
		// from ASCII value 
		else if (hexVal[i] >= 'A' && hexVal[i] <= 'F')
		{
			dec_val += (hexVal[i] - 55) * base;

			// incrementing base by power 
			base = base * 16;
		}
	}

	return dec_val;
}

FILE* OpenWcharFile(LPCSTR filePath)
{
	FILE* file = fopen(filePath, "rb"); //utf16 files have to be opened in rb
	fseek(file, sizeof(wchar_t), SEEK_SET); //remove BOM - we know its gonna be little endian
	return file;
}