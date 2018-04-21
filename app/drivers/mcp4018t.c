#include "drivers/mcp4018t.h"
#include "drivers/i2c.h"

#define MCP4018T_ADDRESS	0b0101111
#define I2C_TIMEOUT			100
#define SELF_TEST_VALUE		0b0101010
#define DEFAULT_VALUE		0x01
#define MAX_VALUE			0x7f

static uint8_t mcp4018t_value;

HAL_StatusTypeDef mcp4018t_set_value(uint8_t val)
{
	if (val > MAX_VALUE)
		return HAL_ERROR;

	if (mcp4018t_value == val)
		return HAL_OK;

	return i2c_master_tx(MCP4018T_ADDRESS << 1, &val, sizeof(val), I2C_TIMEOUT);
}

HAL_StatusTypeDef mcp4018t_get_value(uint8_t *p_val)
{
	return i2c_master_rx(MCP4018T_ADDRESS << 1, p_val, sizeof(p_val[0]), I2C_TIMEOUT);
}

HAL_StatusTypeDef mcp4018t_init(void)
{
	HAL_StatusTypeDef status;
	uint8_t value;

	status = mcp4018t_set_value(SELF_TEST_VALUE);
	if (status != HAL_OK)
		return status;

	status = mcp4018t_get_value(&value);
	if (status != HAL_OK)
		return status;

	if (value != SELF_TEST_VALUE)
		return HAL_ERROR;

	status = mcp4018t_set_value(DEFAULT_VALUE);
	if (status != HAL_OK)
		return status;

	mcp4018t_value = DEFAULT_VALUE;

	return HAL_OK;
}
