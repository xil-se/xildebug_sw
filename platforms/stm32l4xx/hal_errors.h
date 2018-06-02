#pragma once

#include "errors.h"

#define HAL_ERROR_GET(x) (((x) >> 14) & 0x3)
#define HAL_ERROR_SET(hal_status, err_to_return) (err_to_return | ((hal_status) << 14))

#define HAL_ERR_CHECK(hal_status, err_to_return) \
	do { \
		if ((hal_status) != HAL_OK) \
			return HAL_ERROR_SET(err_to_return, hal_status); \
	} while (0)
