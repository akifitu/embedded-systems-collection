#include "grade_crossing_controller.h"

#include "gate_monitor.h"
#include "train_zone.h"

#define PREWARN_TARGET_FRAMES 2u

static crossing_output_t make_output(const grade_crossing_controller_t *controller,
                                     train_zone_t zone,
                                     gate_position_t gate_position,
                                     crossing_command_t command,
                                     crossing_fault_t fault, bool warning_active,
                                     bool bell_active) {
    crossing_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.zone = zone;
    output.gate_position = gate_position;
    output.warning_active = warning_active;
    output.bell_active = bell_active;
    return output;
}

static crossing_output_t enter_fault(grade_crossing_controller_t *controller,
                                     train_zone_t zone,
                                     gate_position_t gate_position,
                                     crossing_fault_t fault, bool bell_active) {
    controller->state = CROSSING_STATE_FAULT;
    controller->latched_fault = fault;
    controller->prewarn_frames = 0u;
    controller->motion_frames = 0u;
    return make_output(controller, zone, gate_position,
                       CROSSING_COMMAND_LATCH_FAULT, fault, true, bell_active);
}

void grade_crossing_controller_init(grade_crossing_controller_t *controller) {
    controller->state = CROSSING_STATE_IDLE;
    controller->latched_fault = CROSSING_FAULT_NONE;
    controller->prewarn_frames = 0u;
    controller->motion_frames = 0u;
}

crossing_output_t grade_crossing_controller_step(
    grade_crossing_controller_t *controller, const crossing_input_t *input) {
    train_zone_t zone;
    gate_position_t gate_position;

    zone = train_zone_eval(input->approach_active, input->island_occupied);
    gate_position = gate_monitor_position(input->gate_down_fb, input->gate_up_fb);

    if (controller->state == CROSSING_STATE_FAULT) {
        if (input->reset_request && (zone == TRAIN_ZONE_CLEAR) && input->lamp_ok &&
            (gate_position == GATE_POSITION_UP)) {
            controller->state = CROSSING_STATE_IDLE;
            controller->latched_fault = CROSSING_FAULT_NONE;
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_GATE_UP, CROSSING_FAULT_NONE,
                               false, false);
        }

        return make_output(controller, zone, gate_position,
                           CROSSING_COMMAND_LATCH_FAULT,
                           controller->latched_fault, true,
                           zone != TRAIN_ZONE_CLEAR);
    }

    if (!input->lamp_ok &&
        ((controller->state != CROSSING_STATE_IDLE) || (zone != TRAIN_ZONE_CLEAR))) {
        return enter_fault(controller, zone, gate_position, CROSSING_FAULT_LAMP_FAIL,
                           true);
    }

    switch (controller->state) {
        case CROSSING_STATE_IDLE:
            if (zone != TRAIN_ZONE_CLEAR) {
                controller->state = CROSSING_STATE_PREWARN;
                controller->prewarn_frames = 1u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_START_WARNING,
                                   CROSSING_FAULT_NONE, true, true);
            }
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_GATE_UP, CROSSING_FAULT_NONE,
                               false, false);

        case CROSSING_STATE_PREWARN:
            if (zone == TRAIN_ZONE_CLEAR) {
                controller->state = CROSSING_STATE_IDLE;
                controller->prewarn_frames = 0u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_GATE_UP,
                                   CROSSING_FAULT_NONE, false, false);
            }
            if ((controller->prewarn_frames + 1u) >= PREWARN_TARGET_FRAMES) {
                controller->state = CROSSING_STATE_LOWERING;
                controller->motion_frames = 1u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_LOWER_GATE,
                                   CROSSING_FAULT_NONE, true, true);
            }
            controller->prewarn_frames++;
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_START_WARNING,
                               CROSSING_FAULT_NONE, true, true);

        case CROSSING_STATE_LOWERING:
            if (gate_position == GATE_POSITION_DOWN) {
                controller->state = CROSSING_STATE_PROTECTED;
                controller->motion_frames = 0u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_HOLD_DOWN,
                                   CROSSING_FAULT_NONE, true, false);
            }
            if (gate_monitor_timed_out(controller->motion_frames)) {
                return enter_fault(controller, zone, gate_position,
                                   CROSSING_FAULT_GATE_TIMEOUT, true);
            }
            controller->motion_frames++;
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_LOWER_GATE,
                               CROSSING_FAULT_NONE, true, true);

        case CROSSING_STATE_PROTECTED:
            if (zone == TRAIN_ZONE_CLEAR) {
                controller->state = CROSSING_STATE_RAISING;
                controller->motion_frames = 1u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_RAISE_GATE,
                                   CROSSING_FAULT_NONE, true, false);
            }
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_HOLD_DOWN,
                               CROSSING_FAULT_NONE, true, false);

        case CROSSING_STATE_RAISING:
            if (zone != TRAIN_ZONE_CLEAR) {
                controller->state = CROSSING_STATE_LOWERING;
                controller->motion_frames = 1u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_LOWER_GATE,
                                   CROSSING_FAULT_NONE, true, true);
            }
            if (gate_position == GATE_POSITION_UP) {
                controller->state = CROSSING_STATE_IDLE;
                controller->motion_frames = 0u;
                return make_output(controller, zone, gate_position,
                                   CROSSING_COMMAND_GATE_UP,
                                   CROSSING_FAULT_NONE, false, false);
            }
            if (gate_monitor_timed_out(controller->motion_frames)) {
                return enter_fault(controller, zone, gate_position,
                                   CROSSING_FAULT_GATE_TIMEOUT, false);
            }
            controller->motion_frames++;
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_RAISE_GATE,
                               CROSSING_FAULT_NONE, true, false);

        case CROSSING_STATE_FAULT:
        default:
            return make_output(controller, zone, gate_position,
                               CROSSING_COMMAND_LATCH_FAULT,
                               controller->latched_fault, true, true);
    }
}
