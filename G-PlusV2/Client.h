#pragma once

#include <random>
#include <iostream>
#include "Windows.h"
#include "Server.h"
#include "GMOD.h"
#include "Steam.h"

using namespace std;

class Steam;

class Client {

private:
	string ipc_name;

	int random_number() {
		srand(time(NULL));
		return rand();
	}

	// Starting / Stopping
	HANDLE StartGMOD();
	bool StopGMOD();

	// Sync Game
	bool MustSync();
	bool ForceSync();

	// Getting in game variables.
	bool IsConnected();
public:
	string username;
	string password;
	Server server;
	Steam steam;
	GMOD gmod;

	Client(string client_username, string client_password, Server client_server, Steam client_steam, GMOD client_gmod, string ipc_name = "") {
		username = client_username;
		password = client_password;
		server = client_server;
		steam = client_steam;
		gmod = client_gmod;

		if (ipc_name.empty()) { ipc_name = random_number(); }
	}

	// TODO
	bool Load(); // Stop steam and gmod and clean up.
	bool Unload(); // Start steam and gmod, then do whatever we need to do next.

	// Close everything and clean up.
	void cleanup();
};
