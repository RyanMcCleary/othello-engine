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

float minmax(enum square_state **board, enum player current_player,
			 enum player max_player, size_t depth) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board, current_player);
	}
	struct square_index moves[64];
	enum square_state **new_board = board_alloc();
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == max_player) {
		float value = -1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, new_board, current_player, moves[i]);
			value = max(value, minmax(new_board, other_player(current_player),
				max_player, depth - 1));
		}
		return value;
	}
	else {
		float value = 1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, new_board, current_player, moves[i]);
			value = min(value, minmax(new_board, other_player(current_player), max_player, depth - 1));
		}
		return value;
	}
}
