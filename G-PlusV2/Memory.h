#pragma once

#include <iostream>
#include <Windows.h>

bool M_WriteProcessMemory(int process_id, LPVOID lpBaseAddress, LPCVOID lpBuffer);
bool M_ReadProcessMemory(int process_id, LPCVOID lpBaseAddress, LPVOID lpBuffer);