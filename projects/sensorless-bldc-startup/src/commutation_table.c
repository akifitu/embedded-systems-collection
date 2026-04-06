#include "commutation_table.h"

enum {
    RPM_SCALE = 1500000
};

bldc_sector_t commutation_next_sector(bldc_sector_t sector) {
    switch (sector) {
        case BLDC_SECTOR_S1:
            return BLDC_SECTOR_S2;
        case BLDC_SECTOR_S2:
            return BLDC_SECTOR_S3;
        case BLDC_SECTOR_S3:
            return BLDC_SECTOR_S4;
        case BLDC_SECTOR_S4:
            return BLDC_SECTOR_S5;
        case BLDC_SECTOR_S5:
            return BLDC_SECTOR_S6;
        case BLDC_SECTOR_S6:
            return BLDC_SECTOR_S1;
    }

    return BLDC_SECTOR_S1;
}

uint16_t commutation_period_from_rpm(uint16_t rpm) {
    if (rpm == 0u) {
        return 0u;
    }

    return (uint16_t)(RPM_SCALE / rpm);
}

uint16_t commutation_estimated_rpm(uint16_t period_us) {
    if (period_us == 0u) {
        return 0u;
    }

    return (uint16_t)(RPM_SCALE / period_us);
}
