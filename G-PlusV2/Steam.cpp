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
	commandLineStream << "-master_ipc_name_override " << client->ipc_name << "-login " << client->username << " " << client->password;

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

void Steam::SetStartSteam(Client* client) {
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