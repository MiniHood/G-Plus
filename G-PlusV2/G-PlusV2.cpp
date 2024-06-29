// G-PlusV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "Memory.h"
#include "pages.h"

using namespace std;

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

    pages::home home;
    home.mainmenu();
}
