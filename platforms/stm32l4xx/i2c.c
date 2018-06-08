#include <stdbool.h>

#include "hal_errors.h"
#include "platform/gpio.h"
#include "platform/i2c.h"

#define MODULE_NAME				I2C
#include "macros.h"

static struct {
	bool initialized;
	I2C_HandleTypeDef i2c_handle;
} SELF;

void HAL_I2C_MspInit(I2C_HandleTypeDef *p_handle)
{
	GPIO_InitTypeDef gpio_config;

	if(p_handle->Instance == I2C1) {
		gpio_config.Pin = GPIO_PIN_9 | GPIO_PIN_10;
		gpio_config.Mode = GPIO_MODE_AF_OD;
		gpio_config.Pull = GPIO_NOPULL;
		gpio_config.Speed = GPIO_SPEED_FREQ_LOW;
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

err_t i2c_master_tx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms)
{
	HAL_StatusTypeDef status;

	if (!SELF.initialized)
		return EI2C_NO_INIT;

	status = HAL_I2C_Master_Transmit(&SELF.i2c_handle, dev_address, p_data, size, timeout_ms);
	HAL_ERR_CHECK(status, EI2C_HAL_MASTER_TRANSMIT);

	return ERR_OK;
}

err_t i2c_master_rx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms)
{
	HAL_StatusTypeDef status;

	if (!SELF.initialized)
		return EI2C_NO_INIT;

	status = HAL_I2C_Master_Receive(&SELF.i2c_handle, dev_address, p_data, size, timeout_ms);
	HAL_ERR_CHECK(status, EI2C_HAL_MASTER_RECEIVE);

	return ERR_OK;
}

err_t i2c_init(void)
{
	HAL_StatusTypeDef status;

	if (SELF.initialized)
		return ERR_OK;

	SELF.i2c_handle.Instance = I2C1;
	SELF.i2c_handle.Init.Timing = 0x10909EEE;
	SELF.i2c_handle.Init.OwnAddress1 = 0;
	SELF.i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	SELF.i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	SELF.i2c_handle.Init.OwnAddress2 = 0;
	SELF.i2c_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	SELF.i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	SELF.i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	status = HAL_I2C_Init(&SELF.i2c_handle);
	HAL_ERR_CHECK(status, EI2C_HAL_INIT);

	status = HAL_I2CEx_ConfigAnalogFilter(&SELF.i2c_handle, I2C_ANALOGFILTER_ENABLE);
	HAL_ERR_CHECK(status, EI2C_HAL_CONFIG_ANALOG_FILTER);

	status = HAL_I2CEx_ConfigDigitalFilter(&SELF.i2c_handle, 0);
	HAL_ERR_CHECK(status, EI2C_HAL_CONFIG_DIGITAL_FILTER);

	SELF.initialized = true;

	return ERR_OK;
}
