# compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude
LDFLAGS = -lreadline

# directory setup
SRC_DIR = src
SRC_MAIN_DIR = $(SRC_DIR)/main
SRC_TEST_DIR = $(SRC_DIR)/test

BUILD_DIR = build
BUILD_MAIN_DIR = $(BUILD_DIR)/main
BUILD_TEST_DIR = $(BUILD_DIR)/test

BIN_DIR = bin
BIN_MAIN_DIR = $(BIN_DIR)/main
BIN_TEST_DIR = $(BIN_DIR)/test

# keep intermediates
.SECONDARY:

# main executable
SRC_MAIN = $(wildcard $(SRC_MAIN_DIR)/*.c)
OBJ_MAIN = $(patsubst $(SRC_MAIN_DIR)/%.c,$(BUILD_MAIN_DIR)/%.o,$(SRC_MAIN))
MAIN_EXE = $(BIN_MAIN_DIR)/main

# test executables
TEST_SOURCES = $(wildcard $(SRC_TEST_DIR)/*.c)
TEST_OBJECTS = $(patsubst $(SRC_TEST_DIR)/%.c,$(BUILD_TEST_DIR)/%.o,$(TEST_SOURCES))
TEST_EXECUTABLES = $(patsubst $(SRC_TEST_DIR)/%.c,$(BIN_TEST_DIR)/%,$(TEST_SOURCES))

# default make command
all: $(MAIN_EXE) $(TEST_EXECUTABLES)

# make main
main: $(MAIN_EXE)

# make tests
tests: $(TEST_EXECUTABLES)

$(MAIN_EXE): $(OBJ_MAIN) | $(BIN_MAIN_DIR)
	$(CC) $(OBJ_MAIN) -o $@ $(LDFLAGS)

$(BUILD_MAIN_DIR)/%.o: $(SRC_MAIN_DIR)/%.c | $(BUILD_MAIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

# create main directories if not already there
$(BUILD_MAIN_DIR):
	@mkdir -p $(BUILD_MAIN_DIR)

$(BIN_MAIN_DIR):
	@mkdir -p $(BIN_MAIN_DIR)

# test executables
$(BIN_TEST_DIR)/%: $(BUILD_TEST_DIR)/%.o | $(BIN_TEST_DIR)
	$(CC) $< -o $@

$(BUILD_TEST_DIR)/%.o: $(SRC_TEST_DIR)/%.c | $(BUILD_TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# create test directories if not already there
$(BUILD_TEST_DIR):
	@mkdir -p $(BUILD_TEST_DIR)

$(BIN_TEST_DIR):
	@mkdir -p $(BIN_TEST_DIR)


# clean
clean:
	rm -rf build bin