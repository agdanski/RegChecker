#pragma once
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

