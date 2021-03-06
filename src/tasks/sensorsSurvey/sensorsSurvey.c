#include "sensorsSurvey.h"

void sensorsSurvey(void *pvParameters)
{
    sensors_init();
    TickType_t lowerOuterSensorCrossTime = -SENSORS_TIME_DELTA,
               lowerInnerSensorCrossTime = -SENSORS_TIME_DELTA,
               upperInnerSensorCrossTime = -SENSORS_TIME_DELTA,
               upperOuterSensorCrossTime = -SENSORS_TIME_DELTA;
    struct UserOnStairs user;
    while (true)
    {
        bool lowerOuterSensorCrossed = sensors_IsCrossed(LOWER_OUTER_SENSOR),
             lowerInnerSensorCrossed = sensors_IsCrossed(LOWER_INNER_SENSOR),
             upperInnerSensorCrossed = sensors_IsCrossed(UPPER_INNER_SENSOR),
             upperOuterSensorCrossed = sensors_IsCrossed(UPPER_OUTER_SENSOR);
        if (lowerOuterSensorCrossed)
        {
            if (xTaskGetTickCount() - lowerInnerSensorCrossTime < SENSORS_TIME_DELTA / 1000 * configTICK_RATE_HZ)
            {
                lowerInnerSensorCrossTime = -SENSORS_TIME_DELTA;
            }
            else
            {
                lowerOuterSensorCrossTime = xTaskGetTickCount();
            }
        }
        if (upperOuterSensorCrossed)
        {
            if (xTaskGetTickCount() - upperInnerSensorCrossTime < SENSORS_TIME_DELTA / 1000 * configTICK_RATE_HZ)
            {
                upperInnerSensorCrossTime = -SENSORS_TIME_DELTA;
            }
            else
            {
                upperOuterSensorCrossTime = xTaskGetTickCount();
            }
        }
        if (lowerInnerSensorCrossed)
        {
            if (xTaskGetTickCount() - lowerOuterSensorCrossTime < SENSORS_TIME_DELTA / 1000 * configTICK_RATE_HZ)
            {
                user.direction = UPSTAIRS;
                user.currentStair = 0;
                user.switchDelay = (xTaskGetTickCount() - lowerOuterSensorCrossTime) * (STAIR_LENGTH / LOWER_SENSORS_DISTANCE);
                user.lastSwitchTime = 0;
                xQueueSend(qHandle, &user, 500);
                lowerOuterSensorCrossTime = -SENSORS_TIME_DELTA;
            }
            else
            {
                lowerInnerSensorCrossTime = xTaskGetTickCount();
            }
        }
        if (upperInnerSensorCrossed)
        {
            if (xTaskGetTickCount() - upperOuterSensorCrossTime < SENSORS_TIME_DELTA / 1000 * configTICK_RATE_HZ)
            {
                user.direction = DOWNSTAIRS;
                user.currentStair = STAIRS - 1;
                user.switchDelay = (xTaskGetTickCount() - upperOuterSensorCrossTime) * (STAIR_LENGTH / UPPER_SENSORS_DISTANCE);
                user.lastSwitchTime = 0;
                xQueueSend(qHandle, &user, 500);
                upperOuterSensorCrossTime = -SENSORS_TIME_DELTA;
            }
            else
            {
                upperInnerSensorCrossTime = xTaskGetTickCount();
            }
        }
        vTaskDelay(10);
    }
}