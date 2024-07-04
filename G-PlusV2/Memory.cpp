#include "Memory.h"
#include "iostream"
#include <TlHelp32.h>

static HWND Memory::get_process_wnd(uint32_t pid)
{
		std::pair<HWND, uint32_t> params = { 0, pid };
		BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
			auto pParams = (std::pair<HWND, uint32_t>*)(lParam);
			uint32_t processId = 0;

			if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processId)) && processId == pParams->second) {
				SetLastError((uint32_t)-1);
				pParams->first = hwnd;
				return FALSE;
			}

			return TRUE;

			}, (LPARAM)&params);

		if (!bResult && GetLastError() == -1 && params.first)
			return params.first;

		return NULL;
}

uintptr_t Memory::get_base_address(int pid, std::string module)
{
	std::wstring pname = std::wstring(module.begin(), module.end());
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;
	MODULEENTRY32 ent;
	ent.dwSize = sizeof(ent);
	if (Module32First(hSnap, &ent))
	{
		do
		{
			if (!_stricmp(ent.szModule, (const char*)pname.c_str()))
			{
				CloseHandle(hSnap);
				return (uintptr_t)ent.modBaseAddr;
			}
		} while (Module32Next(hSnap, &ent));
	}
	CloseHandle(hSnap);
	return 0;
}

// Ported from https://github.com/deplantis/CPP-string-memory-scanner/tree/main/src/string%20memory%20scanner/string%20memory%20scanner/scanner
static char* current_memory_page = 0;
static SYSTEM_INFO sys_info;
static MEMORY_BASIC_INFORMATION info;

std::vector<uintptr_t> Memory::ScanForString(HANDLE hProcess, std::string& stringtofind)
{
	GetSystemInfo(&sys_info);
	std::vector<uintptr_t> return_address;
	while (current_memory_page < sys_info.lpMaximumApplicationAddress)
	{
		Memory::NtQueryVirtualMemory(hProcess, current_memory_page, Memory::MemoryBasicInformation, &info, sizeof(info), 0);

		if (info.State == MEM_COMMIT)
		{
			if (info.Protect == PAGE_READWRITE)
			{
				std::string buffer;
				buffer.resize(info.RegionSize + info.RegionSize / 2); // so the buffer don"t overflow

				Memory::ZwReadVirtualMemory(hProcess, current_memory_page, &buffer.at(0), info.RegionSize, 0);

				for (int begin = 0; begin < info.RegionSize; begin++)
				{
					if (buffer[begin] == stringtofind.at(0))
					{
						std::string stringbuffer;

						for (int copy = 0; copy < stringtofind.size(); copy++)
						{
							stringbuffer += buffer[begin + copy];
						}

						// find all strings that contain the word of the string that need to be searched
						/*if (stringtofind.find(stringbuffer) != std::string::npos)
						{
							addres.push_back((uintptr_t)current_memory_page + begin);
						}*/

						// find all strings THAT exactly and exactly look like the string that need to be searched
						if (stringtofind == stringbuffer)
						{
							return_address.push_back((uintptr_t)current_memory_page + begin);
						}
					}

				}
			}
		}

		current_memory_page += info.RegionSize;
	}

	current_memory_page = 0;

	return return_address;
}
