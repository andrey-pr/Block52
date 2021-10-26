#include "sensors.h"

UART_HandleTypeDef usartHandle;
bool state[4] = {false, false, false, false};
bool sensors_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	MOCK_SENSOR_USART_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = MOCK_SENSOR_USART_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = MOCK_SENSOR_USART_GPIO_ALTERNATE;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	MOCK_SENSOR_USART_CLK_ENABLE();

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
	return true;
}

bool sensors_IsCrossed(enum Sensors sensor)
{
	if (state[sensor])
	{
		state[sensor] = false;
		return true;
	}
	uint8_t rxData;
	while (__HAL_UART_GET_FLAG(&usartHandle, UART_FLAG_RXNE))
	{
		if (HAL_UART_Receive(&usartHandle, &rxData, 1, 100) == HAL_OK)
		{
			HAL_UART_Transmit(&usartHandle, &rxData, sizeof(rxData), 500);
			if (rxData - 'a' >= 0 && rxData - 'a' <= 3)
			{
				if (rxData - 'a' == sensor)
				{
					return true;
				}
				else
				{
					state[rxData - 'a'] = true;
				}
			}
		}
	}
	return false;
}