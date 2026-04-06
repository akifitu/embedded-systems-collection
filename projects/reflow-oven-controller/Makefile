CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude
LDLIBS ?= -lm

SRC := \
	src/reflow_types.c \
	src/profile_manager.c \
	src/pid_controller.c \
	src/reflow_controller.c \
	src/main.c

TEST_SRC := \
	src/reflow_types.c \
	src/profile_manager.c \
	src/pid_controller.c \
	src/reflow_controller.c \
	tests/test_reflow.c

.PHONY: all test run clean

all: build/reflow_demo

build/reflow_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

build/test_reflow: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDLIBS)

test: build/test_reflow
	./build/test_reflow

run: build/reflow_demo
	./build/reflow_demo

build:
	mkdir -p build

clean:
	rm -rf build
