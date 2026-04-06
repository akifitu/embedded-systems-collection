CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude
BUILD_DIR := build
SRC := $(wildcard src/*.c)
APP := $(BUILD_DIR)/rtos_scheduler_demo
TEST := $(BUILD_DIR)/test_rtos_scheduler
APP_SRC := $(SRC)
LIB_SRC := $(filter-out src/main.c,$(SRC))

.PHONY: all test run clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(APP_SRC) -o $(APP)

$(TEST): tests/test_scheduler.c $(LIB_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_scheduler.c $(LIB_SRC) -o $(TEST)

test: $(TEST)
	./$(TEST)

run: $(APP)
	./$(APP)

clean:
	rm -rf $(BUILD_DIR)

