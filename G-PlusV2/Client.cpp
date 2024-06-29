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

	bool Client::MustUpdate()
	{
		// Check if steam must update
		return true;
	}

	bool Client::ForceUpdate()
	{
		// Force an update through WPM
		return true;
	}

	bool Client::GetCurrentConnection()
	{
		// stored in engine.dll+7DC248
	}

	HANDLE Client::StartGMOD() {
		// Start gmod and return handle of process
		return (HANDLE)1;
	}

	bool Client::StopGMOD() {
		// Stop gmod and return true or false
		return true;
	}

	HANDLE Client::StartSteam() {
		// Start steam and return handle of process
		return (HANDLE)1;
	}

	bool Client::StopSteam() {
		// Stop steam and return true or false
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
		this->StopGMOD();
		this->StopSteam();
		
	}
#pragma endregion