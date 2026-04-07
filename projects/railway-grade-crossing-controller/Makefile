CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/crossing_types.c \
	src/train_zone.c \
	src/gate_monitor.c \
	src/grade_crossing_controller.c \
	src/main.c

TEST_SRC := \
	src/crossing_types.c \
	src/train_zone.c \
	src/gate_monitor.c \
	src/grade_crossing_controller.c \
	tests/test_crossing.c

.PHONY: all test run clean

all: build/crossing_demo

build/crossing_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_crossing: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_crossing
	./build/test_crossing

run: build/crossing_demo
	./build/crossing_demo

build:
	mkdir -p build

clean:
	rm -rf build
