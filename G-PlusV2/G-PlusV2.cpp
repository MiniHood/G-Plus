// G-PlusV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void typewrite(string input)
{
    for (const auto c : input) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	// Display Credits
    typewrite("This tool is free, if you paid for this you were scammed. Created by minihood.\nGithub: https://github.com/MiniHood/G-Plus/tree/v2");
}
