#ifndef NAC_POLICY_H
#define NAC_POLICY_H

#include "fire_panel_types.h"

typedef struct {
    bool nac_active;
    bool buzzer_active;
    bool alarm_led;
    bool trouble_led;
} nac_policy_output_t;

nac_policy_output_t nac_policy_apply(panel_state_t state, panel_trouble_t trouble);

#endif
