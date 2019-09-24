#pragma once
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <malloc.h>

int StrIndex(char* str, char ch);
char* SubString(char* str, char startChar, char endChar, int* endIndex);
LPCSTR getLine(FILE* stream);
int getAmountOfCharsNot(char* str, char ch);
short strtos(char* string);