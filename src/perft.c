#include "perft.h"

static bitboard pop_ls1b(bitboard *bb) {
    bitboard lsb = *bb & (~*bb + 1);
    *bb &= *bb - 1;
    return lsb;
}

uint64_t perft(Board b, int depth) {
    if (depth == 0) {
        return 1;
    }

    bitboard moves = board_moves(b);
    if (moves == 0) {
        Board passed = board_pass(b);
        if (board_moves(passed) == 0) {
            return 1; /* game over: a single leaf */
        }
        return perft(passed, depth); /* pass: no ply consumed */
    }

    uint64_t nodes = 0;
    for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
        nodes += perft(board_play(b, move), depth - 1);
    }
    return nodes;
}
