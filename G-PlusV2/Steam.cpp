#include "Steam.h"
#include "Globals.h"
#include "Client.h"

using namespace std;

HANDLE Steam::StartSteam(Client* client) {
	// Private called by Load()
	//if (this->_HasUpdate() == true)
	//{
		// We'll just warn them that steam is updating and they should restart this client once steam has updated.
		// Later on we'll implement forcing an update, once steam updates for me I'll get it done.
		//MessageBoxA(GetConsoleWindow(), "WARNING", "The client you attempted to add requires a steam update. Please update steam then try again.", 0);
		// this->ForceUpdate();
		//return client->steam.Process;
	//}

	HANDLE SteamProcess = this->_StartSteamApplication(client, client->ipc_name);

	return SteamProcess;
}


bool Steam::StopSteam() {
	// Private called by Unload()
	// Stop steam and return true or false
	TerminateProcess(this->Process, 1);
	CloseHandle(this->Process);
	return true;
}

bool Steam::_HasUpdate() // Needs fixed
{
	string path = Globals::Steam::Path;
	string steam_client_win32 = path.append("package\\steam_client_win32.manifest");
	cout << steam_client_win32 << endl;
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
	HANDLE Steam = this->_StartSteamApplication(client);
	return;
}

#include <windows.h>
#include <iostream>

PROCESS_INFORMATION _Launch(const string& programPath, const string arguments) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi; // The function returns this
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	cout << programPath << endl;

	// Start the child process.
	if (!CreateProcessA(
		programPath.c_str(),      // app path
		(LPSTR)arguments.c_str(),     // Command line 
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0, // No window creation flag
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		cout << "Failed to create steam process" << endl;
		return;
	}
	else
	{
		return pi;
	}
}


HANDLE Steam::_StartSteamApplication(Client* client, string ipc_name) {
	// Private called by StartSteam()
	// TODO: Add steam guard support

	ostringstream cmdStream;
	ostringstream steamPath;
	steamPath << Globals::Steam::Path << "steam.exe";

	cmdStream <<  // Set project enviroment variable
		steamPath.str() << // Give path to start
		" " << // Add steam.exe to path
		"-master_ipc_name_override " << // Set steam to different shared memory
		ipc_name << // pass our ipc name
		"-login " << // Make sure steam logs us in
		client->username << // Pass our username
		" " << // Whitespace
		client->password; // Pass our password

	PROCESS_INFORMATION cmd = _Launch(steamPath.str(), cmdStream.str());

	// Make sure to close handle on the thread and handle later

	return cmd.hProcess;
}