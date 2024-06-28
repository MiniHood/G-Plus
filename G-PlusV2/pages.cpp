#pragma once

#include "pages.h"

using namespace std;


    void pages::client::mainmenu() {
        cout << "[1] List active clients." << endl;
        cout << "[2] Search client by username." << endl;
        cout << "[3] Add new client." << endl;

        cout << endl << "[000] Terminate all clients." << endl;
    }

    void pages::server::mainmenu() {
        cout << "[1] List saved servers." << endl;
        cout << "[2] Search server by name." << endl;
        cout << "[3] Add new server." << endl;
           
        cout << endl << "[000] Delete all server records." << endl;
    }

    void pages::home::mainmenu() {
        cout << "[1] Servers" << endl;
        cout << "[2] Clients" << endl;
    }
    
   