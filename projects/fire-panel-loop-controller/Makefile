CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/fire_panel_types.c \
	src/zone_evaluator.c \
	src/nac_policy.c \
	src/fire_panel_controller.c \
	src/main.c

TEST_SRC := \
	src/fire_panel_types.c \
	src/zone_evaluator.c \
	src/nac_policy.c \
	src/fire_panel_controller.c \
	tests/test_fire_panel.c

.PHONY: all test run clean

all: build/fire_panel_demo

build/fire_panel_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_fire_panel: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_fire_panel
	./build/test_fire_panel

run: build/fire_panel_demo
	./build/fire_panel_demo

build:
	mkdir -p build

clean:
	rm -rf build
