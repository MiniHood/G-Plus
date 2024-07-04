#pragma once
#include <Windows.h>
#include <string>
#include "Color.h"
#include "CLuaShared.h"
#include "Utils.h"
#include "ICVar.h"
typedef __int64(__fastcall* RunStringEx)(PVOID _this, const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns);

extern RunStringEx oRunStringEx;
extern CCvar* cvarInterface;
extern std::string lastFileName;
static void Execute(std::string fileName, std::string stringToRun)
{
    CLuaShared* LuaShared = (CLuaShared*)GetInterface("lua_shared.dll", "LUASHARED003");
    PVOID cLuaInterface = LuaShared->GetLuaInterface(0);

    if (stringToRun != "print('Sorry, but I couldn't find this file')" && cLuaInterface && oRunStringEx)
    {
        try {
            oRunStringEx(cLuaInterface, lastFileName.c_str(), "", stringToRun.c_str(), true, true, true, true);
        }
        catch (...) {
        }
    }
}