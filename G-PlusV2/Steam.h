#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Client;

using namespace std;
class Steam {
private:
	HANDLE StartSteamApplication(string ipc_name = "default_channel", Client client);
	bool HasUpdate();
	void ForceUpdate();
public:
	HANDLE StartSteam(Client client);
	bool StopSteam();
	HANDLE Process;
};