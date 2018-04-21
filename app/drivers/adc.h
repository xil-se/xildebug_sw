#pragma once

#include "stm32l4xx_hal.h"
#include "errors.h"

#define EADC_HAL_INIT			(ADC_BASE + 0)
#define EADC_HAL_CONFIG_CHANNEL	(ADC_BASE + 1)

err_t adc_init(void);
