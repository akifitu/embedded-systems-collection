CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/triplex_types.c \
	src/lane_health.c \
	src/vote_math.c \
	src/triplex_flight_control_voter.c \
	src/main.c

TEST_SRC := \
	src/triplex_types.c \
	src/lane_health.c \
	src/vote_math.c \
	src/triplex_flight_control_voter.c \
	tests/test_triplex_voter.c

.PHONY: all test run clean

all: build/triplex_voter_demo

build/triplex_voter_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_triplex_voter: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_triplex_voter
	./build/test_triplex_voter

run: build/triplex_voter_demo
	./build/triplex_voter_demo

build:
	mkdir -p build

clean:
	rm -rf build
