#include "regparse.h"

//parse .reg files (going to likely be in whichever program i put the files in, but this is the general idea.
//highly likely going to have to rewrite this in java unless I somehow send the files over or something
reg_file_t* ParseRegistryFiles(LPCSTR filePath)
{
	//need to get registry path

	reg_file_t* regFileT = CreateRegFileStruct();
	FILE* regFile = fopen(filePath, "r");
	char* currentPath = NULL;
	reg_list_t* currentList = NULL;
	bool extendedLine = false;
	while (true)
	{
		if (feof(regFile))
		{
			break;
		}

		char* line = getLine(regFile);

		if (line[0] == '[' && line[strlen(line) - 1] == ']')
		{
			char* path = (char*)malloc(sizeof(char) * (strlen(line) - 1));
			strcpy_s(path, sizeof(char) * (strlen(line) - 2), (line + 1));
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
				char* restOfStr = (name + nameEnd + 2); //next char should be an equals
				int valueLen = 0;
				if (restOfStr[0] == '\"')
				{
					//REG_SZ or REG_EXPAND_SZ
					char* oldPtr = restOfStr;
					restOfStr = SubString(restOfStr, '\"', '\"', NULL);
					free(oldPtr);
					valueLen = strlen(restOfStr);

				}
				else
				{
					int typeEnd = 0;
					free(type);
					type = SubString(restOfStr, restOfStr[0], ':', &typeEnd);
					char* valString = (restOfStr + typeEnd + 2); //should now be exactly on the value
					char* currLine = valString;
					size_t currValStringSize = sizeof(char) * strlen(valString);

					while (currLine[strlen(currLine) - 1] == '\\')
					{
						//extended lines
						currLine[strlen(currLine) - 1] = '\0'; //remove trailing backslash
						if (currValStringSize != sizeof(char) * strlen(valString)) //check if we arent on first iteration
						{
							char* nextLine = getLine(regFile);
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


				}


			}
		}


	cleanup:
		free(line);
	}

	fclose(regFileT);
	return regFileT;
}