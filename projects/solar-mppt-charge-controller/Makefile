CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/solar_types.c \
	src/power_tracker.c \
	src/charge_policy.c \
	src/solar_mppt_controller.c \
	src/main.c

TEST_SRC := \
	src/solar_types.c \
	src/power_tracker.c \
	src/charge_policy.c \
	src/solar_mppt_controller.c \
	tests/test_solar_mppt.c

.PHONY: all test run clean

all: build/solar_mppt_demo

build/solar_mppt_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_solar_mppt: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_solar_mppt
	./build/test_solar_mppt

run: build/solar_mppt_demo
	./build/solar_mppt_demo

build:
	mkdir -p build

clean:
	rm -rf build
