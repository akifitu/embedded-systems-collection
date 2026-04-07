CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/burner_types.c \
	src/interlock_guard.c \
	src/ignition_window.c \
	src/gas_burner_controller.c \
	src/main.c

TEST_SRC := \
	src/burner_types.c \
	src/interlock_guard.c \
	src/ignition_window.c \
	src/gas_burner_controller.c \
	tests/test_burner.c

.PHONY: all test run clean

all: build/burner_demo

build/burner_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_burner: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_burner
	./build/test_burner

run: build/burner_demo
	./build/burner_demo

build:
	mkdir -p build

clean:
	rm -rf build
