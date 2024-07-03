#pragma once
#ifndef PAGES_CONTROLLER
#define PAGES_CONTROLLER

#include <set>
#include "Client.h"
#include "Server.h"


namespace Controller {
	inline std::set<Client*> Clients;
	inline std::set<Server*> Servers;
	
	// if this doesnt work then we'll use OUT set<Client*> outPtr
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

	static void DeleteClient(Client* client)
	{
		client->gmod.Exit();
		client->steam.StopSteam();
		client->cleanup();
		RemoveClient(client);
	}

	static void AddNewServer(Server* server)
	{
		if (Servers.contains(server)) {
			return;
		}

		Servers.insert(server);
	}

	static void RemoveServer(Server* server)
	{
		auto i = find(Servers.begin(), Servers.end(), server);
		Servers.erase(i);
	}

	static void DeleteServer(Server* server)
	{
		RemoveServer(server);
	}

	static void OnExitEvent()
	{
		// Go through all clients and shut down
		for (Client* client : Clients)
		{
			client->gmod.Exit();
			client->steam.StopSteam();
			client->cleanup();
		}

		//for (Server* server : Servers) {
			//DeleteServer(server);
		//} This causes an exception for some reason, I'm not sure why?

		ZeroMemory(&Clients, 0);
		ZeroMemory(&Servers, 0);
	}
}

#endif