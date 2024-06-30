#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Client;

class Steam {
private:
	HANDLE StartSteamApplication(Client* client, std::string ipc_name = "default_channel");
	bool HasUpdate();
	void ForceUpdate(Client* client);
public:
	HANDLE StartSteam(Client* client);
	bool StopSteam();
	HANDLE Process;
};