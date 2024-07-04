// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Executor.h"
#include "Color.h"
#include "ICVar.h"
#include "CLuaShared.h"
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include "Hooking.h"
typedef __int64(__fastcall* RunStringEx)(PVOID _this, const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns);
RunStringEx oRunStringEx;
PVOID cLuaInterface = nullptr;
CCvar* cvarInterface = nullptr;
std::string cvarName = "";
std::string toRun = "";
std::string lastFileName = "";
bool __fastcall hkRunStringEx(PVOID _this,
#ifndef _WIN64
    void*,
#endif
    const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns)
{
    bool shouldCallRunStringEx = true;

    if (!strcmp(filename, "LuaCmd") || !strcmp(filename, "RunString(Ex)") || !strlen(filename)) {
        oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);
        return false; // Early return after calling oRunStringEx
    }

    lastFileName = std::string(filename);

    // Explanation of what this block does
    std::string toRun = "your_lua_script_here"; // Placeholder for actual Lua script
    oRunStringEx(_this, filename, path, toRun.c_str(), run, printErrors, dontPushErrors, noReturns);

    uintptr_t cvar = (uintptr_t)cvarInterface->FindVar(cvarName.c_str());
    if (!cvar) {
        shouldCallRunStringEx = false; // No need to call oRunStringEx again
    }
    else {
        std::string ip = CVarStr(cvar);
        Fix(ip); // Make IP address saveable by sanitizing characters

        if (ip.find(" - loopback") != std::string::npos) {
            shouldCallRunStringEx = false; // Condition met, no need to call oRunStringEx
        }
        else {
            std::string SavePath = "C:/GPlusV2/" + ip + "/" + std::string(filename);
            Sanitize(SavePath);
            StrToAscii(SavePath);

            std::string extension = ToLower(GetExtension(SavePath));
            if (extension == ".lua") {
                CreateDirectoryRec(SavePath);
                std::ofstream outfile(SavePath);
                outfile << stringToRun << std::endl;
            }
        }
    }

    // Final call to oRunStringEx based on conditions met earlier
    if (shouldCallRunStringEx) {
        oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);
    }
}


void Main()
{
    srand(time(nullptr));
    cvarName = RandomString(10);
    toRun = "CreateClientConVar(\"";
    toRun += cvarName;
    toRun += "\", \"\", true, false) :SetString(GetHostName() .. \" - \" ..game.GetIPAddress())";
    //fn = (MsgFn)GetProcAddress(GetModuleHandleW(L"tier0.dll"), ConColorMsg);

    #pragma region CVar Setup
    cvarInterface = (CCvar*)GetInterface("vstdlib.dll", "VEngineCvar007");
    if (cvarInterface)
    {
        std::cout << "Found CCVar: " << cvarInterface << std::endl;
    }
    #pragma endregion

#pragma region CLua Setup
        CLuaShared* LuaShared = (CLuaShared*)GetInterface("lua_shared.dll", "LUASHARED003");
    int counter = -1;
    do {
        counter++;
        if (counter == 1) std::cout << "Please, join a server..." << std::endl;
        cLuaInterface = LuaShared->GetLuaInterface(0);
        Sleep(5);
    } while (cLuaInterface == nullptr);
    std::cout << "Found CLuaInterface: " << std::hex << cLuaInterface << std::endl;
    #pragma endregion

    oRunStringEx = (RunStringEx)VMTHook((PVOID**)cLuaInterface, hkRunStringEx, 111);

    do {
        cLuaInterface = LuaShared->GetLuaInterface(0);
        while (cLuaInterface == nullptr)
        {
            cLuaInterface = LuaShared->GetLuaInterface(0);
            if (cLuaInterface != nullptr)
            {
                oRunStringEx = (RunStringEx)VMTHook((PVOID**)cLuaInterface, hkRunStringEx, 111);
                break;
            }
            Sleep(10);
        }
        Sleep(10);

    } while (true);
}

BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        std::thread(Main).detach();
    return TRUE;
}

