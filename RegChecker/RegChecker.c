// RegChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "RegChecker.h"

static int StrIndex(char* str, char ch);
static char* SubString(char* str, char startChar, char endChar, int* endIndex);
void printKeyValues(HKEY key);

const TCHAR * regPaths[] = {
	REGPATH1, REGPATH2, REGPATH3, REGPATH4
};

//error more data available
int main2(){
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

int main()
{
	//load in a copy of the entire windows registry
	/*printf("Hello\n");
	printf("testing getline - ");
	char* line = getLine(stdin);
	fflush(stdout);
	printf("line was %s\n", line);

	return 0;*/
	//getline is clearly working with stdin, why isnt it working with this specific file...
	//further testing - seems to only be .reg files that are messing up getline
	//im dumb - BOM is only on the first line lmao
	FILE* file = fopen("testregexport.reg", "rb"); //There is a byte order mark in utf-16 - remove it
	fseek(file, sizeof(wchar_t), SEEK_SET); //strip BOM
	//always gonna be utf16le for reg files
	FILE* output = fopen("output.txt", "w");
	while (!feof(file))
	{
		char* line = getLineWchar(file);
		fprintf(output, "%s\n", line);
	}

	fclose(file);
	fclose(output);
	return 0;
	reg_file_t* regFile = ParseRegistryFile("testregexport.reg");
	if (regFile == NULL)
	{
		printf("regfile is null\n");
		return -1;
	}
	else
	{
		PrintRegFile(regFile);
	}

	return 0;
}








