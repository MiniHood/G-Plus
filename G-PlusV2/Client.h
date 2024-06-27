#pragma once

#include <iostream>
#include "Windows.h"
#include "Server.h"
#include "Steam.h"
#include "GMOD.h"

using namespace std;

class Client {

private:
	Server _server;
	Steam _steam;
	GMOD _gmod;

public:
	string username;
	string password;
	Server server() const { return _server; }
	Steam steam() const { return _steam; }
	GMOD gmod() const { return _gmod; }

	Client(string client_username, string client_password, Server client_server, Steam client_steam, GMOD client_gmod) {
		username = client_username;
		password = client_password;
		_server = client_server;
		_steam = client_steam;
		_gmod = client_gmod;
	}
};