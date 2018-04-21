#include "drivers/mcp4018t.h"
#include "drivers/i2c.h"

#define MCP4018T_ADDRESS (0b0101111)
#define I2C_TIMEOUT 100

HAL_StatusTypeDef mcp4018t_set_value(uint8_t val)
{
	return i2c_master_tx(MCP4018T_ADDRESS << 1, &val, sizeof(val), I2C_TIMEOUT);
}
