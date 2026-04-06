#include <stdbool.h>
#include <stdio.h>

#include "stepper_controller.h"

static stepper_feedback_t feedback_for_controller(
    const stepper_controller_t *controller, bool force_stall) {
    stepper_feedback_t feedback;

    feedback.home_switch = controller->position_steps <= 12;
    feedback.min_limit_switch = controller->position_steps <= 0;
    feedback.max_limit_switch = controller->position_steps >= 15000;
    feedback.stalled = force_stall;
    return feedback;
}

static void print_snapshot(const char *phase, const stepper_output_t *output) {
    char fault_buffer[64];

    stepper_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));
    printf(
        "phase=%s state=%s pos=%d target=%d rate=%u dir=%s progress=%u "
        "faults=%s\n",
        phase, stepper_state_name(output->state), output->position_steps,
        output->target_steps, output->step_rate_sps,
        stepper_direction_name(output->direction), output->progress_percent,
        fault_buffer);
}

static void run_demo(void) {
    stepper_controller_t controller;
    stepper_output_t output;
    stepper_command_t command = {0};
    stepper_feedback_t feedback;
    bool printed = false;
    int tick;

    stepper_controller_init(&controller, 2500);

    printed = false;
    for (tick = 0; tick < 400; ++tick) {
        command.start_homing = (tick == 0);
        command.clear_fault = false;
        command.move_request = false;
        command.target_steps = 0;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);

        if (output.state == STEPPER_STATE_HOMING &&
            output.progress_percent >= 50u && output.progress_percent < 60u) {
            print_snapshot("home_seek", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("home_seek", &output);
    }

    printed = false;
    for (tick = 0; tick < 400; ++tick) {
        command.start_homing = false;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_READY) {
            print_snapshot("home_latched", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("home_latched", &output);
    }

    printed = false;
    for (tick = 0; tick < 600; ++tick) {
        command.start_homing = false;
        command.clear_fault = false;
        command.move_request = (tick == 0);
        command.target_steps = 12000;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);

        if (output.state == STEPPER_STATE_MOVING &&
            output.progress_percent >= 13u && output.progress_percent < 20u) {
            print_snapshot("move_profile", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("move_profile", &output);
    }

    printed = false;
    for (tick = 0; tick < 600; ++tick) {
        command.move_request = false;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_READY &&
            output.position_steps == 12000) {
            print_snapshot("move_complete", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("move_complete", &output);
    }

    printed = false;
    for (tick = 0; tick < 500; ++tick) {
        command.move_request = (tick == 0);
        command.target_steps = 16000;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_FAULT) {
            print_snapshot("limit_abort", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("limit_abort", &output);
    }

    command.clear_fault = true;
    command.start_homing = false;
    command.move_request = false;
    feedback = feedback_for_controller(&controller, false);
    (void)stepper_controller_step(&controller, &command, &feedback);

    for (tick = 0; tick < 20; ++tick) {
        command.clear_fault = false;
        command.start_homing = true;
        command.move_request = false;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_HOMING) {
            break;
        }
    }

    printed = false;
    for (tick = 0; tick < 1500; ++tick) {
        command.start_homing = false;
        feedback = feedback_for_controller(&controller, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_READY) {
            print_snapshot("rehome_complete", &output);
            printed = true;
            break;
        }
    }
    if (!printed) {
        print_snapshot("rehome_complete", &output);
    }
}

int main(void) {
    run_demo();
    return 0;
}
