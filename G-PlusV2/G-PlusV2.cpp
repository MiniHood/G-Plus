// G-PlusV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "Memory.h"
#include "Client.h"
#include "Controller.h"
#include "psapi.h"
#include <stdlib.h>  
#include "GUIh.h"
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
    atexit(Controller::OnExitEvent);

    #ifdef NDEBUG #else // Just skipping if I'm debugging, can't be bothered to wait
	// Display Credits
    string Warning = "This tool is free, if you paid for this you were scammed. \nGithub: https://github.com/MiniHood/G-Plus/tree/v2";
    typewrite(Warning);
    sleep(2000);
    clear_console();
    #endif

    // Testing starting steam
    for (size_t i = 0; i < 15; i++)
    {
        Server* server = new Server("DarkRPPPPP"-i, "1277.00.00.11"+i, "8000000"+i);
        AddNewServer(server);
        GMOD gmod{};
        Steam steam{};
        Client* client = new Client("Billy Bobby 12345679101112131415" + i, "Billy Bobby 12345679101112131415" + i, server, steam, gmod, "testClientasfafasfsaffas" + i);
        AddNewClient(client);
        cout << "Added " << client->username << endl;
        //client->steam.SetStartSteam(client);
    }

    // Init imgui
    GUI::StartGUI();

    /*
    // Workflow
    Server server;
    GMOD gmod{};
    Steam steam{};
    //Client* client = new Client("")

    system("pause");

    Controller::OnExitEvent();
    */
    //pages::client client;
    //client.mainmenu();
}