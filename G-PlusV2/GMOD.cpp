#include "GMOD.h"

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
        return NULL;

    char vstModule[] = "vstdlib.dll";

    // Status
    // ct - No connection, idling in menu;
    // r_url '' - Connecting to localhost
    // IP:Port - Connected/Connecting to server
    // Anything else: Connected to localhost

    static DWORD CurrentConnectionAddress = 0x007DBE68;
    static std::vector<DWORD> CurrentConnectionOffsets{ 0x20, 0x10, 0x50, 0x10, 0x38, 0x48, 0xF8 };

    DWORD BaseAddress = this->_GetModuleBaseAddress(vstModule);
    DWORD ConnectionPointerAddress = _GetPointerAddress(BaseAddress, CurrentConnectionAddress, CurrentConnectionOffsets);

    std::string ConnectionBuffer;
    BOOL ReadStatus = ReadProcessMemory(this->pHandle, (LPVOID*)ConnectionPointerAddress, &ConnectionBuffer, 20, NULL);

    if (ReadStatus == false)
    {
        std::cout << "Error reading current server connection. Error code: " << GetLastError() << std::endl;
        return NULL;
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
    DWORD XBaseAddress = this->_GetModuleBaseAddress(clientModule);
    DWORD XPositionPointerAddress = _GetPointerAddress(XBaseAddress, XPositionAddress, XPositionOffsets);

    static DWORD YPositionAddress = 0x009DDDF8;
    static std::vector<DWORD> YPositionOffsets{ 50 }; // UPDATE 
    DWORD YBaseAddress = this->_GetModuleBaseAddress(clientModule); // UPDATE
    DWORD YPositionPointerAddress = _GetPointerAddress(YBaseAddress, YPositionAddress, YPositionOffsets);

    static DWORD ZPositionAddress = 0x009DDDF8; // UPDATE 
    static std::vector<DWORD> ZPositionOffsets{ 50 }; // UPDATE
    DWORD ZBaseAddress = this->_GetModuleBaseAddress(clientModule);
    DWORD ZPositionPointerAddress = _GetPointerAddress(ZBaseAddress, ZPositionAddress, ZPositionOffsets);

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
    // Close application
    delete this->pHandle;
};