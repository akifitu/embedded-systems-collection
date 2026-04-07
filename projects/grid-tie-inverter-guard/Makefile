CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/inverter_guard_types.c \
	src/grid_monitor.c \
	src/derating_model.c \
	src/inverter_guard.c \
	src/main.c

TEST_SRC := \
	src/inverter_guard_types.c \
	src/grid_monitor.c \
	src/derating_model.c \
	src/inverter_guard.c \
	tests/test_inverter_guard.c

.PHONY: all test run clean

all: build/inverter_guard_demo

build/inverter_guard_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_inverter_guard: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_inverter_guard
	./build/test_inverter_guard

run: build/inverter_guard_demo
	./build/inverter_guard_demo

build:
	mkdir -p build

clean:
	rm -rf build
