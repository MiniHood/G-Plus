#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include "Client.h"

using namespace std;
class Steam {
private:
	HANDLE StartSteamApplication(string ipc_name);
	bool HasUpdate();
	void ForceUpdate();
public:
	HANDLE StartSteam(Client client);
	bool StopSteam();
	HANDLE Process;
};