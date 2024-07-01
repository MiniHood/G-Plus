#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h>
#include <vector>
#include <stdlib.h>
#include <map>


class GMOD {
private:
    DWORD _GetModuleBaseAddress(TCHAR* lpszModuleName)
    {
        DWORD dwModuleBaseAddress = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(this->pHandle)); // make snapshot of all modules within process
        MODULEENTRY32 ModuleEntry32 = { 0 };
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
        {
            do {
                if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
                {
                    dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


        }
        CloseHandle(hSnapshot);
        return dwModuleBaseAddress;
    }

	DWORD _GetPointerAddress(DWORD gameBaseAddr, DWORD address, std::vector<DWORD> offsets)
	{
        DWORD pID = NULL; // Game process ID
        GetWindowThreadProcessId(FindWindowA(NULL, "Garry\'s Mod (x64)"), &pID);
        HANDLE phandle = NULL;
        phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
        if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

        DWORD offset_null = NULL;
        ReadProcessMemory(phandle, (unsigned long*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
        DWORD pointeraddress = offset_null; // the address we need
        for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
        {
            ReadProcessMemory(phandle, (unsigned long*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
        }
        return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
	}

    void Close(); // todo

public:
    struct LocalPlayer
    {
        struct Position
        {
            float x;
            float y;
            float z;
        } position;

        float health;
    } localPlayer;


    void UpdatePositionStruct();
	HANDLE pHandle;
    std::string GetServerConnection();
    void Exit();
private:
    LocalPlayer::Position GetPlayerPosition();
};