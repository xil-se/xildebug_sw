#include "stm32l4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "drivers/adc.h"
#include "drivers/gpio.h"
#include "drivers/mcp4018t.h"
#include "power.h"

/* TODO: Make sure these values match reality */
#define MIN_MV 1700
#define MAX_MV 4500

#define POWER_TASK_STACK_SIZE	512
#define POWER_TASK_NAME			"Power"
#define POWER_TASK_PRIORITY		1

#define QUEUE_LENGTH			10
#define QUEUE_ITEM_SIZE			(sizeof(uint16_t) * NUM_OF_ADC_CHANNELS)

static StackType_t power_task_stack[POWER_TASK_STACK_SIZE];
static TaskHandle_t power_task_handle;
static StaticTask_t power_task_tcb;
static QueueHandle_t power_queue_handle;
static StaticQueue_t power_queue;
static uint8_t power_queue_storage[QUEUE_LENGTH * QUEUE_ITEM_SIZE];

static bool m_initialized;
static bool m_dut_vdd_enabled;
static uint16_t m_ldo_voltage;

void power_dut_set_enabled(bool enabled)
{
	gpio_write(DUT_VDD_EN_GPIO_Port, DUT_VDD_EN_Pin, enabled);

	m_dut_vdd_enabled = enabled;
}

err_t power_dut_get_enabled(bool *p_enabled)
{
	if (!p_enabled)
		return EPOWER_INVALID_ARG;

	*p_enabled = m_dut_vdd_enabled;

	return ERR_OK;
}

err_t power_dut_ldo_set(uint32_t millivolt)
{
	err_t r;

	if (!m_initialized)
		return EPOWER_NO_INIT;

	if (millivolt < MIN_MV || millivolt > MAX_MV)
		return EPOWER_VOLTAGE_RANGE;

	const float factor = (((float)millivolt) - MIN_MV) / (MAX_MV - MIN_MV);
	const uint8_t value = (uint8_t) (factor * 127);

	r = mcp4018t_set_value(value);
	ERR_CHECK(r);

	m_ldo_voltage = millivolt;

	return r;
}

err_t power_dut_ldo_get(uint32_t *p_millivolt)
{
	if (!p_millivolt)
		return EPOWER_INVALID_ARG;

	*p_millivolt = m_ldo_voltage;

	return ERR_OK;
}

void adc_conversion_ready_handler(const uint16_t adc_values[NUM_OF_ADC_CHANNELS])
{
	BaseType_t higher_priority_task_woken = pdFALSE;

	xQueueSendFromISR(power_queue_handle, adc_values, &higher_priority_task_woken);
	portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void power_task(void *p_arg)
{
	uint16_t adc_values[NUM_OF_ADC_CHANNELS];

	for (;;) {
		xQueueReceive(power_queue_handle, adc_values, portMAX_DELAY);

		/* TODO: Do stuff with the values */
	}
}

err_t power_init(void)
{
	err_t r;

	power_task_handle = xTaskCreateStatic(
		power_task,
		POWER_TASK_NAME,
		POWER_TASK_STACK_SIZE,
		NULL,
		POWER_TASK_PRIORITY,
		&power_task_stack[0],
		&power_task_tcb);
	if (power_task_handle == NULL)
		return EPOWER_TASK_CREATE;

	power_queue_handle = xQueueCreateStatic(QUEUE_LENGTH, QUEUE_ITEM_SIZE, power_queue_storage, &power_queue);
	if (power_queue_handle == NULL)
		return EPOWER_QUEUE_CREATE;

	adc_set_callback(adc_conversion_ready_handler);
	r = adc_start();
	ERR_CHECK(r);

	m_initialized = true;

	return ERR_OK;
}
