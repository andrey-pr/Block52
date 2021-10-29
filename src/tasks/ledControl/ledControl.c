#include "ledControl.h"

void ledControl(void *pvParameters)
{
    struct UserOnStairs *users = malloc(20 * sizeof(struct UserOnStairs));
    struct UserOnStairs buffer;
    int usersNumber = 0;
    for (int i = 0; i < 20; i++)
    {
        users[i].direction = UPSTAIRS;
        users[i].currentStair = STAIRS + 1;
    }
    while (true)
    {
        if (xQueueReceive(qHandle, &buffer, 10) != 0 && usersNumber < 20)
        {
            for (int i = 0; i <= usersNumber; i++)
            {
                if (i == usersNumber ||
                    (users[i].currentStair >= STAIRS && users[i].direction == UPSTAIRS) ||
                    (users[i].currentStair < 0 && users[i].direction == DOWNSTAIRS))
                {
                    users[i].direction = buffer.direction;
                    users[i].currentStair = buffer.currentStair;
                    users[i].switchDelay = buffer.switchDelay;
                    users[i].lastSwitchTime = xTaskGetTickCount();
                    if (i == usersNumber && i < 19)
                        usersNumber++;
                    break;
                }
            }
        }
        led_cleanFrameboofer();
        for (int i = 0; i < usersNumber; i++)
        {
            if ((users[i].currentStair < STAIRS && users[i].direction == UPSTAIRS) ||
                (users[i].currentStair >= 0 && users[i].direction == DOWNSTAIRS))
            {
                for (int a = 0; a < LEDS_ON_STAIR; a++)
                {
                    led_setPixel(LEDS_ON_STAIR * users[i].currentStair + a, LED_RED, LED_GREEN, LED_BLUE);
                }
                if (xTaskGetTickCount() - users[i].lastSwitchTime > users[i].switchDelay)
                {
                    users[i].currentStair += users[i].direction;
                    users[i].lastSwitchTime = xTaskGetTickCount();
                }
            }
        }
        while (!led_isReady())
            ;
        led_pushFrameboofer();
        vTaskDelay(100);
    }
}