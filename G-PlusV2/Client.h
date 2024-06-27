#pragma once

#include <iostream>
#include "Windows.h"
#include "Server.h"
#include "Steam.h"
#include "GMOD.h"

using namespace std;

class Client {
public:
	string username;
	string password;
	Server server() const { return _server; }
	Steam steam() const { return _steam; }
	GMOD gmod() const { return _gmod; }
private:
	Server _server;
	Steam _steam;
	GMOD _gmod;
};