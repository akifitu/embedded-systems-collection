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
	projects/uds-diagnostic-node

.PHONY: all test clean run-bms run-ota run-can run-motor run-journal run-power run-modbus run-attest run-rtos run-imu run-reflow run-pd run-stepper run-bldc run-sequencer run-sboot run-uds

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
