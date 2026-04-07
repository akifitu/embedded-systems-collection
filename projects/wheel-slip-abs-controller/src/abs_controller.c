#include "abs_controller.h"

#include "pressure_modulator.h"
#include "slip_estimator.h"

#define ABS_ACTIVE_THRESHOLD_PCT 14u

static void set_all_valves(abs_output_t *output, abs_valve_t valve) {
    unsigned wheel;

    for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
        output->valves[wheel] = valve;
    }
}

void abs_controller_init(abs_controller_t *controller) {
    controller->state = ABS_STATE_STANDBY;
    controller->latched_fault = ABS_FAULT_NONE;
}

abs_output_t abs_controller_step(abs_controller_t *controller,
                                 const abs_input_t *input) {
    abs_output_t output = {0};
    abs_fault_t fault;
    unsigned wheel;
    uint8_t max_slip = 0u;

    output.state = controller->state;
    output.fault = controller->latched_fault;
    output.pump_enabled = false;
    set_all_valves(&output, ABS_VALVE_HOLD);

    fault = slip_estimator_calculate(input, output.slip_pct);

    if (controller->state == ABS_STATE_FAULT) {
        if ((input->brake_pct == 0u) && (input->vehicle_speed_kph_x10 <= 50u)) {
            controller->state = ABS_STATE_STANDBY;
            controller->latched_fault = ABS_FAULT_NONE;
            output.state = ABS_STATE_STANDBY;
            output.fault = ABS_FAULT_NONE;
        } else {
            output.state = ABS_STATE_FAULT;
            output.fault = controller->latched_fault;
            return output;
        }
    }

    if (fault != ABS_FAULT_NONE) {
        controller->state = ABS_STATE_FAULT;
        controller->latched_fault = fault;
        output.state = ABS_STATE_FAULT;
        output.fault = fault;
        for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
            output.slip_pct[wheel] = 0u;
        }
        return output;
    }

    if ((input->brake_pct < 5u) || (input->vehicle_speed_kph_x10 < 30u)) {
        controller->state = ABS_STATE_STANDBY;
        controller->latched_fault = ABS_FAULT_NONE;
        output.state = ABS_STATE_STANDBY;
        output.fault = ABS_FAULT_NONE;
        return output;
    }

    for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
        if (output.slip_pct[wheel] > max_slip) {
            max_slip = output.slip_pct[wheel];
        }
    }

    if (max_slip >= ABS_ACTIVE_THRESHOLD_PCT) {
        controller->state = ABS_STATE_ABS_ACTIVE;
        output.state = ABS_STATE_ABS_ACTIVE;
        output.fault = ABS_FAULT_NONE;
        output.pump_enabled = true;
        for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
            output.valves[wheel] =
                pressure_modulator_command(output.slip_pct[wheel]);
        }
        return output;
    }

    controller->state = ABS_STATE_BRAKING;
    output.state = ABS_STATE_BRAKING;
    output.fault = ABS_FAULT_NONE;
    set_all_valves(&output, ABS_VALVE_APPLY);
    return output;
}
