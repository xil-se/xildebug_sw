#pragma once

#include "stm32l4xx_hal.h"
#include "errors.h"

#define EI2C_HAL_INIT					(I2C_BASE + 0)
#define EI2C_HAL_CONFIG_ANALOG_FILTER	(I2C_BASE + 1)
#define EI2C_HAL_CONFIG_DIGITAL_FILTER	(I2C_BASE + 2)
#define EI2C_HAL_MASTER_TRANSMIT		(I2C_BASE + 3)
#define EI2C_HAL_MASTER_RECEIVE			(I2C_BASE + 4)

err_t i2c_init(void);
err_t i2c_master_tx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms);
err_t i2c_master_rx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms);
