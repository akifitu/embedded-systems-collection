CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/boot_types.c \
	src/sha256.c \
	src/hmac_sha256.c \
	src/manifest_verifier.c \
	src/main.c

TEST_SRC := \
	src/boot_types.c \
	src/sha256.c \
	src/hmac_sha256.c \
	src/manifest_verifier.c \
	tests/test_manifest.c

.PHONY: all test run clean

all: build/sboot_demo

build/sboot_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_manifest: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_manifest
	./build/test_manifest

run: build/sboot_demo
	./build/sboot_demo

build:
	mkdir -p build

clean:
	rm -rf build
