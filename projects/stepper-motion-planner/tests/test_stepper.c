#include <assert.h>
#include <stdio.h>

#include "stepper_controller.h"

static stepper_feedback_t feedback_for_position(int32_t position_steps,
                                                bool stalled) {
    stepper_feedback_t feedback;

    feedback.home_switch = position_steps <= 12;
    feedback.min_limit_switch = position_steps <= 0;
    feedback.max_limit_switch = position_steps >= 15000;
    feedback.stalled = stalled;
    return feedback;
}

static void test_short_move_is_triangular(void) {
    trajectory_plan_t plan;

    trajectory_plan_build(&plan, 0, 600, 4800.0f, 24000.0f);
    assert(plan.distance_steps == 600u);
    assert(trajectory_plan_is_triangular(&plan));
}

static void test_homing_latches_zero(void) {
    stepper_controller_t controller;
    stepper_command_t command = {0};
    stepper_feedback_t feedback;
    stepper_output_t output = {0};
    int tick;

    stepper_controller_init(&controller, 2500);
    for (tick = 0; tick < 400; ++tick) {
        command.start_homing = (tick == 0);
        feedback = feedback_for_position(controller.position_steps, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_READY) {
            break;
        }
    }

    assert(output.state == STEPPER_STATE_READY);
    assert(output.position_steps == 0);
    assert(output.faults == STEPPER_FAULT_NONE);
}

static void test_nominal_move_reaches_target(void) {
    stepper_controller_t controller;
    stepper_command_t command = {0};
    stepper_feedback_t feedback;
    stepper_output_t output = {0};
    int tick;

    stepper_controller_init(&controller, 0);
    controller.state = STEPPER_STATE_READY;
    controller.target_steps = 0;

    for (tick = 0; tick < 500; ++tick) {
        command.move_request = (tick == 0);
        command.target_steps = 12000;
        feedback = feedback_for_position(controller.position_steps, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_READY &&
            output.position_steps == 12000) {
            break;
        }
    }

    assert(output.state == STEPPER_STATE_READY);
    assert(output.position_steps == 12000);
    assert(output.faults == STEPPER_FAULT_NONE);
}

static void test_limit_switch_faults_move(void) {
    stepper_controller_t controller;
    stepper_command_t command = {0};
    stepper_feedback_t feedback;
    stepper_output_t output = {0};
    int tick;

    stepper_controller_init(&controller, 12000);
    controller.state = STEPPER_STATE_READY;
    controller.target_steps = 12000;

    for (tick = 0; tick < 500; ++tick) {
        command.move_request = (tick == 0);
        command.target_steps = 16000;
        feedback = feedback_for_position(controller.position_steps, false);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_FAULT) {
            break;
        }
    }

    assert(output.state == STEPPER_STATE_FAULT);
    assert((output.faults & STEPPER_FAULT_LIMIT_HIT) != 0u);
}

static void test_stall_fault_latches(void) {
    stepper_controller_t controller;
    stepper_command_t command = {0};
    stepper_feedback_t feedback;
    stepper_output_t output = {0};
    int tick;

    stepper_controller_init(&controller, 0);
    controller.state = STEPPER_STATE_READY;
    controller.target_steps = 0;

    for (tick = 0; tick < 100; ++tick) {
        command.move_request = (tick == 0);
        command.target_steps = 8000;
        feedback = feedback_for_position(controller.position_steps, true);
        output = stepper_controller_step(&controller, &command, &feedback);
        if (output.state == STEPPER_STATE_FAULT) {
            break;
        }
    }

    assert(output.state == STEPPER_STATE_FAULT);
    assert((output.faults & STEPPER_FAULT_STALL) != 0u);
}

int main(void) {
    test_short_move_is_triangular();
    test_homing_latches_zero();
    test_nominal_move_reaches_target();
    test_limit_switch_faults_move();
    test_stall_fault_latches();

    puts("stepper-motion-planner tests passed");
    return 0;
}
