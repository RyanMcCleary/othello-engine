#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdbool.h>
#include "move_generation.h"

/*
 * A Board is always described from the point of view of the side to move:
 * `player` holds the discs of the side about to play, `opponent` the other
 * side. This keeps the search negamax-friendly -- after a move the two
 * bitboards are swapped so the child's `player` is the next side to move.
 */
typedef struct {
    bitboard player;
    bitboard opponent;
} Board;

/* Standard Othello starting position, Black to move. */
Board board_initial(void);

/* Legal moves (as a bitboard of target squares) for the side to move. */
bitboard board_moves(Board b);

bool board_has_moves(Board b);

/* Neither side has a legal move -> the game is over. */
bool board_terminal(Board b);

/* Apply `move` (a single-bit bitboard) for the side to move and swap sides. */
Board board_play(Board b, bitboard move);

/* Pass the turn without placing a disc (only legal when no moves exist). */
Board board_pass(Board b);

/* Disc count, side-to-move minus opponent. */
int board_disc_diff(Board b);

#endif
