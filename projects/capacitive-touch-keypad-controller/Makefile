CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/touch_types.c \
	src/baseline_tracker.c \
	src/keypad_controller.c \
	src/main.c

TEST_SRC := \
	src/touch_types.c \
	src/baseline_tracker.c \
	src/keypad_controller.c \
	tests/test_touch.c

.PHONY: all test run clean

all: build/touch_demo

build/touch_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_touch: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_touch
	./build/test_touch

run: build/touch_demo
	./build/touch_demo

build:
	mkdir -p build

clean:
	rm -rf build
