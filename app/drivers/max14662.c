#include "drivers/max14662.h"
#include "drivers/i2c.h"

#define MAX14662_ADDRESS_0_0	(0b1001100)
#define MAX14662_ADDRESS_0_1	(0b1001101)
#define MAX14662_ADDRESS_1_0	(0b1001110)
#define MAX14662_ADDRESS_1_1	(0b1001111)
#define MAX14662_REG_ADDR		(0x00)

#define I2C_TIMEOUT 100

#define SELF_TEST_VALUE_1	0b10101010
#define SELF_TEST_VALUE_2	0b01010101
#define DEFAULT_VALUE		0b00000000
#define MAX_VALUE			0b11111111

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
	uint8_t data[2] = {
		MAX14662_REG_ADDR,
		val
	};

	if (max14662_state[address] == val)
		return HAL_OK;

	i2c_address = resolve_address(address);
	status = i2c_master_tx(i2c_address << 1, data, sizeof(data), I2C_TIMEOUT);
	if (status != HAL_OK)
		return status;

	max14662_state[address] = val;

	return status;
}

HAL_StatusTypeDef max14662_set_bit(enum MAX14662_address address, uint8_t bit, bool value)
{
	const uint8_t current_value = max14662_state[address];
	const bool masked_bit = !!(current_value & (1 << bit));
	uint8_t new_value;

	if (masked_bit == value)
		return HAL_OK;

	if (value)
		new_value = max14662_state[address] | (1 << bit);
	else
		new_value = max14662_state[address] & (~(1 << bit));
	
	return max14662_set_value(address, new_value);
}

uint8_t max14662_get_value_cached(enum MAX14662_address address)
{
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

static HAL_StatusTypeDef max14662_set_and_verify(enum MAX14662_address address, uint8_t value)
{
	HAL_StatusTypeDef status;
	uint8_t actual_value;

	status = max14662_set_value(address, value);
	if (status != HAL_OK)
		return status;

	status = max14662_get_value(address, &actual_value);
	if (status != HAL_OK)
		return status;

	if (value != actual_value)
		return HAL_ERROR;

	return HAL_OK;
}

HAL_StatusTypeDef max14662_init(enum MAX14662_address address)
{
	HAL_StatusTypeDef status;

	status = max14662_set_and_verify(address, SELF_TEST_VALUE_1);
	if (status != HAL_OK)
		return status;

	status = max14662_set_and_verify(address, SELF_TEST_VALUE_2);
	if (status != HAL_OK)
		return status;

	status = max14662_set_value(address, DEFAULT_VALUE);
	if (status != HAL_OK)
		return status;

	return HAL_OK;
}
