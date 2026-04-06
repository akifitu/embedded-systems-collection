CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/pd_types.c \
	src/pdo_selector.c \
	src/sink_policy.c \
	src/main.c

TEST_SRC := \
	src/pd_types.c \
	src/pdo_selector.c \
	src/sink_policy.c \
	tests/test_usb_pd.c

.PHONY: all test run clean

all: build/usb_pd_demo

build/usb_pd_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_usb_pd: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_usb_pd
	./build/test_usb_pd

run: build/usb_pd_demo
	./build/usb_pd_demo

build:
	mkdir -p build

clean:
	rm -rf build
