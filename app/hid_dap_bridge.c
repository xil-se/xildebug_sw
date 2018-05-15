#include <FreeRTOS.h>
#include <DAP.h>
#include <stdbool.h>
#include <task.h>
#include <queue.h>

#include "drivers/max14662.h"
#include "drivers/usb/hid.h"
#include "hid_dap_bridge.h"

#define TASK_STACK_SIZE		512
#define TASK_NAME			"HIDdap"
#define TASK_PRIORITY		1

static struct {
	bool initialized;
	StackType_t task_stack[TASK_STACK_SIZE];
	TaskHandle_t task_handle;
	StaticTask_t task_tcb;
} self;

static void task_worker(void *p_arg)
{
	uint8_t resp_buf[USB_FS_MAX_PACKET_SIZE];
	struct usb_rx_queue_item recv_item;
	uint32_t sizes;
	err_t r;

	while (1) {
		r = usb_hid_recv(&recv_item, portMAX_DELAY);
		if (r != ERR_OK) {
			while (1)
				;
		}

		//printf("recv ");
		//for (int i = 0; i < recv_item.len; ++i)
		//	printf("0x%02X ", recv_item.data[i]);
		//printf("\r\n");

		// response in lower 16 bites, request in upper 16 bits
		sizes = DAP_ProcessCommand(recv_item.data, resp_buf);

		//printf("send ");
		//for (int i = 0; i < (sizes & 0xFFFF); ++i)
		//	printf("0x%02X ", resp_buf[i]);
		//printf("\r\n");

		r = usb_hid_send(resp_buf, sizes & 0xFFFF);
		if (r != ERR_OK) {
			while (1)
				;
		}
	}
}

err_t hid_dap_bridge_init(void)
{
	err_t r = ERR_OK;

	if (self.initialized)
		return ERR_OK;

	DAP_Setup();

	self.task_handle = xTaskCreateStatic(
		task_worker,
		TASK_NAME,
		TASK_STACK_SIZE,
		NULL,
		TASK_PRIORITY,
		&self.task_stack[0],
		&self.task_tcb);
	if (self.task_handle == NULL)
		return EHID_DAP_BRIDGE_TASK_CREATE;

	// TODO: enable only swd pins
	r = max14662_set_value(MAX14662_AD_0_0, 0xff);
	ERR_CHECK(r);

	self.initialized = true;
	ERR_CHECK(r);

	return r;
}
