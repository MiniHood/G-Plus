#pragma once

#include <Windows.h>
#include <iostream>
#pragma comment(lib, "ntdll.lib")


class GMOD {
private:
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
    void Inject();

    int pID;
    void UpdatePositionStruct();
	HANDLE pHandle;
    std::string GetServerConnection();
    void Exit();
    HANDLE FindCorrectProcess(std::string& ipc_name);
private:
    LocalPlayer::Position GetPlayerPosition();
};