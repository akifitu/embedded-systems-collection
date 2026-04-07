CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/abs_types.c \
	src/slip_estimator.c \
	src/pressure_modulator.c \
	src/abs_controller.c \
	src/main.c

TEST_SRC := \
	src/abs_types.c \
	src/slip_estimator.c \
	src/pressure_modulator.c \
	src/abs_controller.c \
	tests/test_abs.c

.PHONY: all test run clean

all: build/abs_demo

build/abs_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_abs: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_abs
	./build/test_abs

run: build/abs_demo
	./build/abs_demo

build:
	mkdir -p build

clean:
	rm -rf build
