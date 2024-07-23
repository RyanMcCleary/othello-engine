#ifndef MINMAX_H
#define MINMAX_H

#include <stddef.h>
#include "game_state.h"

float minmax(enum square_state **board, enum player current_player, size_t depth);

struct square_index best_move(enum square_state **board, enum player current_player,
	size_t depth);

float hueristic_eval(enum square_state **board);

float alphabeta(enum square_state **board, enum player current_player,
			    size_t depth, float alpha, float beta);

#endif