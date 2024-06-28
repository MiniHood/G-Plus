#include "pages.h"
#include <iostream>

using namespace std;

class pages {
public:
    class client {
    public:
        void mainmenu() {
            cout << "[1] List active clients." << endl;
            cout << "[2] Search client by username." << endl;
            cout << "[3] Add new client." << endl;

            cout << "[000] Terminate all clients." << endl;
        }
    };
    class server {
    public:
        void mainmenu() {
            cout << "[1] List saved servers." << endl;
            cout << "[2] Search server by name." << endl;
            cout << "[3] Add new server." << endl;

            cout << "[000] Delete all server records." << endl;
        }
    };
    class home {
    public:
        void mainmenu() {
            cout << "[1] Servers" << endl;
            cout << "[2] Clients" << endl;
        }
    };
};