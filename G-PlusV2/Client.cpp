#include "Client.h"
using namespace std;

#pragma region Defining Private Class Functions
	bool Client::MustSync()
	{
		// Check if gmod must sync
		return true;
	}

	bool Client::ForceSync()
	{
		// Force sync through WPM
		return true;
	}

	bool Client::IsConnected()
	{
		// Check if client is connected to a server.
		return false;
	}

	HANDLE Client::StartGMOD() {
		// Start gmod and return handle of process
		return (HANDLE)1;
	}

	bool Client::StopGMOD() {
		// Stop gmod and return true or false
		return true;
	}
#pragma endregion


#pragma region Defining Public Class Functions
	bool Client::Load() {
		return true;
	}

	bool Client::Unload() {
		return true;
	}

	void Client::cleanup() {
		// TODO
	}
#pragma endregion