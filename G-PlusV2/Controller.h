#pragma once
#ifndef PAGES_CONTROLLER
#define PAGES_CONTROLLER

#include <set>
#include "Client.h"
#include "Server.h"


namespace Controller {
	static std::set<Client*> Clients;
	static std::set<Server*> Servers;
	
	static void AddNewClient(Client* client) {
		if (Clients.contains(client)) {
			return;
		}

		Clients.insert(client);
	}

	static void RemoveClient(Client* client)
	{
		auto i = find(Clients.begin(), Clients.end(), client);
		Clients.erase(i);
	}
}

#endif