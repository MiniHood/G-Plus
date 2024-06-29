#include "Steam.h"
#include "Globals.h"
#include "Client.h"



HANDLE Steam::StartSteam(Client client) {
	// Private called by Load()
	if (this->HasUpdate() == true)
	{
		// We'll just warn them that steam is updating and they should restart this client once steam has updated.
		// this->ForceUpdate();
		return client.steam.Process;
	}

	return (HANDLE)1;
}


bool Steam::StopSteam() {
	// Private called by Unload()
	// Stop steam and return true or false
	TerminateProcess(this->Process, 1);
	CloseHandle(this->Process);
	return true;
}

bool Steam::HasUpdate()
{
	string steam_client_win32 = Globals::Steam::Path.append("package\\steam_client_win32.manifest");
	ifstream File(steam_client_win32);
	string lineBuffer;

	if (!File.is_open())
		this->HasUpdate(); // Just keep retrying until we get it.

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

void Steam::ForceUpdate(Client client)
{
	// Private, called by CheckForUpdates()
	// Start steam then
	// Force an update through WPM
	// Then call start steam again

	// TOOD
	HANDLE Steam = this->StartSteamApplication(client);
	return;
}

HANDLE Steam::StartSteamApplication(Client client, string ipc_name) {
	// Private called by StartSteam()
	// TODO: Add steam guard support
	ostringstream Command;
	Command << "set VPROJECT=gplus && \"" <<  // Set project enviroment variable
		Globals::Steam::Path << // Give path to start
		"steam.exe \" " << // Add steam.exe to path
		"-master_ipc_name_override " << // Set steam to different shared memory
		ipc_name << // pass our ipc name
		" -login " << // Make sure steam logs us in
		client.username << // Pass our username
		" " << // Whitespace
		client.password; // Pass our password
	system(Command.str().c_str());
	return (HANDLE)0;
}