#pragma once

#include "Windows.h"
#include <cstdint>
#include "string"
#include "vector"

namespace Memory {
	typedef enum _MEMORY_INFORMATION_CLASS
	{
		MemoryBasicInformation
	} MEMORY_INFORMATION_CLASS;

	extern "C" NTSYSCALLAPI NTSTATUS ZwReadVirtualMemory(
		HANDLE  hProcess,
		LPCVOID lpBaseAddress,
		LPVOID  lpBuffer,
		SIZE_T  nSize,
		SIZE_T * lpNumberOfBytesRead
	);

	extern "C" NTSYSCALLAPI NTSTATUS ZwWriteVirtualMemory(
		HANDLE  hProcess,
		LPVOID  lpBaseAddress,
		LPCVOID lpBuffer,
		SIZE_T  nSize,
		SIZE_T * lpNumberOfBytesWritten
	);


	extern "C" NTSYSCALLAPI NTSTATUS NtQueryVirtualMemory(
		HANDLE                   ProcessHandle,
		PVOID                    BaseAddress,
		MEMORY_INFORMATION_CLASS MemoryInformationClass,
		PVOID                    MemoryInformation,
		SIZE_T                   MemoryInformationLength,
		PSIZE_T                  ReturnLength
	);

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

	std::vector<uintptr_t> ScanForString(HANDLE hProcess, std::string& stringtofind);
}