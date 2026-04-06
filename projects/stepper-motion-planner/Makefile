CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude
LDLIBS ?= -lm

SRC := \
	src/motion_types.c \
	src/trajectory_planner.c \
	src/stepper_controller.c \
	src/main.c

TEST_SRC := \
	src/motion_types.c \
	src/trajectory_planner.c \
	src/stepper_controller.c \
	tests/test_stepper.c

.PHONY: all test run clean

all: build/stepper_demo

build/stepper_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

build/test_stepper: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDLIBS)

test: build/test_stepper
	./build/test_stepper

run: build/stepper_demo
	./build/stepper_demo

build:
	mkdir -p build

clean:
	rm -rf build
