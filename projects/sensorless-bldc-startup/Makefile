CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/bldc_types.c \
	src/commutation_table.c \
	src/startup_controller.c \
	src/main.c

TEST_SRC := \
	src/bldc_types.c \
	src/commutation_table.c \
	src/startup_controller.c \
	tests/test_bldc.c

.PHONY: all test run clean

all: build/bldc_demo

build/bldc_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_bldc: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_bldc
	./build/test_bldc

run: build/bldc_demo
	./build/bldc_demo

build:
	mkdir -p build

clean:
	rm -rf build
