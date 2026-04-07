CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/ventilator_types.c \
	src/patient_guard.c \
	src/breath_profile.c \
	src/ventilator_controller.c \
	src/main.c

TEST_SRC := \
	src/ventilator_types.c \
	src/patient_guard.c \
	src/breath_profile.c \
	src/ventilator_controller.c \
	tests/test_ventilator.c

.PHONY: all test run clean

all: build/ventilator_demo

build/ventilator_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_ventilator: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_ventilator
	./build/test_ventilator

run: build/ventilator_demo
	./build/ventilator_demo

build:
	mkdir -p build

clean:
	rm -rf build
