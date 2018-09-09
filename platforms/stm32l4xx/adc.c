#include <stdbool.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

#include "hal_errors.h"
#include "platform/adc.h"
#include "platform/gpio.h"

#define ADC_TASK_STACK_SIZE	128
#define ADC_TASK_NAME			"ADC"
#define ADC_TASK_PRIORITY		1

static ADC_HandleTypeDef adc_handle;
static bool m_initialized;
static uint16_t m_adc_values[NUM_OF_ADC_CHANNELS];
static uint32_t m_adc_current_index;
static uint32_t m_errors_count;
static uint32_t m_last_error;
static adc_conversion_ready m_callback;
StackType_t task_stack[ADC_TASK_STACK_SIZE];
TaskHandle_t task_handle;
StaticTask_t task_tcb;
bool m_enabled;

/* This is the actual IRQ handler. Need to forward it to the HAL. */
/*void ADC1_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&adc_handle);
}*/

static void adc_task(void *p_arg)
{
	HAL_StatusTypeDef r;
	int vrefint;
	int i=0;
	vTaskDelay(pdMS_TO_TICKS(5000));
    HAL_ADC_Start(&adc_handle);
	while(!__HAL_ADC_GET_FLAG(&adc_handle, ADC_FLAG_EOC));
	vrefint = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(HAL_ADC_GetValue(&adc_handle), ADC_RESOLUTION_12B);
	HAL_ADC_Stop(&adc_handle);

	adc_handle.Init.NbrOfConversion = NUM_OF_ADC_CHANNELS;
	r = HAL_ADC_Init(&adc_handle);
	if (r != HAL_OK)
		printf("Error re-initializing ADC HAL 0x%08X", r);

	printf("VREFINT: %d\r\n", vrefint);

	while(true) {
		vTaskDelay(pdMS_TO_TICKS(1000));
		printf("power task\r\n");
		r = HAL_ADC_Start(&adc_handle);
		if (r != HAL_OK) {
			printf("[E] HAL_ADC_Start 0x%08X\r\n", r);
			continue;
		}
		do {
			while(!__HAL_ADC_GET_FLAG(&adc_handle, ADC_FLAG_EOC));
			uint32_t value = HAL_ADC_GetValue(&adc_handle);
			printf("Got value %d: %lu\r\n", i, __HAL_ADC_CALC_DATA_TO_VOLTAGE(vrefint, value, ADC_RESOLUTION_12B));
			i++;
		} while (!__HAL_ADC_GET_FLAG(&adc_handle, ADC_FLAG_EOS) || __HAL_ADC_GET_FLAG(&adc_handle, ADC_FLAG_EOC));
		__HAL_ADC_CLEAR_FLAG(&adc_handle, ADC_FLAG_EOS);
		i=0;
	}
}

err_t adc_init(void)
{
	HAL_StatusTypeDef status;

	adc_handle.Instance = ADC1;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;
	adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	adc_handle.Init.LowPowerAutoWait = DISABLE;
	adc_handle.Init.ContinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfConversion = 1;
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfDiscConversion = 0;
	adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc_handle.Init.DMAContinuousRequests = DISABLE;
	adc_handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_handle.Init.OversamplingMode = DISABLE;
	/*adc_handle.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_16;
	adc_handle.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_NONE;
	adc_handle.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
	adc_handle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_RESUMED_MODE;*/

	status = HAL_ADC_Init(&adc_handle);
	HAL_ERR_CHECK(status, EADC_HAL_INIT);

	status = HAL_ADCEx_Calibration_Start(&adc_handle, ADC_SINGLE_ENDED);
	HAL_ERR_CHECK(status, EADC_HAL_INIT);

	ADC_ChannelConfTypeDef adc_config = {
		.Channel = ADC_CHANNEL_VREFINT,
		.Rank = ADC_REGULAR_RANK_1,
		.SamplingTime = ADC_SAMPLETIME_640CYCLES_5,
		.SingleDiff = ADC_SINGLE_ENDED,
		.OffsetNumber = ADC_OFFSET_NONE,
		.Offset = 0,
	};

	status = HAL_ADC_ConfigChannel(&adc_handle, &adc_config);
	HAL_ERR_CHECK(status, EADC_HAL_CONFIG_CHANNEL);

	adc_config.Channel = ADC_CHANNEL_5;
	adc_config.Rank = ADC_REGULAR_RANK_2;
	status = HAL_ADC_ConfigChannel(&adc_handle, &adc_config);
	HAL_ERR_CHECK(status, EADC_HAL_CONFIG_CHANNEL);

	adc_config.Channel = ADC_CHANNEL_6;
	adc_config.Rank = ADC_REGULAR_RANK_3;
	status = HAL_ADC_ConfigChannel(&adc_handle, &adc_config);
	HAL_ERR_CHECK(status, EADC_HAL_CONFIG_CHANNEL);
	
	memset(m_adc_values, 0, sizeof(uint16_t) * 3);

	task_handle = xTaskCreateStatic(
		adc_task,
		ADC_TASK_NAME,
		ADC_TASK_STACK_SIZE,
		NULL,
		ADC_TASK_PRIORITY,
		&task_stack[0],
		&task_tcb);
	if (task_handle == NULL)
		return EADC_NO_INIT;

	m_initialized = true;

	return ERR_OK;
}

err_t adc_start(void)
{
	if (!m_initialized)
		return EADC_NO_INIT;

	//HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	//HAL_ADC_Start_IT(&adc_handle);
	m_enabled = true;

	return ERR_OK;
}

err_t adc_stop(void)
{
	if (!m_initialized)
		return EADC_NO_INIT;

	//HAL_ADC_Stop_IT(&adc_handle);
	//HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
	m_enabled = false;
	return ERR_OK;
}

/* Note that callback is called from interrupt context */
void adc_set_callback(adc_conversion_ready callback)
{
	m_callback = callback;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)) {
		m_adc_values[m_adc_current_index] = (uint16_t) HAL_ADC_GetValue(hadc);
		m_adc_current_index++;
		if (m_callback)
			m_callback(m_adc_values);
	}

	/* If End Of Sequence is set, then we have just read the last sample in the sequence. */
	if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOS)) {
		if (m_callback)
			m_callback(m_adc_values);
		m_adc_current_index = 0;
		memset(m_adc_values, 0, sizeof(uint16_t) * 3);
		HAL_ADC_Start_IT(&adc_handle);
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	static uint16_t values[3];
	m_errors_count++;
	m_last_error = hadc->ErrorCode;
	ADC_CLEAR_ERRORCODE(hadc);
	
	values[0] = m_last_error;
	values[1] = m_errors_count;
	values[2] = m_adc_current_index;
	
	if (m_callback)
		m_callback(values);
	//platform_force_hardfault();
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	GPIO_InitTypeDef gpio_config;

	if(adcHandle->Instance == ADC1) {
		__HAL_RCC_ADC_CLK_ENABLE();

		/*gpio_config.Pin = DUT_VDD_IN_Pin | DUT_VDD_OUT_Pin;
		gpio_config.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		gpio_config.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &gpio_config);*/
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
	if(adcHandle->Instance == ADC1) {
		__HAL_RCC_ADC_CLK_DISABLE();

	//	HAL_GPIO_DeInit(GPIOA, DUT_VDD_IN_Pin | DUT_VDD_OUT_Pin);
	}
}
