# Fire Panel Loop Controller

Portable C model of a conventional fire-alarm control panel loop. The project
verifies smoke alarms before evacuation, reacts immediately to manual pull
stations, supervises loop trouble conditions, and supports NAC silence/reset
behavior without requiring any board-specific dependencies.

## Why It Matters

- Shows building life-safety control instead of only generic MCU demos
- Demonstrates alarm verification, NAC output policy, and fault latching
- Maps directly to fire panel, annunciator, and supervised-input firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=MONITOR_LOOP alarm=NONE trouble=NONE nac=OFF buzzer=OFF alarm_led=OFF trouble_led=OFF verify=0
phase=verify_smoke state=VERIFY cmd=START_VERIFY alarm=SMOKE trouble=NONE nac=OFF buzzer=OFF alarm_led=ON trouble_led=OFF verify=50
phase=confirmed_alarm state=ALARM cmd=ACTIVATE_NACS alarm=SMOKE trouble=NONE nac=ON buzzer=ON alarm_led=ON trouble_led=OFF verify=100
phase=silenced state=SILENCED cmd=SILENCE_NACS alarm=SMOKE trouble=NONE nac=OFF buzzer=OFF alarm_led=ON trouble_led=OFF verify=100
phase=manual_pull state=ALARM cmd=ACTIVATE_NACS alarm=MANUAL_PULL trouble=NONE nac=ON buzzer=ON alarm_led=ON trouble_led=OFF verify=0
phase=trouble_loop state=TROUBLE cmd=LATCH_TROUBLE alarm=NONE trouble=LOOP_OPEN nac=OFF buzzer=ON alarm_led=OFF trouble_led=ON verify=0
phase=reset_ready state=IDLE cmd=RESET_PANEL alarm=NONE trouble=NONE nac=OFF buzzer=OFF alarm_led=OFF trouble_led=OFF verify=0
```

## Implementation Notes

- `zone_evaluator.c` classifies smoke, manual-pull, and supervised-loop trouble inputs
- `nac_policy.c` translates panel state into NAC, buzzer, and annunciator outputs
- `fire_panel_controller.c` implements verify, alarm latch, silence, trouble, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, AVR, or a safety-focused panel
MCU with supervised IDC/NAC loops, smoke detector inputs, manual pull stations,
local piezo buzzer drive, and annunciator LEDs.
