#pragma once

#include <stdbool.h>
#include <stdint.h>

struct persistent {
	uint32_t reboot_to_bootloader;
};

extern struct persistent persistent_data;
