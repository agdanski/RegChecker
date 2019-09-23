// RegChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h> 
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

