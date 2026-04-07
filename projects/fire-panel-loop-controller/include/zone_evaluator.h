#ifndef ZONE_EVALUATOR_H
#define ZONE_EVALUATOR_H

#include "fire_panel_types.h"

typedef struct {
    panel_alarm_t alarm;
    panel_trouble_t trouble;
} zone_evaluation_t;

zone_evaluation_t zone_evaluator_eval(const fire_panel_input_t *input);

#endif
