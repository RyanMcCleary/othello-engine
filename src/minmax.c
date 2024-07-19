#include "minmax.h"

float max(float a, float b) {
	return (a > b) ? a : b;
}

float min(float a, float b) {
	return (a < b) ? a : b;
}

float hueristic_eval(enum square_state **board, enum player current_player) {
	int moves = count_moves(board, current_player);
	int opponent_moves = count_moves(board, other_player(current_player));
	return (moves - opponent_moves) / ((float)(moves + opponent_moves));
}

float minmax_helper(enum square_state **board, enum square_state **board_copy,
			 enum player current_player, enum player max_player, size_t depth) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board, current_player);
	}
	struct square_index moves[64];
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == max_player) {
		float value = -1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = max(value, minmax(board_copy, other_player(current_player),
				max_player, depth - 1));
		}
		return value;
	}
	else {
		float value = 1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = min(value, minmax(board_copy, other_player(current_player), max_player, depth - 1));
		}
		return value;
	}
}

float minmax(enum square_state **board, enum player current_player,
			 enum player max_player, size_t depth) {
	enum square_state **board_copy = board_alloc();
	return minmax_helper(board, board_copy, current_player, max_player, depth);
	board_free(board_copy);
}

struct square_index best_move(enum square_state **board, enum player current_player,
		                      size_t depth) {
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t best_idx = 0, num_moves = list_moves(board, current_player, moves);
	float best_eval = -1.0f;
	for (size_t i = 0; i < num_moves; i++) {
		copy_make_move(board, board_copy, current_player, moves[i]);
		float minmax_eval = minmax(board, current_player, current_player, depth);
		if (minmax_eval > best_eval) {
			best_idx = i;
			best_eval = minmax_eval;
		}
	}
	return moves[best_idx];
}