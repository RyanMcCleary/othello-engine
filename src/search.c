#include "search.h"

static bitboard pop_ls1b(bitboard *bb) {
    bitboard lsb = *bb & (~*bb + 1);
    *bb &= *bb - 1;
    return lsb;
}

/* Exact score of a finished game, side-to-move relative. */
static int final_score(Board b) {
    return board_disc_diff(b) * SCORE_WIN_UNIT;
}

int evaluate(Board b) {
    /* Phase 0 placeholder: side-to-move-relative mobility difference.
     * Replaced by a phase-blended positional eval in Phase 3. */
    int player_moves   = (int)count_ones(all_moves(b.opponent, b.player));
    int opponent_moves = (int)count_ones(all_moves(b.player, b.opponent));
    return player_moves - opponent_moves;
}

int negamax(Board b, int depth, int alpha, int beta) {
    bitboard moves = board_moves(b);

    if (moves == 0) {
        /* No move: pass unless the opponent is also stuck (game over). */
        Board passed = board_pass(b);
        if (board_moves(passed) == 0) {
            return final_score(b);
        }
        /* A pass does not consume search depth (you cannot pass twice). */
        return -negamax(passed, depth, -beta, -alpha);
    }

    if (depth <= 0) {
        return evaluate(b);
    }

    int best = -SCORE_INF;
    for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
        int score = -negamax(board_play(b, move), depth - 1, -beta, -alpha);
        if (score > best) {
            best = score;
        }
        if (best > alpha) {
            alpha = best;
        }
        if (alpha >= beta) {
            break;
        }
    }
    return best;
}

bitboard best_move(Board b, int depth) {
    bitboard moves = board_moves(b);
    bitboard best = 0;
    int best_score = -SCORE_INF;

    for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
        int score = -negamax(board_play(b, move), depth - 1, -SCORE_INF, SCORE_INF);
        if (score > best_score) {
            best_score = score;
            best = move;
        }
    }
    return best;
}
