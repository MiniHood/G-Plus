#pragma once

#include "pages.h"

using namespace std;



    #pragma region Client Functions
    void pages::client::mainmenu() {
        cout << "[1] List active clients." << endl;
        cout << "[2] Search client by username." << endl;
        cout << "[3] Add new client." << endl;

        cout << endl << "[000] Terminate all clients." << endl;
    }

    void pages::client::list_active_clients() {
        // TODO
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
        cout << "[3] Plugin Editor" << endl;

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
    
   