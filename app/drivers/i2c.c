#include "drivers/gpio.h"
#include "drivers/i2c.h"

I2C_HandleTypeDef i2c_handle;

HAL_StatusTypeDef i2c_init(void)
{
	HAL_StatusTypeDef status;

	i2c_handle.Instance = I2C1;
	i2c_handle.Init.Timing = 0x10909EEE;
	i2c_handle.Init.OwnAddress1 = 0;
	i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle.Init.OwnAddress2 = 0;
	i2c_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	status = HAL_I2C_Init(&i2c_handle);
	if (status != HAL_OK)
		return status;

	status = HAL_I2CEx_ConfigAnalogFilter(&i2c_handle, I2C_ANALOGFILTER_ENABLE);
	if (status != HAL_OK)
		return status;

	status = HAL_I2CEx_ConfigDigitalFilter(&i2c_handle, 0);
	if (status != HAL_OK)
		return status;

	return status;
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *p_handle)
{
	GPIO_InitTypeDef gpio_config;

	if(p_handle->Instance == I2C1) {
		gpio_config.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		gpio_config.Mode = GPIO_MODE_AF_OD;
		gpio_config.Pull = GPIO_NOPULL;
		gpio_config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		gpio_config.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOA, &gpio_config);

		__HAL_RCC_I2C1_CLK_ENABLE();
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *p_handle)
{
	if(p_handle->Instance == I2C1) {
		__HAL_RCC_I2C1_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
	}
}

