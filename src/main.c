#include "drivers.h"
#define SENSORS_TIME_DELTA 10000   //ms
#define LOWER_SENSORS_DISTANCE 100 //sm
#define UPPER_SENSORS_DISTANCE 100 //sm
#define STAIR_LENGTH 30            //sm
#define STAIRS 2
#define LEDS_ON_STAIR 1
#define LED_RED 128
#define LED_GREEN 0
#define LED_BLUE 255

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

QueueHandle_t qHandle;

void sensorsSurvey(void *pvParameters);
void ledControl(void *pvParameters);
void vPortSetupTimerInterrupt(void);
UART_HandleTypeDef usartHandle;

int main()
{
    hardware_init();
    led_init(STAIRS * LEDS_ON_STAIR);
    sensors_init();
    //////////TEST
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Set GPIOD Pin#15 Parameters */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    /* Init GPIOD Pin#15 */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_InitTypeDef GPIO_InitStruct2;

    MOCK_SENSOR_USART_GPIO_CLK_ENABLE();

    GPIO_InitStruct2.Pin = MOCK_SENSOR_USART_GPIO_PIN;
    GPIO_InitStruct2.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct2.Pull = GPIO_PULLUP;
    GPIO_InitStruct2.Alternate = MOCK_SENSOR_USART_GPIO_ALTERNATE;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct2);

    MOCK_SENSOR_USART_CLK_ENABLE();

    usartHandle.Instance = MOCK_SENSOR_USART;
    usartHandle.Init.BaudRate = MOCK_SENSOR_USART_BAUDRATE;
    usartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    usartHandle.Init.StopBits = UART_STOPBITS_1;
    usartHandle.Init.Parity = UART_PARITY_NONE;
    usartHandle.Init.Mode = UART_MODE_TX_RX;
    usartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    usartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    //////////TEST
    freertos_init();
    qHandle = xQueueCreate(3, sizeof(struct UserOnStairs));
    xTaskCreate(sensorsSurvey, "sensorsSurvey", 50, NULL, 1, NULL);
    xTaskCreate(ledControl, "ledControl", 50, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;)
    {
    }
}

void sensorsSurvey(void *pvParameters)
{
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
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
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
                user.switchDelay = 1000; //(xTaskGetTickCount() - lowerOuterSensorCrossTime / configTICK_RATE_HZ / 60) / LOWER_SENSORS_DISTANCE * STAIR_LENGTH;
                user.lastSwitchTime = 0;
                xQueueSend(qHandle, &user, 500);
                lowerOuterSensorCrossTime = -SENSORS_TIME_DELTA;
                HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
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
                user.switchDelay = 1000; //(xTaskGetTickCount() - upperOuterSensorCrossTime / configTICK_RATE_HZ / 60) / UPPER_SENSORS_DISTANCE * STAIR_LENGTH;
                user.lastSwitchTime = 0;
                xQueueSend(qHandle, &user, 500);
                upperOuterSensorCrossTime = -SENSORS_TIME_DELTA;
            }
            else
            {
                upperInnerSensorCrossTime = xTaskGetTickCount();
            }
        }
        vTaskDelay(100);
    }
}

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
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
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
                    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
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
                    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
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