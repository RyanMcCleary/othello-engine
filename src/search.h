#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include <stddef.h>
#include <stdbool.h>
#include "move_generation.h"

enum game_result {
    GAME_RESULT_WIN,
    GAME_RESULT_LOSS,
    GAME_RESULT_DRAW,
    GAME_RESULT_IN_PROGRESS
};

float minmax(bitboard opponent, bitboard player, bool maximizing, size_t depth);

float alphabeta(bitboard opponent, bitboard player, bool maximizing, size_t depth,
        float alpha, float beta);

bitboard best_move(bitboard opponent, bitboard player, size_t depth);

float hueristic_eval(bitboard opponent, bitboard player);

#endif