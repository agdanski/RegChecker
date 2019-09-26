#pragma once
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <malloc.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

int StrIndex(char* str, char ch);
char* SubString(char* str, char startChar, char endChar, int* endIndex);
char * getLine(FILE* stream);
char* getLineWchar(FILE* stream);
int getAmountOfCharsNot(char* str, char ch);
short strtos(char* string);
FILE* OpenWcharFile(LPCSTR filePath);