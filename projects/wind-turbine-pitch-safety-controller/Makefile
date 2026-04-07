CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/turbine_types.c \
	src/wind_guard.c \
	src/pitch_plan.c \
	src/wind_turbine_controller.c \
	src/main.c

TEST_SRC := \
	src/turbine_types.c \
	src/wind_guard.c \
	src/pitch_plan.c \
	src/wind_turbine_controller.c \
	tests/test_wind_turbine.c

.PHONY: all test run clean

all: build/wind_turbine_demo

build/wind_turbine_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_wind_turbine: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_wind_turbine
	./build/test_wind_turbine

run: build/wind_turbine_demo
	./build/wind_turbine_demo

build:
	mkdir -p build

clean:
	rm -rf build
