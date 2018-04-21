#include "drivers/max14662.h"
#include "drivers/i2c.h"

#define MAX14662_ADDRESS_0_0 (0b1001100)
#define MAX14662_ADDRESS_0_1 (0b1001101)
#define MAX14662_ADDRESS_1_0 (0b1001110)
#define MAX14662_ADDRESS_1_1 (0b1001111)

#define I2C_TIMEOUT 100

static uint8_t max14662_state[NUM_OF_MAX14662_ADDRESSES];

static uint8_t resolve_address(enum MAX14662_address address) {
	switch (address) {
	default:
	case MAX14662_AD_0_0:
		return MAX14662_ADDRESS_0_0;
	case MAX14662_AD_0_1:
		return MAX14662_ADDRESS_0_1;
	case MAX14662_AD_1_0:
		return MAX14662_ADDRESS_1_0;
	case MAX14662_AD_1_1:
		return MAX14662_ADDRESS_1_1;
	}
}

HAL_StatusTypeDef max14662_set_value(enum MAX14662_address address, uint8_t val)
{
	HAL_StatusTypeDef status;
	uint8_t i2c_address;

	if (max14662_state[address] == val)
		return HAL_OK;

	i2c_address = resolve_address(address);

	status = i2c_master_tx(i2c_address << 1, &val, sizeof(val), I2C_TIMEOUT);

	if (status != HAL_OK)
		return status;

	max14662_state[address] = val;

	return status;
}

uint8_t max14662_get_value_cached(enum MAX14662_address address)
{
	const uint8_t i2c_address = resolve_address(address);

	return max14662_state[address];
}

HAL_StatusTypeDef max14662_get_value(enum MAX14662_address address, uint8_t *p_val)
{
	HAL_StatusTypeDef status;
	const uint8_t i2c_address = resolve_address(address);

	status = i2c_master_rx(i2c_address << 1, p_val, sizeof(p_val[0]), I2C_TIMEOUT);
	if (status != HAL_OK)
		return status;
	
	return status;
}
