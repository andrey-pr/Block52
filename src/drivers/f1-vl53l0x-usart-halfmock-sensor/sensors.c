#include "sensors.h"

UART_HandleTypeDef *usartHandle;
I2C_HandleTypeDef *hi2c;
VL53L0X_Dev_t *device;
static void Prepare_GPIO();
static void Prepare_I2C();
static VL53L0X_Error initVL53(VL53L0X_Dev_t *device, I2C_HandleTypeDef *i2c, uint8_t new_addr);
static int getDistance(VL53L0X_Dev_t *device);
bool state[4] = {false, false, false, false};

bool sensors_init()
{
	usartHandle = malloc(sizeof(UART_HandleTypeDef));
	hi2c = malloc(sizeof(I2C_HandleTypeDef));
	device = malloc(sizeof(VL53L0X_Dev_t));

	Prepare_GPIO();
	Prepare_I2C();
	initVL53(device, hi2c, 0x48);
	GPIO_InitTypeDef GPIO_InitStruct;

	MOCK_SENSOR_USART_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = MOCK_SENSOR_USART_GPIO_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MOCK_SENSOR_USART_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOCK_SENSOR_USART_GPIO_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(MOCK_SENSOR_USART_GPIO_PORT, &GPIO_InitStruct);

	MOCK_SENSOR_USART_CLK_ENABLE();

	usartHandle->Instance = MOCK_SENSOR_USART;
	usartHandle->Init.BaudRate = MOCK_SENSOR_USART_BAUDRATE;
	usartHandle->Init.WordLength = UART_WORDLENGTH_8B;
	usartHandle->Init.StopBits = UART_STOPBITS_1;
	usartHandle->Init.Parity = UART_PARITY_NONE;
	usartHandle->Init.Mode = UART_MODE_TX_RX;
	usartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	usartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(usartHandle) != HAL_OK)
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
	if (sensor == LOWER_OUTER_SENSOR)
	{
		bool crossed = getDistance(device) < 500;
		rxData = 'a';
		if (crossed)
			HAL_UART_Transmit(usartHandle, &rxData, sizeof(rxData), 500);
		return crossed;
	}
	while (__HAL_UART_GET_FLAG(usartHandle, UART_FLAG_RXNE))
	{
		if (HAL_UART_Receive(usartHandle, &rxData, 1, 100) == HAL_OK)
		{
			HAL_UART_Transmit(usartHandle, &rxData, sizeof(rxData), 500);
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

static VL53L0X_Error initVL53(VL53L0X_Dev_t *device, I2C_HandleTypeDef *i2c, uint8_t new_addr)
{
	device->I2cHandle = i2c;
	device->I2cDevAddr = 0x52;
	device->Present = 0;
	device->Id = 0; //TODO
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	static uint32_t refSpadCount = 0;	//для процесса конфигурации датчиков
	static uint8_t isApertureSpads = 0; //для процесса конфигурации датчиков
	static uint8_t VhvSettings = 0;		//для процесса конфигурации датчиков
	static uint8_t PhaseCal = 0;		//для процесса конфигурации датчиков

	HAL_GPIO_WritePin(SENSORS_VL_XSHUT_GPIO_Port, SENSORS_VL_XSHUT_Pin, GPIO_PIN_RESET); //shut down the VL53L0X sensor.
	vTaskDelay(100);																	 //TODO								 //100

	HAL_GPIO_WritePin(SENSORS_VL_XSHUT_GPIO_Port, SENSORS_VL_XSHUT_Pin, GPIO_PIN_SET); //start up the sensor.
	vTaskDelay(100);
	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetDeviceAddress(device, new_addr);
		device->I2cDevAddr = new_addr;
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_DataInit(device);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_StaticInit(device);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_PerformRefSpadManagement(device, &refSpadCount, &isApertureSpads);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_PerformRefCalibration(device, &VhvSettings, &PhaseCal);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetReferenceSpads(device, refSpadCount, isApertureSpads);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetRefCalibration(device, VhvSettings, PhaseCal);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetLimitCheckEnable(device,
											 VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetLimitCheckEnable(device,
											 VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetLimitCheckValue(device,
											VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1 * 65536));
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetLimitCheckValue(device,
											VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60 * 65536));
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetDeviceMode(device, VL53L0X_DEVICEMODE_SINGLE_RANGING);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(device, 33000);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetVcselPulsePeriod(device,
											 VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_SetVcselPulsePeriod(device,
											 VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);
	}

	if (Status == VL53L0X_ERROR_NONE)
	{
		Status = VL53L0X_StartMeasurement(device);
	}

	return Status;
}

int getDistance(VL53L0X_Dev_t *device)
{
	VL53L0X_RangingMeasurementData_t result;
	VL53L0X_Error Status = VL53L0X_PerformSingleRangingMeasurement(device, &result);
	if (result.RangeStatus == 0)
	{
		result.RangeMilliMeter = result.RangeMilliMeter;
	}
	else
	{
		result.RangeMilliMeter = 9999;
		result.RangeStatus = VL53L0X_ERROR_RANGE_ERROR;
	}
	return result.RangeMilliMeter;
}

static void Prepare_I2C(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	SENSORS_VL_I2C_GPIO_RCC_Enable();
	GPIO_InitStruct.Pin = SENSORS_VL_I2C_GPIO_SCL;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SENSORS_VL_I2C_GPIO_SCL_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = SENSORS_VL_I2C_GPIO_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SENSORS_VL_I2C_GPIO_SDA_PORT, &GPIO_InitStruct);

	SENSORS_VL_I2C_RCC();
	hi2c->Instance = SENSORS_VL_I2C;
	hi2c->Init.ClockSpeed = 100000;
	hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c->Init.OwnAddress1 = 0;
	hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c->Init.OwnAddress2 = 0;
	hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(hi2c) != HAL_OK)
	{
	}
}

static void Prepare_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	SENSORS_VL_XSHUT_GPIO_RCC_Enable();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SENSORS_VL_XSHUT_GPIO_Port, SENSORS_VL_XSHUT_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : VL_XSHUT_Pin */
	GPIO_InitStruct.Pin = SENSORS_VL_XSHUT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SENSORS_VL_XSHUT_GPIO_Port, &GPIO_InitStruct);
}

void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}