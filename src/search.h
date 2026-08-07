#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include "board.h"

/* Scores are integers, side-to-move relative (positive = good for the player
 * about to move). Terminal positions score the disc differential scaled by
 * SCORE_WIN_UNIT, which stays well inside SCORE_INF (max 64 * unit). */
#define SCORE_INF      1000000
#define SCORE_WIN_UNIT 10000

/* Switch to the exact endgame solver at or below this many empty squares. */
#ifndef ENDGAME_EMPTIES
#define ENDGAME_EMPTIES 12
#endif

/* Count of nodes visited by the most recent search (for benchmarking / time
 * control). Reset by the caller as needed. */
extern uint64_t search_nodes;

int evaluate(Board b);            /* hand-crafted phase-blended eval */
int evaluate_mobility(Board b);   /* mobility-only reference eval     */

/* Leaf evaluator used by the search (defaults to `evaluate`). Swappable so two
 * evals can be matched against each other in self-play. */
extern int (*search_eval)(Board b);

/* Plain negamax + alpha-beta. Reference implementation: the PVS+TT search must
 * return the identical value for the same depth (it is an exact optimization). */
int negamax(Board b, int depth, int alpha, int beta);

/* Iterative-deepening PVS with transposition table and move ordering. Returns
 * the score and writes the chosen move (single-bit bitboard) to *best. */
int iterative_search(Board b, int max_depth, bitboard *best);

/* As above, but stop once `time_ms` of search has elapsed (checked between and
 * within iterations); the move from the last fully completed depth is kept.
 * time_ms <= 0 means no time limit. */
int iterative_search_timed(Board b, int max_depth, double time_ms,
                           bitboard *best);

/* Exact endgame: best disc differential to the end of the game. */
int solve_endgame(Board b, int alpha, int beta);

/* Best move for the side to move: exact solver in the endgame, otherwise
 * iterative-deepening PVS to `depth`. Returns 0 if there is no legal move. */
bitboard best_move(Board b, int depth);

#endif
