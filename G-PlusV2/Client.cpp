#include "Client.h"
using namespace std;

#pragma region Defining Private Class Functions
	bool Client::_MustSync()
	{
		// Check if gmod must sync
		return true;
	}

	bool Client::_ForceSync()
	{
		// Force sync through WPM
		return true;
	}

	bool Client::_IsConnected()
	{
		// Check if client is connected to a server.
		return false;
	}

	HANDLE Client::_StartGMOD() {
		// Start gmod and return handle of process
		return (HANDLE)1;
	}

	bool Client::_StopGMOD() {
		// Stop gmod and return true or false
		return true;
	}
#pragma endregion


#pragma region Defining Public Class Functions
	bool Client::Load() {
		HANDLE Steam = this->steam.StartSteam(this);
		if (Steam == 0)
		{
			cout << "Failed to load steam." << endl;
			return false;
		}

		this->steam.Process = Steam;
	}

	bool Client::Unload() {
		return true;
	}

	void Client::cleanup() {
		// TODO
	}
#pragma endregion