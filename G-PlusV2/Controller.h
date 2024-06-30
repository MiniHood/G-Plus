#pragma once
#ifndef PAGES_CONTROLLER
#define PAGES_CONTROLLER

#include <set>
#include "Client.h"
#include "Server.h"


namespace Controller {
	// dont bully me for using externs
	inline std::set<Client*> Clients;
	inline std::set<Server*> Servers;
	
	// if this doesnt work then we'll use OUT set<Client*> outPtr
	static void AddNewClient(Client* client);

	static void RemoveClient(Client* client);
}

#endif