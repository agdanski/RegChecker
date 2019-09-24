#pragma once
#include <stdbool.h>
#include <Windows.h>
#include <malloc.h>
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