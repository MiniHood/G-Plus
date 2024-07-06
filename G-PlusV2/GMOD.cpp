#include "GMOD.h"
#include "util.h"
#include "Memory.h"
#include "Globals.h"
#include <cassert>
std::string GMOD::GetServerConnection()
{
    // TODO: When free change this to signature scanning.
    // Known Bug: Value changes in localhost games.
    // Possible issues: Value may change randomly to uncoherint strings during online games.

    /*
        Some other behaviours are that when you have joined a localhost game and not moved, the value will change to ightenv "0"
        On top of this when moving in a localhost game the value will randomly change to one of the following values;
        - 9
        - 33
        - t 11
        - ht 14

        Another possible issue is that once you have tabbed out the value will default to NULL.

        When loading into a multiplayer game, the value will stay consistent as the serverip followed by the port. (IP:PORT)
        Yet to test being fully loaded into a multiplayer game, all of this applies to main menu or localhost.



        While this behaviour is weird it's predictible. 
    */


    if (this->pHandle == NULL)
        return {};

    char vstModule[] = "vstdlib.dll";

    // Status
    // ct - No connection, idling in menu;
    // r_url '' - Connecting to localhost
    // IP:Port - Connected/Connecting to server
    // Anything else: Connected to localhost

    static DWORD CurrentConnectionAddress = 0x007DBE68;
    static std::vector<DWORD> CurrentConnectionOffsets{ 0x20, 0x10, 0x50, 0x10, 0x38, 0x48, 0xF8 };

    DWORD BaseAddress = util::GetModuleBaseAddress(this->pHandle, vstModule);
    DWORD ConnectionPointerAddress = util::GetPointerAddress(this->pHandle, BaseAddress, CurrentConnectionAddress, CurrentConnectionOffsets);

    std::string ConnectionBuffer;
    BOOL ReadStatus = ReadProcessMemory(this->pHandle, (LPVOID*)ConnectionPointerAddress, &ConnectionBuffer, 20, NULL);

    if (ReadStatus == false)
    {
        std::cout << "Error reading current server connection. Error code: " << GetLastError() << std::endl;
        return {};
    }
        
    return ConnectionBuffer;
}

GMOD::LocalPlayer::Position GMOD::GetPlayerPosition()
{
    if (this->pHandle == NULL)
        return this->localPlayer.position;

    char clientModule[] = "client.dll";

    static DWORD XPositionAddress = 0x009DDDF8;
    static std::vector<DWORD> XPositionOffsets{ 50 };
    DWORD XBaseAddress = util::GetModuleBaseAddress(this->pHandle, clientModule);
    DWORD XPositionPointerAddress = util::GetPointerAddress(this->pHandle, XBaseAddress, XPositionAddress, XPositionOffsets);

    static DWORD YPositionAddress = 0x009DDDF8;
    static std::vector<DWORD> YPositionOffsets{ 50 }; // UPDATE 
    DWORD YBaseAddress = util::GetModuleBaseAddress(this->pHandle, clientModule); // UPDATE
    DWORD YPositionPointerAddress = util::GetPointerAddress(this->pHandle, YBaseAddress, YPositionAddress, YPositionOffsets);

    static DWORD ZPositionAddress = 0x009DDDF8; // UPDATE 
    static std::vector<DWORD> ZPositionOffsets{ 50 }; // UPDATE
    DWORD ZBaseAddress = util::GetModuleBaseAddress(this->pHandle, clientModule);
    DWORD ZPositionPointerAddress = util::GetPointerAddress(this->pHandle, ZBaseAddress, ZPositionAddress, ZPositionOffsets);

    float xBuffer;
    float yBuffer;
    float zBuffer;


    BOOL xReadStatus = ReadProcessMemory(this->pHandle, (unsigned long*)XPositionPointerAddress, &xBuffer, 4, NULL);
    BOOL yReadStatus = ReadProcessMemory(this->pHandle, (unsigned long*)YPositionPointerAddress, &yBuffer, 4, NULL);
    BOOL zReadStatus = ReadProcessMemory(this->pHandle, (unsigned long*)ZPositionPointerAddress, &zBuffer, 4, NULL);

    if (xReadStatus == false || yReadStatus == false || zReadStatus == false)
    {
        std::cout << "Error reading current player position. Error code: " << GetLastError() << std::endl;
        return this->localPlayer.position;
    }

    GMOD::LocalPlayer::Position vectorTable;
    vectorTable.x = xBuffer;
    vectorTable.y = yBuffer;
    vectorTable.z = zBuffer;

    return vectorTable;
}

void GMOD::UpdatePositionStruct()
{
    // We'll run this in a loop.
    GMOD::LocalPlayer::Position vectorTable = this->GetPlayerPosition();
    this->localPlayer.position.x = vectorTable.x;
    this->localPlayer.position.y = vectorTable.y;
    this->localPlayer.position.z = vectorTable.z;
}

void GMOD::Exit()
{
    TerminateProcess(this->pHandle, 0);
    CloseHandle(this->pHandle);
    // dont need to delete, exiting should clear the memory anyways
};

void GMOD::Inject(GMOD* gmod)
{
    DWORD gmodPID = gmod->pID;
    const char* path = "bin/HookModule.dll";

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gmodPID);
    if (hProc == INVALID_HANDLE_VALUE)
    {
        return;
    }

    LPVOID strAddy = VirtualAllocEx(hProc, NULL, strlen(path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (strAddy == NULL) {
        return;
    }
    if (!WriteProcessMemory(hProc, strAddy, path, strlen(path), NULL))
    {
        return;
    }

    LPVOID loadLibraryA = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    if (!loadLibraryA)
    {
        std::cout << "Congratulations, you've somehow deleted kernel32.dll. I'm not sure how the fuck your operating system is working right now." << std::endl;
        return;
    }
    HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryA, strAddy, NULL, NULL);

    if (hThread == INVALID_HANDLE_VALUE)
    {
        return;
    }

    CloseHandle(hProc);
    VirtualFreeEx(hProc, strAddy, 0, MEM_RELEASE);
}


HANDLE GMOD::FindCorrectProcess(std::string &ipc_name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot." << std::endl;
        return nullptr;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &pe32)) {
        CloseHandle(snapshot);
        std::cerr << "Failed to retrieve process list." << std::endl;
        return nullptr;
    }

    int Timer = Globals::GMOD::SearchTimeOut;



    do {
        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
        char buffer[MAX_PATH];
        if (hProcess != NULL && GetProcessInformation()) {
            std::vector<uintptr_t> addresses = Memory::ScanForString(hProcess, ipc_name);
            if (!addresses.empty()) {
                CloseHandle(snapshot);
                return hProcess; // Found the process containing the ipc_name
            }
            CloseHandle(hProcess); // Always close handles when done
        }
    } while (Process32Next(snapshot, &pe32));

    CloseHandle(snapshot);
    return nullptr; // No matching process found
}