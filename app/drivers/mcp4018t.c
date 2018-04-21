#include "drivers/mcp4018t.h"

static I2C_HandleTypeDef *p_i2c_handle;

#define MCP4018T_ADDRESS (0b0101111)
#define I2C_TIMEOUT 100

HAL_StatusTypeDef mcp4018t_set_value(uint8_t val)
{
	if (!p_i2c_handle)
		return HAL_ERROR;

	return HAL_I2C_Master_Transmit(p_i2c_handle, MCP4018T_ADDRESS << 1, &val, sizeof(val), I2C_TIMEOUT);
}

void mcp4018t_init(I2C_HandleTypeDef *p_handle)
{
	p_i2c_handle = p_handle;
}
