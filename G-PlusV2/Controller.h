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
	static void AddNewClient(Client* client) {
		std::cout << "Added " << client->username << std::endl;
		if (Clients.contains(client)) {
			return;
		}

		Clients.insert(client);
		std::cout << Clients.size() << std::endl;
	}

	static void RemoveClient(Client* client)
	{
		auto i = find(Clients.begin(), Clients.end(), client);
		Clients.erase(i);
	}
}

#endif