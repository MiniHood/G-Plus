#pragma once

#include "Windows.h"
#include <cstdint>
#include <string>

namespace Memory {
	static HWND get_process_wnd(uint32_t pid);
	uintptr_t get_base_address(int pid, std::string module);
	template <class T>
	static T ReadProcMemory(HANDLE hProcess, uintptr_t Address)
	{
		T value;
		ReadProcessMemory(hProcess, (LPCVOID)Address, &value, sizeof(value), NULL);
		return value;
	}

	template <class T>
	static void WriteProcMemory(HANDLE hProcess, uintptr_t address, T value)
	{
		WriteProcessMemory(hProcess, address, (LPCVOID)value, sizeof(value), NULL);
	}
}