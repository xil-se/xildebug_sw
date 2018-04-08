#pragma once

#include "stm32l4xx_hal.h"

UART_HandleTypeDef *uart_get_handle(void);
HAL_StatusTypeDef uart_init(void);
