#include "Client.h"
#include "Controller.h"
#include <algorithm>
#include <iterator>
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

	string Client::_GenerateRandString()
	{
		// this might cause a memory leak :)
		size_t Len = 24;
		string Buffer;
		mt19937 MT{ random_device{}() };
		uniform_int_distribution<char> distance{ 'a', 'z' };
		generate_n(back_inserter(Buffer), Len, [&]() { return distance(MT); });
		return Buffer;
	}
#pragma endregion


#pragma region Defining Public Class Functions
	bool Client::Load() {
		this->steam.SetStartSteam(this);

		if (this->steam.pInfo.hProcess == NULL)
			return false;

		return true;
	}

	bool Client::Unload() {
		return true;
	}

	void Client::cleanup() {
		// TODO
	}
#pragma endregion