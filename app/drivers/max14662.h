#pragma once

#include "stm32l4xx_hal.h"
#include <stdbool.h>

enum MAX14662_address {
	MAX14662_AD_0_0 = 0,
	MAX14662_AD_0_1,
	MAX14662_AD_1_0,
	MAX14662_AD_1_1,
	NUM_OF_MAX14662_ADDRESSES,
};

HAL_StatusTypeDef max14662_set_value(enum MAX14662_address address, uint8_t val);
HAL_StatusTypeDef max14662_set_bit(enum MAX14662_address address, uint8_t bit, bool value);
uint8_t max14662_get_value_cached(enum MAX14662_address address);
HAL_StatusTypeDef max14662_get_value(enum MAX14662_address address, uint8_t *p_val);
HAL_StatusTypeDef max14662_init(enum MAX14662_address address);
