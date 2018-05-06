#pragma once

#include "errors.h"

#include <usbd_cdc.h>

#define EUSB_CDC_NO_INIT		(EUSB_CDC_BASE + 0)
#define EUSB_CDC_INVALID_ARG	(EUSB_CDC_BASE + 1)
#define EUSB_CDC_REG_CLASS		(EUSB_CDC_BASE + 2)
#define EUSB_CDC_REG_IF			(EUSB_CDC_BASE + 3)
#define EUSB_CDC_TRANSMIT		(EUSB_CDC_BASE + 4)
#define EUSB_CDC_BUSY			(EUSB_CDC_BASE + 5)
#define EUSB_CDC_RX_TIMEOUT		(EUSB_CDC_BASE + 6)

struct rx_queue_item {
	uint8_t len;
	uint8_t data[64];
} __packed;

err_t usb_cdc_rx(struct rx_queue_item *p_rx_queue_item, uint32_t timeout_ticks);
err_t usb_cdc_tx(uint8_t *p_buf, uint16_t len);
err_t usb_cdc_init(USBD_HandleTypeDef *p_usbd);
