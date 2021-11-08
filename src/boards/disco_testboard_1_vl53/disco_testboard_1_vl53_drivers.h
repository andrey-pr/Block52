#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#include "../../drivers/f4-ws2812/frontend.h"
#include "../../drivers/f4-startup/startup.h"
#include "../../drivers/f4-vl53l0x-usart-halfmock-sensor/sensors.h"
#include "../../drivers/f4-freertos/f4-freertos.h"