#include "zone_evaluator.h"

zone_evaluation_t zone_evaluator_eval(const fire_panel_input_t *input) {
    zone_evaluation_t evaluation;

    evaluation.alarm = PANEL_ALARM_NONE;
    evaluation.trouble = PANEL_TROUBLE_NONE;

    if (input->manual_pull) {
        evaluation.alarm = PANEL_ALARM_MANUAL_PULL;
    } else if (input->smoke_alarm) {
        evaluation.alarm = PANEL_ALARM_SMOKE;
    }

    if (input->loop_open) {
        evaluation.trouble = PANEL_TROUBLE_LOOP_OPEN;
    } else if (input->sensor_dirty) {
        evaluation.trouble = PANEL_TROUBLE_SENSOR_DIRTY;
    }

    return evaluation;
}
