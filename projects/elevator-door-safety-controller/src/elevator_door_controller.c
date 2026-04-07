#include "elevator_door_controller.h"

#include "door_monitor.h"
#include "obstruction_policy.h"

static elevator_door_output_t make_output(
    const elevator_door_controller_t *controller,
    door_position_t position, door_lock_t lock, door_command_t command,
    door_fault_t fault) {
    elevator_door_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.position = position;
    output.lock = lock;
    output.buzzer_active =
        (controller->state == DOOR_STATE_NUDGE) ||
        (controller->state == DOOR_STATE_FAULT);
    output.obstruction_retries = controller->obstruction_retries;

    switch (controller->state) {
        case DOOR_STATE_OPENING:
            output.motor = DOOR_MOTOR_OPEN;
            break;

        case DOOR_STATE_CLOSING:
        case DOOR_STATE_NUDGE:
            output.motor = DOOR_MOTOR_CLOSE;
            break;

        case DOOR_STATE_CLOSED:
        case DOOR_STATE_OPEN:
        case DOOR_STATE_FAULT:
        default:
            output.motor = DOOR_MOTOR_STOP;
            break;
    }

    return output;
}

static elevator_door_output_t enter_fault(
    elevator_door_controller_t *controller, door_position_t position,
    door_lock_t lock, door_fault_t fault) {
    controller->state = DOOR_STATE_FAULT;
    controller->latched_fault = fault;
    controller->motion_frames = 0u;
    return make_output(controller, position, lock, DOOR_COMMAND_LATCH_FAULT,
                       fault);
}

void elevator_door_controller_init(elevator_door_controller_t *controller) {
    controller->state = DOOR_STATE_CLOSED;
    controller->latched_fault = DOOR_FAULT_NONE;
    controller->obstruction_retries = 0u;
    controller->motion_frames = 0u;
}

elevator_door_output_t elevator_door_controller_step(
    elevator_door_controller_t *controller, const elevator_door_input_t *input) {
    door_position_t position;
    door_lock_t lock;

    position = door_monitor_position(input->door_open_fb, input->door_closed_fb);
    lock = input->lock_engaged ? DOOR_LOCK_LOCKED : DOOR_LOCK_UNLOCKED;

    if (controller->state == DOOR_STATE_FAULT) {
        if (input->reset_request && (position == DOOR_POSITION_CLOSED) &&
            (lock == DOOR_LOCK_LOCKED)) {
            controller->state = DOOR_STATE_CLOSED;
            controller->latched_fault = DOOR_FAULT_NONE;
            controller->obstruction_retries = 0u;
            controller->motion_frames = 0u;
            return make_output(controller, position, lock, DOOR_COMMAND_RESET_DOOR,
                               DOOR_FAULT_NONE);
        }

        return make_output(controller, position, lock, DOOR_COMMAND_LATCH_FAULT,
                           controller->latched_fault);
    }

    switch (controller->state) {
        case DOOR_STATE_CLOSED:
            if (input->open_request) {
                controller->state = DOOR_STATE_OPENING;
                controller->motion_frames = 1u;
                return make_output(controller, position, lock,
                                   DOOR_COMMAND_DRIVE_OPEN, DOOR_FAULT_NONE);
            }
            return make_output(controller, position, lock,
                               DOOR_COMMAND_HOLD_CLOSED, DOOR_FAULT_NONE);

        case DOOR_STATE_OPENING:
            if (position == DOOR_POSITION_OPEN) {
                controller->state = DOOR_STATE_OPEN;
                controller->motion_frames = 0u;
                return make_output(controller, position, lock,
                                   DOOR_COMMAND_HOLD_OPEN, DOOR_FAULT_NONE);
            }
            if (door_monitor_timed_out(controller->motion_frames)) {
                return enter_fault(controller, position, lock,
                                   DOOR_FAULT_MOTION_TIMEOUT);
            }
            controller->motion_frames++;
            return make_output(controller, position, lock, DOOR_COMMAND_DRIVE_OPEN,
                               DOOR_FAULT_NONE);

        case DOOR_STATE_OPEN:
            if (input->close_request) {
                controller->state = obstruction_policy_should_nudge(
                                        controller->obstruction_retries)
                                        ? DOOR_STATE_NUDGE
                                        : DOOR_STATE_CLOSING;
                controller->motion_frames = 1u;
                return make_output(
                    controller, position, lock,
                    (controller->state == DOOR_STATE_NUDGE)
                        ? DOOR_COMMAND_NUDGE_CLOSE
                        : DOOR_COMMAND_DRIVE_CLOSE,
                    DOOR_FAULT_NONE);
            }
            return make_output(controller, position, lock, DOOR_COMMAND_HOLD_OPEN,
                               DOOR_FAULT_NONE);

        case DOOR_STATE_CLOSING:
            if (input->obstruction_detected) {
                controller->obstruction_retries =
                    obstruction_policy_record_retry(controller->obstruction_retries);
                controller->state = DOOR_STATE_OPENING;
                controller->motion_frames = 1u;
                return make_output(controller, position, lock,
                                   DOOR_COMMAND_DRIVE_OPEN, DOOR_FAULT_NONE);
            }
            if (position == DOOR_POSITION_CLOSED) {
                controller->motion_frames = 0u;
                if (lock != DOOR_LOCK_LOCKED) {
                    return enter_fault(controller, position, lock,
                                       DOOR_FAULT_LOCK_FAIL);
                }
                controller->state = DOOR_STATE_CLOSED;
                controller->obstruction_retries = 0u;
                return make_output(controller, position, lock,
                                   DOOR_COMMAND_HOLD_CLOSED, DOOR_FAULT_NONE);
            }
            if (door_monitor_timed_out(controller->motion_frames)) {
                return enter_fault(controller, position, lock,
                                   DOOR_FAULT_MOTION_TIMEOUT);
            }
            controller->motion_frames++;
            return make_output(controller, position, lock,
                               DOOR_COMMAND_DRIVE_CLOSE, DOOR_FAULT_NONE);

        case DOOR_STATE_NUDGE:
            if (position == DOOR_POSITION_CLOSED) {
                controller->motion_frames = 0u;
                if (lock != DOOR_LOCK_LOCKED) {
                    return enter_fault(controller, position, lock,
                                       DOOR_FAULT_LOCK_FAIL);
                }
                controller->state = DOOR_STATE_CLOSED;
                controller->obstruction_retries = 0u;
                return make_output(controller, position, lock,
                                   DOOR_COMMAND_HOLD_CLOSED, DOOR_FAULT_NONE);
            }
            if (door_monitor_timed_out(controller->motion_frames)) {
                return enter_fault(controller, position, lock,
                                   DOOR_FAULT_MOTION_TIMEOUT);
            }
            controller->motion_frames++;
            return make_output(controller, position, lock,
                               DOOR_COMMAND_NUDGE_CLOSE, DOOR_FAULT_NONE);

        case DOOR_STATE_FAULT:
        default:
            return make_output(controller, position, lock,
                               DOOR_COMMAND_LATCH_FAULT,
                               controller->latched_fault);
    }
}
