#include "drivers/gpio.h"
#include "drivers/adc.h"

ADC_HandleTypeDef adc_handle;

err_t adc_init(void)
{
	HAL_StatusTypeDef status;

	adc_handle.Instance = ADC1;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc_handle.Init.LowPowerAutoWait = DISABLE;
	adc_handle.Init.ContinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfConversion = 1;
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfDiscConversion = 1;
	adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc_handle.Init.DMAContinuousRequests = DISABLE;
	adc_handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_handle.Init.OversamplingMode = ENABLE;
	adc_handle.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_2;
	adc_handle.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_NONE;
	adc_handle.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
	adc_handle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;

	status = HAL_ADC_Init(&adc_handle);
	HAL_ERR_CHECK(status, EADC_HAL_INIT);

	ADC_ChannelConfTypeDef adc_config = {
		.Channel = ADC_CHANNEL_5,
		.Rank = ADC_REGULAR_RANK_1,
		.SamplingTime = ADC_SAMPLETIME_2CYCLES_5,
		.SingleDiff = ADC_SINGLE_ENDED,
		.OffsetNumber = ADC_OFFSET_NONE,
		.Offset = 0,
	};

	status = HAL_ADC_ConfigChannel(&adc_handle, &adc_config);
	HAL_ERR_CHECK(status, EADC_HAL_CONFIG_CHANNEL);

	return ERR_OK;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	GPIO_InitTypeDef gpio_config;

	if(adcHandle->Instance == ADC1) {
		__HAL_RCC_ADC_CLK_ENABLE();

		gpio_config.Pin = DUT_VDD_IN_Pin | DUT_VDD_OUT_Pin;
		gpio_config.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		gpio_config.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &gpio_config);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
	if(adcHandle->Instance == ADC1) {
		__HAL_RCC_ADC_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, DUT_VDD_IN_Pin | DUT_VDD_OUT_Pin);
	}
}
