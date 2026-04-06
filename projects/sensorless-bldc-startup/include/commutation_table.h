#ifndef COMMUTATION_TABLE_H
#define COMMUTATION_TABLE_H

#include <stdint.h>

#include "bldc_types.h"

bldc_sector_t commutation_next_sector(bldc_sector_t sector);
uint16_t commutation_period_from_rpm(uint16_t rpm);
uint16_t commutation_estimated_rpm(uint16_t period_us);

#endif
