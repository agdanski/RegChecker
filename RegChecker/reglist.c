#include "reglist.h"

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

reg_list_t* GetRegListFromFile(reg_file_t* regFile, int index)
{
	if (index >= regFile->size)
	{
		return NULL;
	}

	reg_list_t* ptr = regFile->list;
	int curr = 0;
	while (curr != index)
	{
		ptr = ptr->next;
		curr++;
	}

	return ptr;
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
	regFile->size = regFile->size + 1;
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


	regList->size = regList->size + 1;
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
		curr++;
	}

	return ptr;
}

reg_list_t* CreateRegList(char* path)
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

void PrintRegFile(reg_file_t* regFile)
{
	printf("REGFILE PRINT - %d\n", regFile->size);
	for (int i = 0; i < regFile->size; i++)
	{
		reg_list_t* regList = GetRegListFromFile(regFile, i);
		printf("[%s]\n\n", regList->path);
		
		for (int x = 0; x < regList->size; x++)
		{
			reg_entry_t* entry = GetEntryFromRegList(regList, x);
			printf("Type: %s\n, Name: %s, ValueLen: %d\n", entry->type, entry->name, entry->valueLen);
			for (int y = 0; y < entry->valueLen; y++)
			{
				printf("%c, ", entry->value[y]);
			}

			printf("\n");
		}

	}
}