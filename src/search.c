#include "search.h"
#include "tt.h"

uint64_t search_nodes = 0;
int (*search_eval)(Board b) = evaluate;

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

/* Reference eval kept for comparison/regression: side-to-move mobility only. */
int evaluate_mobility(Board b) {
    int player_moves   = (int)count_ones(all_moves(b.opponent, b.player));
    int opponent_moves = (int)count_ones(all_moves(b.player, b.opponent));
    return player_moves - opponent_moves;
}

#define CORNER_MASK UINT64_C(0x8100000000000081)

/* The four squares of an edge, corner-to-corner, for the 4 edges. Shared
 * corners are deduplicated because stability is accumulated into a bitboard. */
static const int EDGES[4][8] = {
    { 0,  1,  2,  3,  4,  5,  6,  7},
    {56, 57, 58, 59, 60, 61, 62, 63},
    { 0,  8, 16, 24, 32, 40, 48, 56},
    { 7, 15, 23, 31, 39, 47, 55, 63},
};

/* X-squares (diagonally adjacent to a corner) and the corner each guards. */
static const int XSQ[4]     = { 9, 14, 49, 54};
static const int XCORNER[4] = { 0,  7, 56, 63};

/* Edge-stable discs of `mine`: an outer-edge disc can only ever be flipped
 * along its edge, so a run anchored to an owned corner -- or any disc on a
 * fully occupied edge -- is permanently stable. A safe lower bound on full
 * stability, computed into a bitboard so shared corners count once. */
static bitboard edge_stable(bitboard mine, bitboard occ) {
    bitboard stable = 0;
    for (int e = 0; e < 4; e++) {
        const int *idx = EDGES[e];
        if ((mine >> idx[0]) & 1) {
            for (int i = 0; i < 8 && ((mine >> idx[i]) & 1); i++)
                stable |= UINT64_C(1) << idx[i];
        }
        if ((mine >> idx[7]) & 1) {
            for (int i = 7; i >= 0 && ((mine >> idx[i]) & 1); i--)
                stable |= UINT64_C(1) << idx[i];
        }
        int full = 1;
        for (int i = 0; i < 8; i++)
            if (!((occ >> idx[i]) & 1)) { full = 0; break; }
        if (full) {
            for (int i = 0; i < 8; i++)
                if ((mine >> idx[i]) & 1) stable |= UINT64_C(1) << idx[i];
        }
    }
    return stable;
}

/* X-squares owned by `mine` whose guarded corner is still empty (a liability). */
static int x_square_risk(bitboard mine, bitboard occ) {
    int risk = 0;
    for (int i = 0; i < 4; i++) {
        if (((mine >> XSQ[i]) & 1) && !((occ >> XCORNER[i]) & 1)) risk++;
    }
    return risk;
}

/*
 * Hand-crafted, phase-blended evaluation (no ML). All terms are side-to-move
 * relative: positive favours the player about to move. Weights interpolate from
 * an opening profile (mobility/position) to an endgame profile (stability/disc
 * count) as the board fills.
 */
int evaluate(Board b) {
    bitboard P = b.player, O = b.opponent;
    bitboard occ = P | O;
    bitboard empty = ~occ;
    int discs = (int)count_ones(occ);

    int t = (discs - 4) * 256 / 60;     /* 0 = opening .. 256 = full board */
    if (t < 0) t = 0;
    if (t > 256) t = 256;
#define BLEND(o, e) ((o) * (256 - t) / 256 + (e) * t / 256)

    int mob = (int)count_ones(all_moves(O, P)) - (int)count_ones(all_moves(P, O));
    int pot = (int)count_ones(empty & neighbors(O)) -
              (int)count_ones(empty & neighbors(P));
    int corner = (int)count_ones(P & CORNER_MASK) -
                 (int)count_ones(O & CORNER_MASK);
    int xrisk = x_square_risk(O, occ) - x_square_risk(P, occ);
    int stab = (int)count_ones(edge_stable(P, occ)) -
               (int)count_ones(edge_stable(O, occ));
    int disc = (int)count_ones(P) - (int)count_ones(O);
    int parity = ((64 - discs) & 1) ? 1 : -1;

    return BLEND(80, 0)    * mob
         + BLEND(40, 0)    * pot
         + BLEND(300, 320) * corner
         + BLEND(110, 20)  * xrisk
         + BLEND(80, 320)  * stab
         + BLEND(0, 120)   * disc
         + BLEND(0, 40)    * parity;
#undef BLEND
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
        return search_eval(b);
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
        return search_eval(b);
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
        return search_eval(b);
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
