#pragma once

#include "stm32l4xx_hal.h"

HAL_StatusTypeDef mcp4018t_set_value(uint8_t val);
void mcp4018t_init(I2C_HandleTypeDef *p_handle);