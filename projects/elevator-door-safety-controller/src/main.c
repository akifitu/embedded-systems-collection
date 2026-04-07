#include <stdio.h>

#include "elevator_door_controller.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static elevator_door_input_t make_input(bool open_request, bool close_request,
                                        bool obstruction_detected,
                                        bool door_open_fb,
                                        bool door_closed_fb,
                                        bool lock_engaged,
                                        bool reset_request) {
    elevator_door_input_t input;

    input.open_request = open_request;
    input.close_request = close_request;
    input.obstruction_detected = obstruction_detected;
    input.door_open_fb = door_open_fb;
    input.door_closed_fb = door_closed_fb;
    input.lock_engaged = lock_engaged;
    input.reset_request = reset_request;
    return input;
}

static void print_phase(const char *phase,
                        const elevator_door_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s door=%s motor=%s lock=%s "
        "buzzer=%s retry=%u\n",
        phase, door_state_name(output->state),
        door_command_name(output->command), door_fault_name(output->fault),
        door_position_name(output->position), door_motor_name(output->motor),
        door_lock_name(output->lock), on_off(output->buzzer_active),
        output->obstruction_retries);
}

int main(void) {
    elevator_door_controller_t controller;
    elevator_door_controller_t fault_controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(false, false, false, false, true, true, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("idle_closed", &output);

    input = make_input(true, false, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("arrival_opening", &output);

    input = make_input(false, false, false, true, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("boarding_open", &output);

    input = make_input(false, true, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("close_attempt", &output);

    input = make_input(false, false, true, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("obstruction_retry", &output);

    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, true, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    print_phase("nudge_close", &output);

    elevator_door_controller_init(&fault_controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&fault_controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&fault_controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&fault_controller, &input);
    input = make_input(false, false, false, false, true, false, false);
    output = elevator_door_controller_step(&fault_controller, &input);
    print_phase("lock_fault", &output);

    input = make_input(false, false, false, false, true, true, true);
    output = elevator_door_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
