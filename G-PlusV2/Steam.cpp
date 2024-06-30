#include "Steam.h"
#include "Globals.h"
#include "Client.h"



HANDLE Steam::StartSteam(Client* client) {
	// Private called by Load()
	if (this->_HasUpdate() == true)
	{
		// We'll just warn them that steam is updating and they should restart this client once steam has updated.
		// Later on we'll implement forcing an update, once steam updates for me I'll get it done.
		MessageBoxA(GetConsoleWindow(), "WARNING", "The client you attempted to add requires a steam update. Please update steam then try again.", 0);
		// this->ForceUpdate();
		return client->steam.Process;
	}

	HANDLE SteamProcess = this->_StartSteamApplication(client, client->ipc_name);

	return (HANDLE)1;
}


bool Steam::StopSteam() {
	// Private called by Unload()
	// Stop steam and return true or false
	TerminateProcess(this->Process, 1);
	CloseHandle(this->Process);
	return true;
}

bool Steam::_HasUpdate()
{
	std::string path = Globals::Steam::Path;
	std::string steam_client_win32 = path.append("package\\steam_client_win32.manifest");
	std::ifstream File(steam_client_win32);
	std::string lineBuffer;

	if (!File.is_open())
		this->_HasUpdate(); // Just keep retrying until we get it.

	while (getline(File, lineBuffer))
	{
		if (lineBuffer.find("version") != std::string::npos) // If we're on the line with version then
		{
			// Check if the line contains our current version
			if (lineBuffer.find(Globals::Steam::Version) != std::string::npos)
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

PROCESS_INFORMATION launchExecutable(const std::string& programPath, const std::string& arguments) {
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(sInfo));
	sInfo.cb = sizeof(sInfo);
	ZeroMemory(&pInfo, sizeof(pInfo));

	// Convert strings to LPCTSTR format required by CreateProcess
	LPCTSTR lpApplicationName = programPath.c_str();
	LPCTSTR lpCommandLine = arguments.c_str();

	BOOL success = CreateProcess(lpApplicationName,   // Application name
	   (LPSTR)lpCommandLine,       // Command line arguments
		NULL,                // Process handle not inheritable
		NULL,                // Thread handle not inheritable
		FALSE,               // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,    // Create No Window flag
		NULL,                // Use parent's environment block
		NULL,                // Use parent's starting directory
		&sInfo,              // Pointer to STARTUPINFO structure
		&pInfo);             // Pointer to PROCESS_INFORMATION structure

	if (!success) {
		DWORD dwError = GetLastError();
		std::cout << "CreateProcess failed (" << dwError << ").\n";
		return pInfo;
	}
	else {
		return pInfo;
	}
}


HANDLE Steam::_StartSteamApplication(Client* client, std::string ipc_name) {
	// Private called by StartSteam()
	// TODO: Add steam guard support

	std::ostringstream cmdStream;
	cmdStream <<  // Set project enviroment variable
		Globals::Steam::Path << // Give path to start
		"steam.exe \" " << // Add steam.exe to path
		"-master_ipc_name_override " << // Set steam to different shared memory
		ipc_name << // pass our ipc name
		" -login " << // Make sure steam logs us in
		client->username << // Pass our username
		" " << // Whitespace
		client->password; // Pass our password

	PROCESS_INFORMATION cmd = launchExecutable("C:\WINDOWS\system32\cmd.exe", "set VPROJECT=gplus");

	std::ostringstream SteamString;
	SteamString << Globals::Steam::Path << "steam.exe";

	std::ostringstream SteamArguments;
	SteamArguments 
		<< "-master_ipc_name_override " << // Set steam to different shared memory
		ipc_name << // pass our ipc name
		" -login " << // Make sure steam logs us in
		client->username << // Pass our username
		" " << // Whitespace
		client->password; // Pass our password

	PROCESS_INFORMATION steam = launchExecutable(SteamString.str(), SteamArguments.str());

	CloseHandle(cmd.hProcess);
	CloseHandle(cmd.hThread);

	return steam.hProcess;
}