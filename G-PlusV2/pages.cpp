#pragma once

#include "pages.h"
#include "Controller.h"
using namespace std;


    #pragma region Settings Functions
        
    #pragma endregion

    #pragma region Client Functions
    void pages::client::mainmenu() {
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

    void pages::client::client_options(Client* client) 
    {
        return;
    }

    void pages::client::list_active_clients() {
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
        // TODO
    }

    void pages::client::add_new_client() {
        // TODO
    }

    void pages::client::teminate_all_clients() {
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
    
   