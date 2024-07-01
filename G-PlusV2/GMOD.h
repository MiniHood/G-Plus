#pragma once

#include <Windows.h>
#include <iostream>


class GMOD {
private:
    void Close(); // todo

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


    void UpdatePositionStruct();
	HANDLE pHandle;
    std::string GetServerConnection();
    void Exit();
private:
    LocalPlayer::Position GetPlayerPosition();
};