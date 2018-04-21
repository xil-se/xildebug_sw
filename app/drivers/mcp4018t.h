#pragma once

#include "stm32l4xx_hal.h"

HAL_StatusTypeDef mcp4018t_set_value(uint8_t val);
HAL_StatusTypeDef mcp4018t_get_value(uint8_t *p_val);
HAL_StatusTypeDef mcp4018t_init(void);
