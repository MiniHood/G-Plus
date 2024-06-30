#pragma once

#include <set>
#include "Client.h"
#include "Server.h"

using namespace std;

namespace Controller {
	set<Client*> Clients;
	set<Server*> Servers;
	
	void AddNewClient(Client* client) {
		if (Clients.contains(client)) {
			return;
		}

		Clients.insert(client);
	}

	void RemoveClient(Client* client)
	{
		auto i = find(Clients.begin(), Clients.end(), client);
		Clients.erase(i);
	}
}
