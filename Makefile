# Othello engine build.
#   make            -> native demo/bench binary (build/othello)
#   make test       -> build and run unit + perft tests
#   make bench       -> build and run the demo/perft benchmark
#   make wasm        -> Emscripten smoke build (Phase 4 packaging lands here)
#   make clean

CC      ?= cc
CFLAGS  ?= -O3 -march=native -Wall -Wextra -std=c11
SRC_DIR := src
BUILD   := build

CORE := \
	$(SRC_DIR)/board.c \
	$(SRC_DIR)/move_generation.c \
	$(SRC_DIR)/search.c \
	$(SRC_DIR)/perft.c

.PHONY: all test bench wasm clean

all: $(BUILD)/othello

$(BUILD)/othello: $(CORE) $(SRC_DIR)/test.c | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/tests: $(CORE) $(SRC_DIR)/tests.c | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

test: $(BUILD)/tests
	./$(BUILD)/tests

bench: $(BUILD)/othello
	./$(BUILD)/othello

$(BUILD):
	mkdir -p $(BUILD)

# Phase 4: a proper JS API (othello-wasm/) replaces this smoke build.
EMCC    ?= emcc
EMFLAGS ?= -O3
wasm: | $(BUILD)
	$(EMCC) $(EMFLAGS) $(CORE) $(SRC_DIR)/test.c -o $(BUILD)/othello.js

clean:
	rm -rf $(BUILD)
