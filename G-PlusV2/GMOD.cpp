#include "GMOD.h"

string GMOD::GetServerConnection()
{
    // TODO: When free change this to signature scanning.
    
    char vstModule[] = "vstdlib.dll";
    // Status
    // CT = no connection in menu
    // NULL or url means in local game
    // In an actual game will show ip:port
    static DWORD CurrentConnectionAddress = 0x007DBE68;
    static vector<DWORD> CurrentConnectionOffsets{ 0x20, 0x10, 0x50, 0x10, 0x38, 0x48, 0xF8 };

    if (this->pHandle == NULL)
        return;

    DWORD BaseAddress = this->GetModuleBaseAddress(vstModule);
    DWORD ConnectionPointerAddress = GetPointerAddress(BaseAddress, CurrentConnectionAddress, CurrentConnectionOffsets);

    string ConnectionBuffer;
    BOOL ReadStatus = ReadProcessMemory(this->pHandle, (LPVOID*)ConnectionPointerAddress, &ConnectionBuffer, 20, NULL);

    if (ReadStatus == false)
    {
        cout << "Error reading current server connection. Error code: " << GetLastError() << endl;
        return;
    }
        
    return ConnectionBuffer;
}