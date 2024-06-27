#pragma once

#include <iostream>
#include <Windows.h>

#define bool M_WriteProcessMemory(int process_id, LPVOID lpBaseAddress, LPCVOID lpBuffer);
#define bool M_ReadProcessMemory(int process_id, LPCVOID lpBaseAddress, LPVOID lpBuffer);