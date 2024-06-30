// G-PlusV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "Memory.h"
#include "pages.h"
#include "Client.h"
#include "Controller.h"

using namespace std;
using namespace Controller;

void clear_console() { system("cls"); }

void sleep(int ms) { std::this_thread::sleep_for(chrono::milliseconds(ms)); }

void typewrite(string input)
{
    for (const auto c : input) {
        std::cout << c << flush;
        sleep(5);
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    #ifdef NDEBUG #else // Just skipping if I'm debugging, can't be bothered to wait
	// Display Credits
    string Warning = "This tool is free, if you paid for this you were scammed. \nGithub: https://github.com/MiniHood/G-Plus/tree/v2";
    typewrite(Warning);
    sleep(2000);
    clear_console();
    #endif

    // Testing list active clients
    for (size_t i = 0; i < 15; i++)
    {
        Server server;
        GMOD gmod{};
        Steam steam{};
        ostringstream test;
        test << "test" << i << endl;
        Client* client = new Client(test.str(), test.str(), server, steam, gmod, "default");
        AddNewClient(client);
        cout << "Added " << client->username << endl;
    }


    // Testing starting steam
    //client.steam.StartSteam(&client);
    //cout << "Starting steam" << endl;

    pages::home home;
    home.mainmenu();
}