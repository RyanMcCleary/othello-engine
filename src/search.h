#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include "board.h"

/* Score bounds. Scores are integers, relative to the side to move
 * (positive = good for the player about to play). */
#define SCORE_INF      1000000
#define SCORE_WIN_UNIT 10000   /* weight per disc of a decided endgame */

/* Static evaluation of a leaf, side-to-move relative. */
int evaluate(Board b);

/* Negamax with alpha-beta. Returns the score of `b` for the side to move. */
int negamax(Board b, int depth, int alpha, int beta);

/* Best legal move (single-bit bitboard) for the side to move, or 0 if none. */
bitboard best_move(Board b, int depth);

#endif
