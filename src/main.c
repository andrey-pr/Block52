#include "drivers.h"

int main()
{
    hardware_init();
    led_init(1);
    sensors_init();
    GPIO_InitTypeDef GPIO_InitStruct;

	MOCK_SENSOR_USART_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = MOCK_SENSOR_USART_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = MOCK_SENSOR_USART_GPIO_ALTERNATE;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	MOCK_SENSOR_USART_CLK_ENABLE();

    UART_HandleTypeDef usartHandle;
	usartHandle.Instance = MOCK_SENSOR_USART;
	usartHandle.Init.BaudRate = MOCK_SENSOR_USART_BAUDRATE;
	usartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	usartHandle.Init.StopBits = UART_STOPBITS_1;
	usartHandle.Init.Parity = UART_PARITY_NONE;
	usartHandle.Init.Mode = UART_MODE_TX_RX;
	usartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	usartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&usartHandle) != HAL_OK)
	{
		return false;
	}
    while (true)
    {
        while (!led_isReady())
            ;
        if (sensors_IsCrossed(LOWER_OUTER_SENSOR))
        {
            led_setPixel(0, 128, 0, 64);
            //led_setPixel(1, 64, 0, 128);
        }
        led_pushFrameboofer();
        delay(100);
        while (!led_isReady())
            ;
        if (sensors_IsCrossed(LOWER_INNER_SENSOR))
        {
            led_setPixel(0, 64, 0, 128);
            //led_setPixel(1, 128, 0, 64);
        }
        led_pushFrameboofer();
        delay(100);
    }
}