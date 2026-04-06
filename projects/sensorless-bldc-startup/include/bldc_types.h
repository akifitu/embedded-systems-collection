#ifndef BLDC_TYPES_H
#define BLDC_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BLDC_STATE_IDLE = 0,
    BLDC_STATE_ALIGN,
    BLDC_STATE_OPEN_LOOP,
    BLDC_STATE_CLOSED_LOOP,
    BLDC_STATE_FAULT
} bldc_state_t;

typedef enum {
    BLDC_SECTOR_S1 = 0,
    BLDC_SECTOR_S2,
    BLDC_SECTOR_S3,
    BLDC_SECTOR_S4,
    BLDC_SECTOR_S5,
    BLDC_SECTOR_S6
} bldc_sector_t;

typedef enum {
    BLDC_LOCK_NONE = 0,
    BLDC_LOCK_SEEKING,
    BLDC_LOCK_LOCKED,
    BLDC_LOCK_LOST
} bldc_lock_t;

enum {
    BLDC_FAULT_NONE = 0u,
    BLDC_FAULT_NO_LOCK = 1u << 0,
    BLDC_FAULT_OVERCURRENT = 1u << 1,
    BLDC_FAULT_STALL = 1u << 2
};

typedef struct {
    bool zero_cross;
    uint16_t phase_current_ma;
    uint16_t measured_rpm;
} bldc_observation_t;

typedef struct {
    bool start;
    bool clear_fault;
    uint8_t throttle_percent;
} bldc_command_t;

typedef struct {
    bldc_state_t state;
    bldc_sector_t sector;
    bldc_lock_t lock;
    uint8_t duty_percent;
    uint16_t commutation_period_us;
    uint16_t estimated_rpm;
    uint32_t faults;
} bldc_output_t;

const char *bldc_state_name(bldc_state_t state);
const char *bldc_sector_name(bldc_sector_t sector);
const char *bldc_lock_name(bldc_lock_t lock);
void bldc_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);

#endif
