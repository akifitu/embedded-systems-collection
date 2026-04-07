CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/generator_types.c \
	src/engine_guard.c \
	src/crank_manager.c \
	src/generator_controller.c \
	src/main.c

TEST_SRC := \
	src/generator_types.c \
	src/engine_guard.c \
	src/crank_manager.c \
	src/generator_controller.c \
	tests/test_generator.c

.PHONY: all test run clean

all: build/generator_demo

build/generator_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_generator: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_generator
	./build/test_generator

run: build/generator_demo
	./build/generator_demo

build:
	mkdir -p build

clean:
	rm -rf build
