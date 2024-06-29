#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Client;

using namespace std;
class Steam {
private:
	HANDLE StartSteamApplication(Client client, string ipc_name = "default_channel");
	bool HasUpdate();
	void ForceUpdate(Client client);
public:
	HANDLE StartSteam(Client client);
	bool StopSteam();
	HANDLE Process;
};