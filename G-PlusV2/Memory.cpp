#include "Memory.h"

using namespace std;

// please never read this file again, this is a shit show.

variant<int, string, char> Q_WriteProcessMemory(variant<int, string, char> input, int process_id, LPVOID lpBaseAddress)
{
    HANDLE proc_handle = OpenProcess(PROCESS_VM_READ, false, process_id);

    if (proc_handle == 0)
        return 0;

    // Use buffer to hold the input data
    variant<int, string, char> buffer = input;

    // Write the data
    BOOL wpm_status = WriteProcessMemory(proc_handle, lpBaseAddress, &buffer, sizeof(buffer), NULL);

    CloseHandle(proc_handle);

    if (wpm_status == false)
        return false;

    // Verify we wrote correctly (might remove due to race events, also pretty sure this is done when we write anyways.)
    variant<int, string, char> rpm_output = Q_ReadProcessMemory(process_id, lpBaseAddress, sizeof(buffer));
    if (rpm_output != input)
        return false;

    return true;
}

// it wouldnt let me use LPCVOID or SIZE_T without erroring because of the type so
// that's why I'm using variant<LPCVOID> etc
variant<int, string, char> Q_ReadProcessMemory(int process_id, variant<LPCVOID> lpBaseAddress, variant<SIZE_T> size){
    HANDLE proc_handle = OpenProcess(PROCESS_VM_READ, false, process_id);

    if (proc_handle == 0)
        return false;

    variant<int, string, char> buffer;
    BOOL rpm_status = ReadProcessMemory(proc_handle, get<LPCVOID>(lpBaseAddress), &buffer, get<SIZE_T>(size), NULL);

    CloseHandle(proc_handle);

    if (rpm_status == FALSE)
        return false;

    return buffer;
}