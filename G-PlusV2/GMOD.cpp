#include "GMOD.h"

string GMOD::GetServerConnection()
{
    // TODO: When free change this to signature scanning.
    // Known Bug: Value randomly changes in localhost games.
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

    char vstModule[] = "vstdlib.dll";

    // Status
    // ct - No connection, idling in menu;
    // r_url '' - Connecting to localhost
    // IP:Port - Connected/Connecting to server
    // Anything else: Connected to localhost

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

bool GMOD::IsConnected()
{

}

GMOD::LocalPlayer::Position GMOD::GetPlayerPosition()
{

}