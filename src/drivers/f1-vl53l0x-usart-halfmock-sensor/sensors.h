#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "vl53l0x_api.h"
#include "defines.h"

enum Sensors
{
    LOWER_OUTER_SENSOR = 0,
    LOWER_INNER_SENSOR = 1,
    UPPER_INNER_SENSOR = 2,
    UPPER_OUTER_SENSOR = 3
};

bool sensors_init();
bool sensors_IsCrossed(enum Sensors sensor);
void _Error_Handler(char *file, int line);