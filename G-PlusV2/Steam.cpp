#include "Steam.h"
#include "Globals.h"

/*
HANDLE Steam::StartSteam(Client client) {
	// Private called by Load()
	if (this->HasUpdate() == true)
	{
		this->ForceUpdate();
		return client.steam.Process;
	}

	return (HANDLE)1;
}
*/

bool Steam::StopSteam() {
	// Private called by Unload()
	// Stop steam and return true or false
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

void Steam::ForceUpdate()
{
	// Private, called by CheckForUpdates()
	// Start steam then
	// Force an update through WPM
	// Then call start steam again

	HANDLE Steam = this->StartSteamApplication();


	return;
}

HANDLE Steam::StartSteamApplication(string ipc_name) {
	// Private called by StartSteam()

	return (HANDLE)0;
}