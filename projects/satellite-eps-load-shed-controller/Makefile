CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/eps_types.c \
	src/battery_guard.c \
	src/load_plan.c \
	src/satellite_eps_controller.c \
	src/main.c

TEST_SRC := \
	src/eps_types.c \
	src/battery_guard.c \
	src/load_plan.c \
	src/satellite_eps_controller.c \
	tests/test_satellite_eps.c

.PHONY: all test run clean

all: build/satellite_eps_demo

build/satellite_eps_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_satellite_eps: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_satellite_eps
	./build/test_satellite_eps

run: build/satellite_eps_demo
	./build/satellite_eps_demo

build:
	mkdir -p build

clean:
	rm -rf build
