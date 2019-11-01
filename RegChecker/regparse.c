#include "regparse.h"
/*
//parse .reg files (going to likely be in whichever program i put the files in, but this is the general idea.
//highly likely going to have to rewrite this in java unless I somehow send the files over or something
reg_file_t* ParseRegistryFile(LPCSTR filePath)
{
	//need to get registry path
	reg_file_t* regFileT = CreateRegFileStruct();
	//FILE* regFile = fopen(filePath, "r");
	//fseek(regFile, 0L, SEEK_SET); //doubt this will fix file issue but why not
	FILE * regFile = OpenWcharFile(filePath);
	if (regFile == NULL)
	{
		printf("RegFile is null\n");
		return NULL;
	}

	char* currentPath = NULL;
	reg_list_t* currentList = NULL;
	bool extendedLine = false;
	while (true)
	{
		if (feof(regFile))
		{
			break;
		}

		printf("1\n");
		LPCSTR line = getLineWchar(regFile); //is this on the stack?
		//LPCSTR line2 = (LPCSTR)malloc(sizeof(char) * strlen(line));
		//strcpy(line2, line);
		//line = line2;
		printf("2 %s\n", line);
		if (line == NULL)
		{
			printf("Line is null, error w malloc?\n");
			return NULL;
		}

		if (line[0] == '[' && line[strlen(line) - 1] == ']')
		{
			printf("3\n");
			char* path = (char*)malloc(sizeof(char) * (strlen(line) - (size_t) 1));
			size_t amtToCpy = sizeof(char) * ((size_t)strlen(line) - (size_t) 2);
			//strncpy_s(path, amtToCpy, (line + 1));
			strncpy_s(path, strlen(line) - (size_t) 1, line + 1, amtToCpy);
			path[strlen(line) - 2] = '\0'; //ensure null terminator
			currentPath = path;
			if (currentList != NULL)
			{
				AddRegPathToFile(regFileT, currentList);
			}
			currentList = CreateRegList(path);
			printf("4\n");
		}
		else
		{
			printf("5\n");
			if (currentPath == NULL)
			{
				printf("6\n");
				//goto cleanup;
			}
			else
			{
				printf("7\n");
				int nameEnd = 0;
				char* name = SubString(line, '\"', '\"', &nameEnd); //key 
				char* type = (char*)malloc(sizeof(char) * 7); //REG_SZ by default
				strcpy_s(type, sizeof(char) * 7, "REG_SZ");
				char* restOfStr = (line + nameEnd + 2); //next char should be an equals
				int valueLen = 0;
				printf("8\n");
				if (restOfStr[0] == '\"')
				{
					printf("9\n");
					//REG_SZ or REG_EXPAND_SZ
					char* newPtr = SubString(restOfStr + 2, '\"', '\"', NULL);
					//free(restOfStr);
					char* fullStr = (char*)malloc(sizeof(char) * (strlen(newPtr) + 3));
					strcpy_s(fullStr, sizeof(char) * (strlen(newPtr) + 3), "\"\\"); //seems they all have "\" on front
					strcat_s(fullStr, sizeof(char) * (strlen(newPtr) + 3), newPtr);
					restOfStr = fullStr;
					//free(newPtr);
					valueLen = strlen(fullStr);
					printf("10\n");

				}
				else
				{
					printf("11\n");
					int typeEnd = 0;
					//free(type);
					type = SubString(restOfStr, NULL, ':', &typeEnd);
					char* valString = (restOfStr + typeEnd + 1); //should now be exactly on the value
					char* currLine = valString;
					size_t currValStringSize = sizeof(char) * strlen(valString);
					printf("12\n");
					while (currLine[strlen(currLine) - 1] == '\\') //currently debugging this section
					{
						printf("13\n");
						//extended lines
						currLine[strlen(currLine) - 1] = '\0'; //remove trailing backslash
						if (currValStringSize != sizeof(char) * strlen(valString)) //check if we arent on first iteration
						{
							printf("14\n");
							char* nextLine = getLineWchar(regFile);
							printf("14.1\n");
							char* nextLine2 = (char*)malloc(sizeof(char) * strlen(nextLine));
							printf("14.2\n");
							strcpy(nextLine2, nextLine);
							printf("14.3\n");
							nextLine = nextLine2;
							printf("nextLine %s\n", nextLine);
							int index = 0;
							printf("15\n");
							while (isspace(nextLine[index]) && index < strlen(nextLine)) //should never hit second condition
							{
								index++;
							}
							printf("16\n");

							char* toUse = (nextLine + index);

							printf("currline - %s, currValSize - %ld, strlen(toUse) - %ld, index - %d\n", currLine, currValStringSize, strlen(toUse), index);
							//currLine = (char*)realloc(currLine, currValStringSize + (sizeof(char) * strlen(toUse)));
							printf("%s - touse %ld %ld %ld\n", toUse, strlen(toUse), currValStringSize, strlen(currLine));
							//currLine = (char*)realloc(currLine, sizeof(char) * (strlen(toUse) + currValStringSize));
							size_t sz2 = strlen(toUse) + currValStringSize;
							printf("%ld\n", sz2);
							//char* newPtr = (char*)realloc(currLine, sizeof(char) * sz2);
							char* newPtr = (char*)malloc(sizeof(char) * sz2);
							printf("17\n");
							if (newPtr != NULL)
							{
								printf("debug - sz2: %lld old %s\n", sz2, currLine);
								strcpy_s(newPtr, sz2, currLine);
								//free(currLine);
								currLine = newPtr;
							}
							else
							{
								//error
								printf("realloc null\n");
								return NULL;
							}
							printf("18\n");
							printf("done realloc\n");
							char* end = (currLine + strlen(currLine));
							strcpy_s(end, sizeof(char) * strlen(toUse) + 1, toUse);
							//free(nextLine);
						}
						printf("19\n");
						//account for strings that are hex bytes, change this type of value to a byte array
					}

					printf("beforeexception: %s\n", type);
					if (strncmp(type, "hex", 3) == 0) //account for hex strings
					{
						printf("20\n");
						char* first = strtok(valString, ",");
						int size = getAmountOfCharsNot(valString, ',') / 2;
						char* bytes = (char*)malloc(sizeof(char) * size);
						int index = 0;
						while (true)
						{
							bytes[index] = strtos(first);
							first = strtok(valString, NULL);
							index++;
							if (first == NULL)
							{
								break;
							}
						}
						//free(valString);
						valString = bytes;
						currValStringSize = size;
						printf("21\n");
						
					}

					AddToRegList(currentList, name, type, valString, currValStringSize);


				}

				printf("22\n");
			}
		}


	cleanup:
		//free(line);
		printf("at cleanup\n");
	}

	fclose(regFile);
	printf("DONE PARSE\n");
	return regFileT;
}*/

//storing this in memory may not be the best idea, worst comes to worst could process the files line by line

//lemme c how much memory this uses when I parse the entire registry for my computer - if its a lot ill change it
reg_file_t* ParseRegistryFile(char* filePath)
{
	FILE* regFile = OpenWcharFile(filePath);
	if (regFile == NULL)
	{
		printf("File does not exist\n");
		return NULL;
	}

	reg_file_t* regFileStruct = CreateRegFileStruct();
	getLineWchar(regFile);
	getLineWchar(regFile);
	//first two lines are useless, lets just pass them
	char* line = NULL;
	while ((line = getLineWchar(regFile)) != NULL)
	{
		printf("LINE: %s\n", line);
		printf("Strlen: %lld\n", strlen(line));
		free(line);
	}

	return NULL;
}