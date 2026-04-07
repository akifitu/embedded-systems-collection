PROJECTS := \
	projects/smart-bms-firmware \
	projects/ota-bootloader-simulator \
	projects/can-telemetry-node \
	projects/motor-condition-monitor \
	projects/resilient-flash-journal \
	projects/energy-harvesting-node \
	projects/modbus-rtu-field-node \
	projects/secure-attestation-node \
	projects/rtos-scheduler-lab \
	projects/imu-attitude-estimator \
	projects/reflow-oven-controller \
	projects/usb-pd-sink-controller \
	projects/stepper-motion-planner \
	projects/sensorless-bldc-startup \
	projects/multi-rail-power-sequencer \
	projects/secure-boot-manifest-verifier \
	projects/uds-diagnostic-node \
	projects/capacitive-touch-keypad-controller \
	projects/gpsdo-holdover-controller \
	projects/uav-failsafe-controller \
	projects/grid-tie-inverter-guard \
	projects/wheel-slip-abs-controller \
	projects/evse-charge-port-controller \
	projects/dual-pump-lift-station-controller \
	projects/diesel-generator-autostart-controller \
	projects/railway-grade-crossing-controller

.PHONY: all test clean run-bms run-ota run-can run-motor run-journal run-power run-modbus run-attest run-rtos run-imu run-reflow run-pd run-stepper run-bldc run-sequencer run-sboot run-uds run-touch run-gpsdo run-uav run-inverter run-abs run-evse run-lift run-gen run-crossing

all:
	@for project in $(PROJECTS); do $(MAKE) -C $$project all; done

test:
	@for project in $(PROJECTS); do $(MAKE) -C $$project test; done

clean:
	@for project in $(PROJECTS); do $(MAKE) -C $$project clean; done

run-bms:
	@$(MAKE) -C projects/smart-bms-firmware run

run-ota:
	@$(MAKE) -C projects/ota-bootloader-simulator run

run-can:
	@$(MAKE) -C projects/can-telemetry-node run

run-motor:
	@$(MAKE) -C projects/motor-condition-monitor run

run-journal:
	@$(MAKE) -C projects/resilient-flash-journal run

run-power:
	@$(MAKE) -C projects/energy-harvesting-node run

run-modbus:
	@$(MAKE) -C projects/modbus-rtu-field-node run

run-attest:
	@$(MAKE) -C projects/secure-attestation-node run

run-rtos:
	@$(MAKE) -C projects/rtos-scheduler-lab run

run-imu:
	@$(MAKE) -C projects/imu-attitude-estimator run

run-reflow:
	@$(MAKE) -C projects/reflow-oven-controller run

run-pd:
	@$(MAKE) -C projects/usb-pd-sink-controller run

run-stepper:
	@$(MAKE) -C projects/stepper-motion-planner run

run-bldc:
	@$(MAKE) -C projects/sensorless-bldc-startup run

run-sequencer:
	@$(MAKE) -C projects/multi-rail-power-sequencer run

run-sboot:
	@$(MAKE) -C projects/secure-boot-manifest-verifier run

run-uds:
	@$(MAKE) -C projects/uds-diagnostic-node run

run-touch:
	@$(MAKE) -C projects/capacitive-touch-keypad-controller run

run-gpsdo:
	@$(MAKE) -C projects/gpsdo-holdover-controller run

run-uav:
	@$(MAKE) -C projects/uav-failsafe-controller run

run-inverter:
	@$(MAKE) -C projects/grid-tie-inverter-guard run

run-abs:
	@$(MAKE) -C projects/wheel-slip-abs-controller run

run-evse:
	@$(MAKE) -C projects/evse-charge-port-controller run

run-lift:
	@$(MAKE) -C projects/dual-pump-lift-station-controller run

run-gen:
	@$(MAKE) -C projects/diesel-generator-autostart-controller run

run-crossing:
	@$(MAKE) -C projects/railway-grade-crossing-controller run
