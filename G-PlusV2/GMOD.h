#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h>
#include <vector>
#include <stdlib.h>

using namespace std;

class GMOD {
public:
	HANDLE pHandle;
	// Memory management
	DWORD GetModuleBaseAddress(TCHAR* lpszModuleName);
	DWORD GetPointerAddress(DWORD gameBaseAddr, DWORD address, vector<DWORD> offsets);
};
