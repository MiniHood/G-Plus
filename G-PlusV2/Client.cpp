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
		const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, charset.size() - 1);

		std::string result;
		result.reserve(24);
		for (int i = 0; i < 24; ++i) {
			result += charset[distr(gen)];
		}
		return result;
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