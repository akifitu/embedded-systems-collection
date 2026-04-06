#ifndef BMS_PORT_HAL_H
#define BMS_PORT_HAL_H

#include <stdbool.h>
#include <stdint.h>

#include "bms_controller.h"

void bms_port_hal_init(void);
bool bms_port_hal_read_sample(bms_sample_t *sample);
void bms_port_hal_apply_output(const bms_output_t *output);
uint32_t bms_port_hal_millis(void);
void bms_port_hal_log(const char *message);

#endif

