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
	$(SRC_DIR)/tt.c \
	$(SRC_DIR)/perft.c

.PHONY: all test bench wasm serve clean

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

# WebAssembly build: an ES module + .wasm in othello-wasm/, driven by the C ABI
# in src/wasm_api.c. Boards cross the boundary as BigInt (-sWASM_BIGINT).
EMCC    ?= emcc
WASM_DIR := othello-wasm
WASM_EXPORTS := _othello_initial_black,_othello_initial_white,_othello_legal_moves,\
_othello_evaluate,_othello_best_move,_othello_apply,_othello_result_black,\
_othello_result_white,_othello_count
EMFLAGS ?= -O3 -sWASM_BIGINT -sMODULARIZE -sEXPORT_ES6 \
	-sEXPORT_NAME=createOthello -sENVIRONMENT=web,worker,node \
	-sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
	-sEXPORTED_FUNCTIONS='$(WASM_EXPORTS)' \
	-sALLOW_MEMORY_GROWTH

wasm:
	$(EMCC) $(EMFLAGS) $(CORE) $(SRC_DIR)/wasm_api.c -o $(WASM_DIR)/othello.js

# Serve the web UI (module workers + wasm need HTTP, not file://).
# Open http://localhost:8000/  after running.
serve:
	cd $(WASM_DIR) && python3 -m http.server 8000

clean:
	rm -rf $(BUILD)
