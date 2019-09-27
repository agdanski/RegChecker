#include "regparse.h"

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


		LPCSTR line = getLineWchar(regFile);
		if (line[0] == '[' && line[strlen(line) - 1] == ']')
		{
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
		}
		else
		{
			if (currentPath == NULL)
			{
				goto cleanup;
			}
			else
			{
				int nameEnd = 0;
				char* name = SubString(line, '\"', '\"', &nameEnd); //key 
				char* type = (char*)malloc(sizeof(char) * 7); //REG_SZ by default
				strcpy_s(type, sizeof(char) * 7, "REG_SZ");
				char* restOfStr = (line + nameEnd + 2); //next char should be an equals
				int valueLen = 0;
				if (restOfStr[0] == '\"')
				{
					//REG_SZ or REG_EXPAND_SZ
					char* newPtr = SubString(restOfStr, '\"', '\"', NULL);
					//free(restOfStr);
					restOfStr = newPtr;
					valueLen = strlen(restOfStr);

				}
				else
				{
					int typeEnd = 0;
					free(type);
					type = SubString(restOfStr, NULL, ':', &typeEnd);
					char* valString = (restOfStr + typeEnd + 2); //should now be exactly on the value
					char* currLine = valString;
					size_t currValStringSize = sizeof(char) * strlen(valString);

					while (currLine[strlen(currLine) - 1] == '\\')
					{
						//extended lines
						currLine[strlen(currLine) - 1] = '\0'; //remove trailing backslash
						if (currValStringSize != sizeof(char) * strlen(valString)) //check if we arent on first iteration
						{
							char* nextLine = getLineWchar(regFile);
							int index = 0;
							while (isspace(index) && index < strlen(nextLine)) //should never hit second condition
							{
								index++;
							}

							char* toUse = (nextLine + index);


							currLine = (char*)realloc(currLine, currValStringSize + (sizeof(char) * strlen(toUse)));
							char* end = (currLine + strlen(currLine));
							strcpy_s(end, sizeof(char) * (strlen(toUse) + 1), toUse);
							free(nextLine);
						}

						//account for strings that are hex bytes, change this type of value to a byte array
					}
					if (strncmp(type, "hex", 3) == 0) //account for hex strings
					{
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
						free(valString);
						valString = bytes;
						currValStringSize = size;
						
					}

					AddToRegList(currentList, name, type, valString, currValStringSize);


				}

				printf("11\n");
			}
		}


	cleanup:
		free(line);
	}

	fclose(regFile);
	printf("DONE PARSE\n");
	return regFileT;
}