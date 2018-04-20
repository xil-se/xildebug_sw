#include "stm32l4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#include "drivers/gpio.h"
#include "drivers/uart.h"
#include "drivers/i2c.h"
#include "drivers/adc.h"
#include "drivers/led.h"

#define MAIN_TASK_STACK_SIZE	512
#define MAIN_TASK_NAME			"Main"
#define MAIN_TASK_PRIORITY		1

StackType_t main_task_stack[MAIN_TASK_STACK_SIZE];
TaskHandle_t main_task_handle;
StaticTask_t main_task_tcb;

HAL_StatusTypeDef SystemClock_Config(void)
{
	HAL_StatusTypeDef status;

	RCC_OscInitTypeDef RCC_OscInitStruct = {
		.OscillatorType = RCC_OSCILLATORTYPE_MSI,
		.MSIState = RCC_MSI_ON,
		.MSICalibrationValue = 0,
		.MSIClockRange = RCC_MSIRANGE_6,
		.PLL.PLLState = RCC_PLL_ON,
		.PLL.PLLSource = RCC_PLLSOURCE_MSI,
		.PLL.PLLN = 40,
		.PLL.PLLP = RCC_PLLP_DIV7,
		.PLL.PLLQ = RCC_PLLQ_DIV2,
		.PLL.PLLR = RCC_PLLR_DIV2,
	};

	status = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (status != HAL_OK)
		return status;

	RCC_ClkInitTypeDef RCC_ClkInitStruct = {
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV1,
		.APB2CLKDivider = RCC_HCLK_DIV1,
	};

	status = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if (status != HAL_OK)
		return status;

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {
		.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_ADC,
		.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1,
		.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1,
		.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1,
		.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1,
		.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI,
		.PLLSAI1.PLLSAI1M = 1,
		.PLLSAI1.PLLSAI1N = 24,
		.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7,
		.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2,
		.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2,
		.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK | RCC_PLLSAI1_ADC1CLK,
	};

	status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	if (status != HAL_OK)
		return status;

	status = HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	if (status != HAL_OK)
		return status;

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

	return status;
}

void main_task(void *p_arg)
{
	int i = 0;

	HAL_UART_Transmit(uart_get_handle(), (uint8_t*)"Hello World", 11, HAL_MAX_DELAY);

	while (1) {
		i++;
		led_rgb_set(i % 8);
		led_tx_set(i % 2);
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

int main(void)
{
	HAL_StatusTypeDef status;

	status = HAL_Init();
	if (status != HAL_OK)
		while (1) ;

	status = SystemClock_Config();
	if (status != HAL_OK)
		while (1) ;

	gpio_init();

	status = i2c_init();
	if (status != HAL_OK)
		while (1) ;

	status = uart_init();
	if (status != HAL_OK)
		while (1) ;

	status = adc_init();
	if (status != HAL_OK)
		while (1) ;

	main_task_handle = xTaskCreateStatic(
		main_task,
		MAIN_TASK_NAME,
		MAIN_TASK_STACK_SIZE,
		NULL,
		MAIN_TASK_PRIORITY,
		&main_task_stack[0],
		&main_task_tcb);

	if (main_task_handle == NULL)
		while (1) ;

	vTaskStartScheduler();

	while (1) {

	}

	return 0;
}
