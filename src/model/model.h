#pragma once

#include "FreeRTOS.h"

enum Directions
{
    DOWNSTAIRS = -1,
    UPSTAIRS = 1
};
struct UserOnStairs
{
    enum Directions direction;
    int currentStair;
    int switchDelay;
    TickType_t lastSwitchTime;
};