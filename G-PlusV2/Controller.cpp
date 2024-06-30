#include "Controller.h"

static void Controller::AddNewClient(Client* client)
{
	if (Clients.contains(client)) {
		return;
	}

	Clients.insert(client);
	std::cout << Clients.size() << std::endl;
}

static void Controller::RemoveClient(Client* client)
{
	auto i = find(Clients.begin(), Clients.end(), client);
	Clients.erase(i);
}