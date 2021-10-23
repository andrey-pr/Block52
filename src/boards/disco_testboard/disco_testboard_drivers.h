#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#include "../../drivers/f4-ws2812/frontend.h"
#include "../../drivers/f4-delay/delay.h"
#include "../../drivers/f4-startup/startup.h"
#include "../../drivers/f4-mock-usart-sensors/sensors.h"
#include "../../drivers/f4-freertos/f4-freertos.h"