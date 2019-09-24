#pragma once
#include <stdio.h> 
#include <stdbool.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>
#include <malloc.h>
#define MAX_VALUE_NAME 16383
#define MAX_KEY_LENGTH 255

#define REGPATH1 TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")
#define REGPATH2 TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce")
#define REGPATH3 TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices")
#define REGPATH4 TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce")
#define REGPATH5 TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Userinit")

struct reg_entry
{
	char* name;
	char* type;
	char* value;
	int valueLen;
	struct reg_entry* next;
};

typedef struct reg_entry reg_entry_t;

struct reg_list
{
	char* path;
	reg_entry_t* first;
	int size;
	struct reg_list* next;
};

typedef struct reg_list reg_list_t;

struct reg_file
{
	reg_list_t* list;
	int size;
};

typedef struct reg_file reg_file_t;

void printKeyValues(HKEY hKey);
LPCSTR getLine(FILE* stream);
reg_file_t* CreateRegFileStruct();
void FreeRegFile(reg_file_t* regFile);
reg_list_t* GetRegListFromFile(reg_file_t* regFile, int index);
void AddRegPathToFile(reg_file_t* regFile, reg_list_t* regList);
void AddToRegList(
	reg_list_t* regList,
	char* name,
	char* type,
	char* value,
	char* valueLen
);
reg_entry_t* GetEntryFromRegList(reg_list_t* regList, int index);

reg_list_t* CreateRegList(char* path);
void FreeRegList(reg_list_t* regList);
reg_list_t* ParseRegistryFiles(LPCSTR filePath);