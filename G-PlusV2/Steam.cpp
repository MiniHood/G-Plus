#include "Steam.h"
#include "Globals.h"
#include "Client.h"
#include "util.h"
#include "Psapi.h"
#include <TlHelp32.h>
#include "thread"
#include "offsets.h"
#include "Winternl.h"
#include "ntstatus.h"
#include "Memory.h"
#include <sstream>
#include <locale>
#include <codecvt>

using namespace std;

bool Steam::StopSteam() {
	// Private called by Unload()
	// Stop steam and return true or false
	TerminateProcess(this->pInfo.hProcess, 1);
	CloseHandle(this->pInfo.hProcess);
	CloseHandle(this->pInfo.hThread);
	return true;
}

bool Steam::_HasUpdate() // Needs fixed
{
	string path = Globals::Steam::Path;
	string steam_client_win32 = path.append("package\\steam_client_win32.manifest");
	ifstream File(steam_client_win32);
	string lineBuffer;

	if (!File.is_open())
		this->_HasUpdate(); // Just keep retrying until we get it.

	while (getline(File, lineBuffer))
	{
		if (lineBuffer.find("version") != string::npos) // If we're on the line with version then
		{
			// Check if the line contains our current version
			if (lineBuffer.find(Globals::Steam::Version) != string::npos)
				return false;
		}
	}

	return true;
}

void Steam::_ForceUpdate(Client* client)
{
	// Private, called by CheckForUpdates()
	// Start steam then
	// Force an update through WPM
	// Then call start steam again

	// TOOD
	//HANDLE Steam = this->SetStartSteam(client);
	return;
}

PROCESS_INFORMATION Open_Steam_Process(Client* client)
{
	ostringstream commandLineStream;
	commandLineStream << "-master_ipc_name_override " << client->ipc_name;

	ostringstream SteamPath;
	SteamPath << Globals::Steam::Path << "steam.exe";
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(sInfo));
	sInfo.cb = sizeof(sInfo);
	ZeroMemory(&pInfo, sizeof(pInfo));

	std::ostringstream fullCommandLine;
	fullCommandLine << "set VPROJECT=" << client->ipc_name << "&&" << "\"" << SteamPath.str() << "\" " << commandLineStream.str();

	BOOL hProcess = CreateProcessA(
		SteamPath.str().c_str(),
		(LPSTR)fullCommandLine.str().c_str(),
		0,
		0,
		0,
		0,
		NULL, // I spent about 2 days trying to make an enviroment block, just to find out it doesn't actually matter. Fuck you steam.
		0,
		&sInfo,
		&pInfo
	);

	if (!hProcess) {
		return (PROCESS_INFORMATION)NULL;
	}

	// this took longer that it should've. it turns out i was doing it correct from when i first started...
	return pInfo;
}

#pragma region Failed attempts...
// This was an attempt at opening steam via command line arguments since VPROJECT wasn't setting via openprocess arugments
// The other is the same, this was an attempted recreation with the same result, you are not able to get steams process handle without starting it

/*DWORD_PTR Steam::get_steam_by_ipc_name(string ipc_name) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to create snapshot." << std::endl;
		return 0;
	}

	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	if (!Process32First(hSnapshot, &pe32)) {
		CloseHandle(hSnapshot);
		std::cerr << "Failed to get first process." << std::endl;
		return 0;
	}

	DWORD_PTR steamHandle = 0;
	do {
		// WHAT THE FUCK STEAM
		if (_stricmp(pe32.szExeFile, "steam.exe") == 0) {
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL) {
				SIZE_T bytesRead;
				DWORD_PTR value = 0;
				string Value = Memory::ReadProcMemory<string>(hProcess, (Memory::get_base_address(pe32.th32ProcessID, "steamclient.dll") + 0x1263F43));
				if (value == NULL)
				{
					cout << "Failed to read any memory for " << pe32.th32ProcessID << endl;
					continue;
				}

				cout << "Read " << Value << " from " << pe32.th32ProcessID << endl;
				if (Value.find(ipc_name)) {
					cout << "Found correct steam " << pe32.th32ProcessID << endl;
					steamHandle = pe32.th32ProcessID;
					CloseHandle(hProcess);
					break;
				}
				CloseHandle(hProcess);
			}
		}
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	return steamHandle;
}*/

// This was an attempt at the same
/*HANDLE Steam::get_steam_by_ipc_name(string ipc_name, vector<int> checkedList)
{
	// Loop through every steam labeled process
	// Read a pointer
	// If the pointer is the same as our ipc_name
	// Grab the handle of the process and return

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	cout << "Initlized variables." << endl;

	// Take snapshot
	cout << "Taking snapshot" << endl;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout << "Unable to create snapshot" << endl;
		return (HANDLE)NULL;
	}

	cout << "Getting first process info" << endl;
	// Get info on first process
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		cout << "Unable to get info" << endl;
		return (HANDLE)NULL;
	}

	bool found = false;

	// Loop through the processes
	do {
		cout << "Checking process " << pe32.szExeFile << endl;
		string pe32Str = (string)pe32.szExeFile;
		if (pe32Str.find("steam.exe") != string::npos)
		{
			if (find(checkedList.begin(), checkedList.end(), pe32.th32ProcessID) != checkedList.end())
				continue; // check if we're already checked this process

			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

			// test

			HWND window;
			window = FindWindowEx(NULL, NULL, "Steam", NULL);
			if (window != NULL)
			{
				HANDLE procHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
				if (procHandle)
				{
					HMODULE hMods[1024];
					DWORD cbNeeded;

					if (EnumProcessModules(procHandle, hMods, sizeof(hMods), &cbNeeded) && cbNeeded > 0)
					{
						void* faddress = (void*)(hMods[0] + 0x1263F43);
						char test[sizeof(ipc_name)];
						ReadProcessMemory(procHandle, (LPVOID)faddress, &test, sizeof(test), NULL);
						cout << "Read " << test << " from " << faddress << endl;
					}
					else { cout << "No process modules" << endl; }
				}
				else { cout << "Proc Handle is NULL" << endl; }
			}
			else { cout << "Window is null" << endl; };
			// end test
			if (hProcess == NULL)
			{
				cout << "Unable to open " << pe32.szExeFile << endl;
				continue;
			}
			else {
				found = true;
				cout << "Retrieved steam.exe, process ID: " << pe32.th32ProcessID << endl;
				checkedList.insert(checkedList.end(), pe32.th32ProcessID);
				break;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

		wstring pNameConversion = wstring(pNameConversion.begin(), pNameConversion.end());
		const wchar_t* pName = pNameConversion.c_str();

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (hSnap == INVALID_HANDLE_VALUE)
			return 0;
		MODULEENTRY32 ent;
		ent.dwSize = sizeof(ent);
		if (Module32First(hSnap, &ent))
		{
			do
			{
				if (!_wcsicmp(ent.szModule, pName))
				{
					std::wcout << "Build :" << ent.szModule << std::endl;
					CloseHandle(hSnap);
					return (uintptr_t)ent.modBaseAddr;
				}
			} while (Module32Next(hSnap, &ent));
		}
		CloseHandle(hSnap);
		return 0;


	CloseHandle(hProcessSnap);

	if (!found)
	{
		get_steam_by_ipc_name(ipc_name);
	}

	// Read memory for IPC name
	string buffer;
	string fileNameBuffer;
	DWORD BaseAddress = offsets::steam::steamclient::BaseAddress; // util::GetModuleBaseAddress(hProcessSnap, (LPSTR)"steamclient.dll");
	DWORD Address = BaseAddress + 0x1263F43;
	cout << "Information" << endl << "Process ID: " << pe32.th32ProcessID << endl << "steamclient.dll Base Address: " << BaseAddress << endl << "Targetted Address: " << Address << endl << "Handle: " << hProcessSnap << endl;

	BOOL rpm_status = ReadProcessMemory(hProcessSnap, (LPCVOID)Address, &buffer, sizeof(buffer), nullptr);
	if (!rpm_status)
	{
		DWORD error = GetLastError();
		cout << "Could not read process memory at " << Address << ". Error: " << error << " retrying." << endl;
		get_steam_by_ipc_name(ipc_name); // Retry reading
	}

	cout << "Buffer: " << buffer << endl;

	if (buffer != ipc_name) {
		cout << "Buffer was not our IPC name, rechecking other processes" << endl;
		get_steam_by_ipc_name(ipc_name, checkedList);
	}


	return (HANDLE)NULL;
}*/
#pragma endregion

void Steam::SetStartSteam(Client* client) {
	// Private called by StartSteam()
	// TODO: Add steam guard support
	
	// Make sure we're have VPROJECT
	// idea loop through every steam process and read memory until we find the one with our IPC name
	// we're going to have to go back to calling through system
	
	//ostringstream cmdStm;

	//cmdStm << "set VPROJECT=" << ipc_name << " && \"" << Globals::Steam::Path << "steam.exe\" -master_ipc_name_override " << ipc_name;
	//cout << "Init variable" << endl;

	// Doing this because for some reason our thread stops when we call system
	//string cmdStr = cmdStm.str();
	//jthread cmdThread(&sys_call, ref(cmdStr));


	// Pass it off to get_steam_by_ipc_name to wait for the process and grab it's handle
	//this->get_steam_by_ipc_name(ipc_name);
	//cout << "Passed get_steam_by_ipc_name" << endl;

	// Private called by Load()
	//if (this->_HasUpdate() == true)
	//{
		// We'll just warn them that steam is updating and they should restart this client once steam has updated.
		// Later on we'll implement forcing an update, once steam updates for me I'll get it done.
		//MessageBoxA(GetConsoleWindow(), "WARNING", "The client you attempted to add requires a steam update. Please update steam then try again.", 0);
		// this->ForceUpdate();
		//return client->steam.Process;
	//}

	if (client->ipc_name.size() == 0)
		client->ipc_name = "default_channel";
	
	client->steam.pInfo = Open_Steam_Process(client);
	return;
}