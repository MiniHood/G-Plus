#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>

class Client;

using namespace std;
class Steam {
private:
	HANDLE StartSteamApplication(string ipc_name = "default_channel");
	bool HasUpdate();
	void ForceUpdate();
public:
	//HANDLE StartSteam(Client client);
	bool StopSteam();
	HANDLE Process;
};