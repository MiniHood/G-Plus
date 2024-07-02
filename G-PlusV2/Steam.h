#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class Client;

class Steam {
private:
	bool _HasUpdate();
	void _ForceUpdate(Client* client);
public:
	void SetStartSteam(Client* client);
	bool StopSteam();
	PROCESS_INFORMATION pInfo;
};