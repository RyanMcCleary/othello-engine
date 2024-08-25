#include "search.h"
#include <stdio.h>

#define EVAL_MAX 10.0f
#define EVAL_MIN -10.0f

float max(float a, float b) {
	return (a > b) ? a : b;
}

float min(float a, float b) {
	return (a < b) ? a : b;
}

enum game_result check_win(bitboard opponent, bitboard player) {
	int player_disks = count_ones(player);
    int opponent_disks = count_ones(opponent);    
	int player_moves = count_ones(all_moves(opponent, player));
	int opponent_moves = count_ones(all_moves(player, opponent));
	if (player_moves == 0 && opponent_moves == 0) {
        if (player_disks > opponent_disks) {
            return GAME_RESULT_WIN;
        }
        else if (opponent_disks > player_disks) {
            return GAME_RESULT_LOSS;
        }
        else {
            return GAME_RESULT_DRAW;
        }
	}
	else {
		return GAME_RESULT_IN_PROGRESS;
	}
}

float mobility_hueristic(bitboard opponent, bitboard player) {
	int player_moves = count_ones(all_moves(opponent, player));
	int opponent_moves = count_ones(all_moves(player, opponent));
	return (player_moves - opponent_moves) / ((float)(player_moves + opponent_moves));
}

float hueristic_eval(bitboard opponent, bitboard player) {
	return mobility_hueristic(opponent, player);
}

bitboard pop_ls1b(bitboard *bb) {
    bitboard temp = *bb;
    *bb = *bb & (*bb - 1);
    return temp & -temp;
}

float minmax(bitboard opponent, bitboard player, bool maximizing, size_t depth) {
	if (depth < 1 || check_win(opponent, player) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(opponent, player);
	}
    bitboard moves = all_moves(opponent, player);
	if (maximizing) {
		float value = EVAL_MIN;
		for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
            bitboard flipped = flip_all(opponent, player, move);
			value = max(value, minmax(player | move | flipped, opponent & ~flipped,
                !maximizing, depth - 1));
		}
		return value;
	}
	else {
		float value = EVAL_MAX;
		for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
			bitboard flipped = flip_all(opponent, player, move);
			value = min(value, minmax(player | move | flipped, opponent & ~flipped,
                !maximizing, depth - 1));
		}
		return value;
	}
}

float alphabeta(bitboard opponent, bitboard player, bool maximizing, size_t depth,
        float alpha, float beta) {
	if (depth < 1 || check_win(opponent, player) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(opponent, player);
	}
    bitboard moves = all_moves(opponent, player);
	if (maximizing) {
		float value = EVAL_MIN;
		for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
			bitboard flipped = flip_all(opponent, player, move);
			value = max(value, alphabeta(player | move | flipped, opponent & ~flipped,
				!maximizing, depth - 1, alpha, beta));
			alpha = max(value, alpha);
			if (value >= beta) {
				break;
			}
		}
		return value;
	}
	else {
		float value = EVAL_MAX;
		for (bitboard move = pop_ls1b(&moves); move; move = pop_ls1b(&moves)) {
            bitboard flipped = flip_all(opponent, player, move);
            value = min(value, alphabeta(player | move | flipped, opponent & ~flipped,
				!maximizing, depth - 1, alpha, beta));
			beta = min(beta, value);
			if (value <= alpha) {
				break;
			}
		}
		return value;
	}
}

bitboard best_move(bitboard opponent, bitboard player, size_t depth) {
    bitboard best_move = 0, moves = all_moves(opponent, player);
	float best_eval = EVAL_MIN;
	for (bitboard move = pop_ls1b(&moves); moves; move = pop_ls1b(&moves)) {
        bitboard flipped = flip_all(opponent, player, move);
		float alphabeta_eval = alphabeta(opponent & ~flipped, player | flipped | move,
            true, depth, EVAL_MIN, EVAL_MAX);
        printf("%.4f\n", alphabeta_eval);
		if (alphabeta_eval > best_eval) {
			best_move = move;
			best_eval = alphabeta_eval;
		}
	}
	return best_move;
}

