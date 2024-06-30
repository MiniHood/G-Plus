#pragma once

#include "pages.h"
#include "Controller.h"
#include "util.h"

using namespace std;

    #pragma region Client Specific Functions
    void pages::client::client_options(Client* client)
    {
        ostringstream CurrentlyEditing;
        CurrentlyEditing << "You are currently editing " << client->username;
        ostringstream Options;
        Options
            << "[1] Load client." << endl
            << "[2] Unload client." << endl
            << "[3] Garry\'s Mod Control Options" << endl
            << "[4] Steam Control Options" << endl
            << endl;

        util::clear_console();
        util::CoutCentered(CurrentlyEditing.str());
        cout << Options.str() << endl;

        cout << "[!] ";
        int input;
        cin >> input;

        return;
    }
    #pragma endregion

    #pragma region Client Functions
    void pages::client::mainmenu() {
        util::clear_console();
        cout << "[1] List active clients." << endl;
        cout << "[2] Search client by username." << endl;
        cout << "[3] Add new client." << endl;

        cout << endl << "[000] Terminate all clients." << endl;

        cout << "[!] ";
        int input;
        cin >> input;

        switch (input)
        {
        case 1:
            this->list_active_clients();
            break;
        case 2:
            break;
        }
    }

    void pages::client::list_active_clients() {
        util::clear_console();

        int i = 0;
        map<int, Client*> selectionMap;
        for (Client* client : Controller::Clients)
        {
            selectionMap.insert({i, client});
            cout << "[" << i << "] " << client->username << endl;
            i++;
        }

        cout << "[!] ";
        int input;
        cin >> input;

        this->client_options(selectionMap[input]);
    }

    void pages::client::search_client_by_name() {
        util::clear_console();
        // TODO
    }

    void pages::client::add_new_client() {
        util::clear_console();
        // TODO
    }

    void pages::client::teminate_all_clients() {
        util::clear_console();
        // TODO
    }
    #pragma endregion

    #pragma region Server Functions
    void pages::server::mainmenu() {
        cout << "[1] List saved servers." << endl;
        cout << "[2] Search server by name." << endl;
        cout << "[3] Add new server." << endl;

        cout << endl << "[000] Delete all server records." << endl;
    }

    void pages::server::list_saved_servers() {
        // TODO
    }

    void pages::server::search_server_by_name() {
        // TODO
    }

    void pages::server::add_new_server() {
        // TODO
    }

    void pages::server::delete_server_records() {
        // TODO
    }
    #pragma endregion

    void pages::home::mainmenu() {
        util::clear_console();

        cout << "[1] Servers" << endl;
        cout << "[2] Clients" << endl;
        cout << "[3] Settings" << endl;

        cout << "[!] ";
        int input;
        cin >> input;

        switch (input) {
        case 1:
            pages::server server;
            server.mainmenu();
            break;
        case 2:
            pages::client client;
            client.mainmenu();
            break;
        case 3:
            break;

        default:
            break;
        }
    }
    
   