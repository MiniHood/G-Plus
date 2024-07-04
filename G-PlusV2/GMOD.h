#pragma once

#include <Windows.h>
#include <iostream>
#pragma comment(lib, "ntdll.lib")


class GMOD {
private:
    void Inject(GMOD* gmod);
public:
    struct LocalPlayer
    {
        struct Position
        {
            float x;
            float y;
            float z;
        } position;

        float health;
    } localPlayer;

    int pID;
    void UpdatePositionStruct();
	HANDLE pHandle;
    std::string GetServerConnection();
    void Exit();
    HANDLE FindCorrectProcess(std::string& ipc_name);
private:
    LocalPlayer::Position GetPlayerPosition();
};