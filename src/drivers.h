#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "string.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef DISCO_TESTBOARD
#include "boards/disco_testboard/disco_testboard_drivers.h"
#endif