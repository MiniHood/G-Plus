#pragma once

#include <iostream>
#include "Windows.h"
#include "variant"

variant<int, string, char> Q_WriteProcessMemory(variant<int, string, char> input, int process_id, LPVOID lpBaseAddress);
variant<int, string, char> Q_ReadProcessMemory(int process_id, variant<LPCVOID> lpBaseAddress, variant<SIZE_T> size);