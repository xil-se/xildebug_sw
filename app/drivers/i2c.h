#pragma once

#include "stm32l4xx_hal.h"

HAL_StatusTypeDef i2c_init(void);
HAL_StatusTypeDef i2c_master_tx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms);
HAL_StatusTypeDef i2c_master_rx(uint16_t dev_address, uint8_t *p_data, uint16_t size, uint32_t timeout_ms);
