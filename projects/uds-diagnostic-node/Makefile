CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/uds_types.c \
	src/did_table.c \
	src/uds_node.c \
	src/main.c

TEST_SRC := \
	src/uds_types.c \
	src/did_table.c \
	src/uds_node.c \
	tests/test_uds.c

.PHONY: all test run clean

all: build/uds_demo

build/uds_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_uds: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_uds
	./build/test_uds

run: build/uds_demo
	./build/uds_demo

build:
	mkdir -p build

clean:
	rm -rf build
