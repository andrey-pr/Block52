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
#ifdef DISCO_TESTBOARD_1_VL53
#include "boards/disco_testboard_1_vl53/disco_testboard_1_vl53_drivers.h"
#endif
#ifdef BLUEPILL_TESTBOARD
#include "boards/bluepill_testboard/bluepill_testboard_drivers.h"
#endif
#ifdef BLUEPILL_TESTBOARD_1_VL53
#include "boards/bluepill_testboard_1_vl53/bluepill_testboard_drivers_1_vl53.h"
#endif
#ifdef BLUEPILL_TESTBOARD_VL53
#include "boards/bluepill_testboard_vl53/bluepill_testboard_drivers_vl53.h"
#endif