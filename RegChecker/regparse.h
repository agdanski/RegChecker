#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "reglist.h"

reg_file_t* ParseRegistryFile(LPCSTR filePath);