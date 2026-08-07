/*
 * C ABI exposed to JavaScript. Boards are passed as two bitboards (black,
 * white) plus a side-to-move flag; with -sWASM_BIGINT these map to JS BigInt.
 * Move squares are 0..63 (a1=0, h8=63), or -1 for "no move".
 */
#include <stdint.h>
#include "board.h"
#include "search.h"
#include "tt.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define API EMSCRIPTEN_KEEPALIVE
#else
#define API
#endif

static Board make_board(uint64_t black, uint64_t white, int black_to_move) {
    Board b;
    if (black_to_move) {
        b.player = black;
        b.opponent = white;
    } else {
        b.player = white;
        b.opponent = black;
    }
    return b;
}

/* board_play applies a move and swaps sides, so after the move the side that
 * moved is the new `opponent`. The result is stashed for the getters below. */
static uint64_t g_black, g_white;

API uint64_t othello_initial_black(void) { return board_initial().player; }
API uint64_t othello_initial_white(void) { return board_initial().opponent; }

API uint64_t othello_legal_moves(uint64_t black, uint64_t white, int btm) {
    return board_moves(make_board(black, white, btm));
}

API int othello_evaluate(uint64_t black, uint64_t white, int btm) {
    return evaluate(make_board(black, white, btm));
}

/* Choose a move. time_ms > 0 -> time-bounded iterative deepening to at most
 * `depth`; otherwise a fixed-depth search with the exact endgame solver.
 * Returns the move square, or -1 if the side to move must pass. */
API int othello_best_move(uint64_t black, uint64_t white, int btm,
                          int depth, int time_ms) {
    Board b = make_board(black, white, btm);
    if (board_moves(b) == 0) {
        return -1;
    }
    bitboard mv;
    if (time_ms > 0) {
        iterative_search_timed(b, depth, (double)time_ms, &mv);
    } else {
        mv = best_move(b, depth);
    }
    return mv ? (int)__builtin_ctzll(mv) : -1;
}

/* Apply move `sq` and stash the resulting board; read via the getters. */
API void othello_apply(uint64_t black, uint64_t white, int btm, int sq) {
    Board b = make_board(black, white, btm);
    Board after = board_play(b, UINT64_C(1) << sq);
    uint64_t mover = after.opponent;  /* the side that just moved */
    uint64_t other = after.player;
    if (btm) {
        g_black = mover;
        g_white = other;
    } else {
        g_white = mover;
        g_black = other;
    }
}

API uint64_t othello_result_black(void) { return g_black; }
API uint64_t othello_result_white(void) { return g_white; }

/* popcount helpers so the UI can score without reimplementing it in JS. */
API int othello_count(uint64_t bb) { return (int)count_ones(bb); }
