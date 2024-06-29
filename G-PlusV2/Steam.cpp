#include "Steam.h"

HANDLE Steam::StartSteam(Client client) {
	if (this->HasUpdate() == true)
	{
		this->ForceUpdate();
		return;
	}

	return (HANDLE)1;
}

bool Steam::StopSteam() {
	// Private called by Load()
	// Stop steam and return true or false
	return true;
}

bool Steam::HasUpdate()
{
	// Check if steam must update
	return true;
}

void Steam::ForceUpdate()
{
	// Private, called by CheckForUpdates()
	// Force an update through WPM
	// Then call start steam again
	return;
}

HANDLE Steam::StartSteamApplication(string ipc_name) {
	// Private called by StartSteam()
}