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
