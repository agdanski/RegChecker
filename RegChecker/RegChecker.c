// RegChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

void printKeyValues(HKEY hKey);

const TCHAR * regPaths[] = {
	REGPATH1, REGPATH2, REGPATH3, REGPATH4
};

//error more data available
int main(){
	HKEY key;
	printf("Reading from HKEY_CURRENT_USER\n");
	for (int i = 0; i < 4; i++) {
		DWORD result = RegOpenKeyEx(
			HKEY_CURRENT_USER,
			regPaths[i],
			0,
			KEY_READ | KEY_WOW64_64KEY,
			&key
		);

		if (result == ERROR_SUCCESS) {
			_tprintf(TEXT("Printing values from %s\n"), regPaths[i]);
			printKeyValues(key);
			printf("Done\n");
		}
		else {
			_tprintf(TEXT("Error opening registry key with path %s: %ld\n"), regPaths[i], result);
		}
		RegCloseKey(key);
	}

	printf("Reading from HKEY_LOCAL_MACHINE\n");
	for (int i = 0; i < 4; i++) {
		DWORD result = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			regPaths[i],
			0,
			KEY_READ | KEY_WOW64_64KEY,
			&key
		);

		if (result == ERROR_SUCCESS) {
			_tprintf(TEXT("Printing values from %s\n"), regPaths[i]);
			printKeyValues(key);
			printf("Done\n");
		}
		else {
			_tprintf(TEXT("Error opening registry key with path %s: %ld\n"), regPaths[i], result);
		}
		RegCloseKey(key);
	}
	printf("Reading from userinit\n");
	DWORD result = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		REGPATH5,
		0,
		KEY_READ | KEY_WOW64_64KEY,
		&key
	);
	if (result != ERROR_SUCCESS) {
		printf("Error opening userinit key: %ld\n", result);

	}
	else {
		printKeyValues(key);
	}
	RegCloseKey(key);

	return 0;
}


//im thinking issues are bcus not run as administrator
void printKeyValues(HKEY hKey) //eventually try and cut some stack space off this, but this should be a decent baseline
{
	DWORD bName;
	DWORD cValues;
	DWORD cbMaxValueData;
	DWORD i, retCode;

	TCHAR* achValue = (TCHAR*)malloc(sizeof(TCHAR) * MAX_VALUE_NAME); //can probably optimize this further
	DWORD cchValue = MAX_VALUE_NAME;
	DWORD cbMaxSubKey;
	retCode = RegQueryInfoKey(
		hKey,
		NULL,
		NULL,
		NULL,
		NULL,
		&cbMaxSubKey,
		NULL,
		&cValues,
		NULL,
		&cbMaxValueData,
		NULL,
		NULL
	);
	
	TCHAR* achKey = (TCHAR*)malloc(sizeof(TCHAR) * cbMaxSubKey);

	// Enumerate the key values. 

	if (cValues)
	{
		printf("\nNumber of values: %d\n", cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValue(hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, achValue);
				LPCWSTR value = (LPCWSTR)malloc(sizeof(WCHAR) * cbMaxValueData);
				DWORD getValCode = RegGetValueW(hKey, NULL, achValue, RRF_RT_REG_SZ, NULL, value, &cbMaxValueData);
				if (getValCode != ERROR_SUCCESS) {
					printf("Error with RegGetValue: %ld\n", getValCode);
					
				}
				else {
					wprintf(L"Value: %ls\n", value);
				}
			}
		}
	}
	free(achKey);
	free(achValue);
}



/*
[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run]
[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunOnce]
[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunServices]
[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunServicesOnce]
[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\Userinit]

[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run]
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunOnce]
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunServices]
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunServicesOnce]
[HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\Windows]
*/


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

reg_file_t* CreateRegFileStruct()
{
	reg_file_t* regFile = (reg_file_t*)malloc(sizeof(reg_file_t));
	memset(regFile, 0x0, sizeof(reg_file_t));
	regFile->list = NULL;
	regFile->size = 0;
	return regFile;
}

void FreeRegFile(reg_file_t* regFile)
{
	for (int i = 0; i < regFile->size; i++)
	{
		FreeRegList(GetRegListFromFile(regFile, i));
	}

	free(regFile);
}

void AddRegPathToFile(reg_file_t* regFile, reg_list_t* regList)
{
	if (regFile->list == NULL)
	{
		regFile->list = regList;
	}
	else
	{
		reg_list_t* ptr = regFile->list;

		while (true)
		{
			if (ptr->next != NULL)
			{
				ptr = ptr->next;
			}
			else
			{
				break;
			}
		}

		ptr->next = regList;
	}
	regFile->size += 1;
}

void AddToRegList(
	reg_list_t* regList, 
	char* name, 
	char* type, 
	char* value, 
	char* valueLen
)
{
	reg_entry_t* regEntry = (reg_entry_t*)malloc(sizeof(reg_entry_t));
	memset(regEntry, 0x0, sizeof(reg_entry_t));
	regEntry->next = NULL;
	regEntry->name = name;
	regEntry->type = type;
	regEntry->value = value;
	regEntry->valueLen = valueLen;
	if (regList->first == NULL)
	{
		regList->first = regEntry;
	}
	else
	{
		reg_entry_t* ptr = regList->first;
		while (true)
		{
			if (ptr->next != NULL)
			{
				ptr = ptr->next;
			}
			else
			{
				break;
			}
		}

		ptr->next = regEntry;
	}


	regList->size += 1;
}

reg_entry_t* GetEntryFromRegList(reg_list_t* regList, int index)
{
	if (index >= regList->size)
	{
		return NULL;
	}
	reg_entry_t* ptr = regList->first;
	int curr = 0;
	while (curr != index)
	{
		ptr = ptr->next;
	}

	return ptr;
}

reg_list_t* CreateRegList(char * path)
{
	reg_list_t* regList = (reg_list_t*)malloc(sizeof(reg_list_t));
	memset(regList, 0x0, sizeof(reg_list_t));
	regList->size = 0;
	regList->first = NULL;
	regList->path = (char*)malloc(sizeof(char) * strlen(path));
	regList->next = NULL;
	strcpy_s(regList->path, strlen(path) * sizeof(char), path);
	return regList;
}

void FreeRegList(reg_list_t* regList)
{
	for (int i = 0; i < regList->size; i++)
	{
		reg_entry_t* regEntry = GetEntryFromRegList(regList, i);
		free(regEntry);
	}

	free(regList->path);
	free(regList);
}


//parse .reg files (going to likely be in whichever program i put the files in, but this is the general idea.
//highly likely going to have to rewrite this in java unless I somehow send the files over or something
reg_list_t * ParseRegistryFiles(LPCSTR filePath) 
{
	//need to get registry path
	
	reg_list_t* regList = CreateRegList();
	FILE* regFile = fopen(filePath, "r");
	char* currentPath = NULL;
	while (true)
	{
		if (feof(regFile))
		{
			break;
		}


	}

	fclose(regFile);
	return regList;
}

