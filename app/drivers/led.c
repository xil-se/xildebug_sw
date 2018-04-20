#include "stm32l4xx_hal.h"

#include "drivers/led.h"
#include "drivers/gpio.h"

void led_rgb_set(uint8_t color)
{
	if (color & RGB_RED)
		HAL_GPIO_WritePin(LED_RGB_R_GPIO_Port, LED_RGB_R_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(LED_RGB_R_GPIO_Port, LED_RGB_R_Pin, GPIO_PIN_SET);

	if (color & RGB_GREEN)
		HAL_GPIO_WritePin(LED_RGB_G_GPIO_Port, LED_RGB_G_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(LED_RGB_G_GPIO_Port, LED_RGB_G_Pin, GPIO_PIN_SET);

	if (color & RGB_BLUE)
		HAL_GPIO_WritePin(LED_RGB_B_GPIO_Port, LED_RGB_B_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(LED_RGB_B_GPIO_Port, LED_RGB_B_Pin, GPIO_PIN_SET);
}

void led_tx_set(bool enabled)
{
	HAL_GPIO_WritePin(DUT_UART_TX_LED_GPIO_Port, DUT_UART_TX_LED_Pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_rx_set(bool enabled)
{
	HAL_GPIO_WritePin(DUT_UART_RX_LED_GPIO_Port, DUT_UART_RX_LED_Pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_swd_set(bool enabled)
{
	HAL_GPIO_WritePin(DUT_SWD_LED_GPIO_Port, DUT_SWD_LED_Pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_init(void)
{
	led_tx_set(false);
	led_rx_set(false);
	led_swd_set(false);
	led_rgb_set(0);
}
