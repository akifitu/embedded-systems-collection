CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/door_types.c \
	src/door_monitor.c \
	src/obstruction_policy.c \
	src/elevator_door_controller.c \
	src/main.c

TEST_SRC := \
	src/door_types.c \
	src/door_monitor.c \
	src/obstruction_policy.c \
	src/elevator_door_controller.c \
	tests/test_elevator_door.c

.PHONY: all test run clean

all: build/elevator_door_demo

build/elevator_door_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_elevator_door: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_elevator_door
	./build/test_elevator_door

run: build/elevator_door_demo
	./build/elevator_door_demo

build:
	mkdir -p build

clean:
	rm -rf build
