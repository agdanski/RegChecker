#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "reglist.h"
#include "utils.h"

reg_file_t* ParseRegistryFile(LPCSTR filePath);