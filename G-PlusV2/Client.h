#pragma once

#include <random>
#include <iostream>
#include "Windows.h"
#include "Server.h"
#include "GMOD.h"
#include "Steam.h"

class Steam;

class Client {

private:
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
	std::string ipc_name;
	std::string username;
	std::string password;
	Server server;
	Steam steam;
	GMOD gmod;

	Client(std::string client_username, std::string client_password, Server client_server, Steam client_steam, GMOD client_gmod, std::string ipc_name = "") {
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
