#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "Executor.h"

// Exported functions
extern "C" __declspec(dllexport) void __stdcall ExecuteModule(std::string fileName, std::string stringToRu) {
    Execute(fileName, stringToRu);
}