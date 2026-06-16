#include "search.h"
#include "tt.h"

uint64_t search_nodes = 0;

/* ---- helpers ---------------------------------------------------------- */

static inline int square_index(bitboard single) {
    return __builtin_ctzll(single);
}

static inline bitboard pop_ls1b(bitboard *bb) {
    bitboard lsb = *bb & (~*bb + 1);
    *bb &= *bb - 1;
    return lsb;
}

static int final_score(Board b) {
    return board_disc_diff(b) * SCORE_WIN_UNIT;
}

int evaluate(Board b) {
    /* Phase 0 placeholder eval (replaced by positional eval in Phase 3). */
    int player_moves   = (int)count_ones(all_moves(b.opponent, b.player));
    int opponent_moves = (int)count_ones(all_moves(b.player, b.opponent));
    return player_moves - opponent_moves;
}

/* Static square weights used only for move ordering (corners best, X/C-squares
 * worst). This is a cheap proxy until eval-based ordering arrives. */
static const int ORDER_WEIGHT[64] = {
    100, -20,  10,   5,   5,  10, -20, 100,
    -20, -50,  -2,  -2,  -2,  -2, -50, -20,
     10,  -2,  -1,  -1,  -1,  -1,  -2,  10,
      5,  -2,  -1,  -1,  -1,  -1,  -2,   5,
      5,  -2,  -1,  -1,  -1,  -1,  -2,   5,
     10,  -2,  -1,  -1,  -1,  -1,  -2,  10,
    -20, -50,  -2,  -2,  -2,  -2, -50, -20,
    100, -20,  10,   5,   5,  10, -20, 100,
};

typedef struct { bitboard move; int score; } ScoredMove;

/* Expand `moves` into a list sorted by descending ordering score; the TT move
 * (if any) is forced to the front. */
static int order_moves(bitboard moves, bitboard tt_move, ScoredMove *out) {
    int n = 0;
    while (moves) {
        bitboard mv = pop_ls1b(&moves);
        int s = ORDER_WEIGHT[square_index(mv)];
        if (mv == tt_move) {
            s += 1000000;
        }
        out[n].move = mv;
        out[n].score = s;
        n++;
    }
    for (int i = 1; i < n; i++) {            /* insertion sort, n is small */
        ScoredMove t = out[i];
        int j = i - 1;
        while (j >= 0 && out[j].score < t.score) {
            out[j + 1] = out[j];
            j--;
        }
        out[j + 1] = t;
    }
    return n;
}

/* ---- reference search ------------------------------------------------- */

int negamax(Board b, int depth, int alpha, int beta) {
    search_nodes++;
    bitboard moves = board_moves(b);
    if (moves == 0) {
        Board passed = board_pass(b);
        if (board_moves(passed) == 0) {
            return final_score(b);
        }
        return -negamax(passed, depth, -beta, -alpha);
    }
    if (depth <= 0) {
        return evaluate(b);
    }
    int best = -SCORE_INF;
    for (bitboard m = pop_ls1b(&moves); m; m = pop_ls1b(&moves)) {
        int score = -negamax(board_play(b, m), depth - 1, -beta, -alpha);
        if (score > best) best = score;
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }
    return best;
}

/* ---- principal variation search with transposition table -------------- */

/* Below this depth the hashing + sort overhead outweighs the pruning benefit
 * (the frontier holds the vast majority of nodes), so we skip the TT there and
 * just run plain alpha-beta in natural order. */
#ifndef TT_MIN_DEPTH
#define TT_MIN_DEPTH 2
#endif

static int pvs(Board b, int depth, int alpha, int beta) {
    search_nodes++;
    bitboard moves = board_moves(b);
    if (moves == 0) {
        Board passed = board_pass(b);
        if (board_moves(passed) == 0) {
            return final_score(b);
        }
        return -pvs(passed, depth, -beta, -alpha);
    }
    if (depth <= 0) {
        return evaluate(b);
    }

    /* Shallow nodes: cheap path, no hashing, no sort. */
    if (depth < TT_MIN_DEPTH) {
        int best = -SCORE_INF;
        for (bitboard m = pop_ls1b(&moves); m; m = pop_ls1b(&moves)) {
            int score = -pvs(board_play(b, m), depth - 1, -beta, -alpha);
            if (score > best) best = score;
            if (best > alpha) alpha = best;
            if (alpha >= beta) break;
        }
        return best;
    }

    uint64_t key = board_hash(b);
    TTEntry *e = tt_get(key);
    bitboard tt_move = 0;
    if (e->key == key && e->kind == TT_KIND_MID) {
        if (e->move_sq != TT_NO_MOVE) {
            tt_move = UINT64_C(1) << e->move_sq;
        }
        if (e->depth >= depth) {
            if (e->flag == TT_EXACT) return e->score;
            if (e->flag == TT_LOWER && e->score >= beta) return e->score;
            if (e->flag == TT_UPPER && e->score <= alpha) return e->score;
        }
    }

    ScoredMove list[64];
    int n = order_moves(moves, tt_move, list);

    int alpha_orig = alpha;
    int best = -SCORE_INF;
    bitboard best_mv = list[0].move;
    for (int i = 0; i < n; i++) {
        Board child = board_play(b, list[i].move);
        int score;
        if (i == 0) {
            score = -pvs(child, depth - 1, -beta, -alpha);
        } else {
            score = -pvs(child, depth - 1, -alpha - 1, -alpha);
            if (score > alpha && score < beta) {
                score = -pvs(child, depth - 1, -beta, -alpha);
            }
        }
        if (score > best) {
            best = score;
            best_mv = list[i].move;
        }
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }

    int flag = best <= alpha_orig ? TT_UPPER : best >= beta ? TT_LOWER : TT_EXACT;
    tt_store(key, depth, best, flag, (uint8_t)square_index(best_mv), TT_KIND_MID);
    return best;
}

/* One root iteration: PVS over the legal moves, returns the best move + score. */
static int search_root(Board b, int depth, bitboard *best_out) {
    bitboard moves = board_moves(b);

    uint64_t key = board_hash(b);
    TTEntry *e = tt_get(key);
    bitboard tt_move = (e->key == key && e->kind == TT_KIND_MID &&
                        e->move_sq != TT_NO_MOVE)
                           ? (UINT64_C(1) << e->move_sq) : 0;

    ScoredMove list[64];
    int n = order_moves(moves, tt_move, list);

    int alpha = -SCORE_INF, beta = SCORE_INF, best = -SCORE_INF;
    bitboard best_mv = list[0].move;
    for (int i = 0; i < n; i++) {
        Board child = board_play(b, list[i].move);
        int score;
        if (i == 0) {
            score = -pvs(child, depth - 1, -beta, -alpha);
        } else {
            score = -pvs(child, depth - 1, -alpha - 1, -alpha);
            if (score > alpha) {
                score = -pvs(child, depth - 1, -beta, -alpha);
            }
        }
        if (score > best) {
            best = score;
            best_mv = list[i].move;
        }
        if (best > alpha) alpha = best;
    }

    tt_store(key, depth, best, TT_EXACT, (uint8_t)square_index(best_mv),
             TT_KIND_MID);
    *best_out = best_mv;
    return best;
}

int iterative_search(Board b, int max_depth, bitboard *best) {
    *best = 0;
    if (board_moves(b) == 0) {
        return evaluate(b);
    }
    int score = 0;
    for (int d = 1; d <= max_depth; d++) {
        score = search_root(b, d, best);
    }
    return score;
}

/* ---- exact endgame solver --------------------------------------------- */

/* Above this many empties the endgame TT pays for itself; below it the raw
 * recursion is cheaper than hashing. */
#ifndef ENDGAME_TT_MIN
#define ENDGAME_TT_MIN 8
#endif

int solve_endgame(Board b, int alpha, int beta) {
    search_nodes++;
    bitboard moves = board_moves(b);
    if (moves == 0) {
        Board passed = board_pass(b);
        if (board_moves(passed) == 0) {
            return board_disc_diff(b);
        }
        return -solve_endgame(passed, -beta, -alpha);
    }

    int empties = 64 - (int)count_ones(b.player | b.opponent);
    int use_tt = empties >= ENDGAME_TT_MIN;
    uint64_t key = 0;
    bitboard tt_move = 0;
    if (use_tt) {
        key = board_hash(b);
        TTEntry *e = tt_get(key);
        if (e->key == key && e->kind == TT_KIND_END) {
            if (e->move_sq != TT_NO_MOVE) tt_move = UINT64_C(1) << e->move_sq;
            /* Solver scores are fully resolved, so any bound is depth-agnostic. */
            if (e->flag == TT_EXACT) return e->score;
            if (e->flag == TT_LOWER && e->score >= beta) return e->score;
            if (e->flag == TT_UPPER && e->score <= alpha) return e->score;
        }
    }

    ScoredMove list[64];
    int n = order_moves(moves, tt_move, list);
    int alpha_orig = alpha;
    int best = -SCORE_INF;
    bitboard best_mv = list[0].move;
    for (int i = 0; i < n; i++) {
        int score = -solve_endgame(board_play(b, list[i].move), -beta, -alpha);
        if (score > best) {
            best = score;
            best_mv = list[i].move;
        }
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }

    if (use_tt) {
        int flag = best <= alpha_orig ? TT_UPPER : best >= beta ? TT_LOWER : TT_EXACT;
        tt_store(key, empties, best, flag, (uint8_t)square_index(best_mv),
                 TT_KIND_END);
    }
    return best;
}

static bitboard solve_root(Board b) {
    bitboard moves = board_moves(b);
    if (moves == 0) {
        return 0;
    }
    ScoredMove list[64];
    int n = order_moves(moves, 0, list);
    int alpha = -SCORE_INF, best = -SCORE_INF;
    bitboard best_mv = list[0].move;
    for (int i = 0; i < n; i++) {
        int score = -solve_endgame(board_play(b, list[i].move), -SCORE_INF, -alpha);
        if (score > best) {
            best = score;
            best_mv = list[i].move;
        }
        if (best > alpha) alpha = best;
    }
    return best_mv;
}

/* ---- public entry point ----------------------------------------------- */

bitboard best_move(Board b, int depth) {
    if (board_moves(b) == 0) {
        return 0;
    }
    int empties = 64 - (int)count_ones(b.player | b.opponent);
    if (empties <= ENDGAME_EMPTIES) {
        return solve_root(b);
    }
    bitboard best;
    iterative_search(b, depth, &best);
    return best;
}
