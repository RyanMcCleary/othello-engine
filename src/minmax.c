#include "minmax.h"

float max(float a, float b) {
	return (a > b) ? a : b;
}

float min(float a, float b) {
	return (a < b) ? a : b;
}

float mobility_hueristic(enum square_state **board, enum player current_player) {
	int moves = count_moves(board, current_player);
	int opponent_moves = count_moves(board, other_player(current_player));
	return (moves - opponent_moves) / ((float)(moves + opponent_moves));
}

struct square_index corner_table[][] = {
	{ { .rank = 0, .file = 0}, { .rank = 0, .file = 1}, { .rank = 1, .file = 0},
		{ .rank = 1, .file = 1} },
	{ { .rank = 0, .file = 7}, { .rank = 1, .file = 7}, { .rank = 0, .file = 6},
		{ .rank = 1, .file = 6}},
	{ { .rank = 7, .file = 0}, { .rank = 6, .file = 0}, { .rank = 7, .file = 1},
		{ .rank = 6, .file = 1}},
	{ { .rank = 7, .file = 7}, { .rank = 6, .file = 7}, { .rank = 7, .file = 6},
		{ .rank = 6, .file = 6}}};

float corners_hueristic(enum square_state **board) {
	float result = 0.0;
	for (int i = 0; i < 4; i++) {
		
	}
}


float minmax(enum square_state **board,
			 enum player current_player, enum player max_player, size_t depth) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board, current_player);
	}
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == max_player) {
		float value = -1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = max(value, minmax(board_copy, other_player(current_player),
				max_player, depth - 1));
		}
		board_free(board_copy);
		return value;
	}
	else {
		float value = 1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = min(value, minmax(board_copy, other_player(current_player),
				max_player, depth - 1));
		}
		board_free(board_copy);
		return value;
	}
}

float alphabeta(enum square_state **board, enum player current_player,
			    enum player max_player, size_t depth, float alpha, float beta) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board, current_player);
	}
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == max_player) {
		float value = -1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = max(value, alphabeta(board_copy, other_player(current_player),
				max_player, depth - 1, alpha, beta));
			alpha = max(value, alpha);
			if (value >= beta) {
				break;
			}
		}
		board_free(board_copy);
		return value;
	}
	else {
		float value = 1.0f;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = min(value, alphabeta(board_copy, other_player(current_player),
				max_player, depth - 1, alpha, beta));
			beta = min(beta, value);
			if (value <= alpha) {
				break;
			}
		}
		board_free(board_copy);
		return value;
	}
}

struct square_index best_move(enum square_state **board, enum player current_player,
		                      size_t depth) {
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t best_idx = 0, num_moves = list_moves(board, current_player, moves);
	float best_eval = -1.0f;
	for (size_t i = 0; i < num_moves; i++) {
		copy_make_move(board, board_copy, current_player, moves[i]);
		float alphabeta_eval = alphabeta(board, current_player, current_player, depth, -1.0, 1.0);
		if (alphabeta_eval > best_eval) {
			best_idx = i;
			best_eval = alphabeta_eval;
		}
	}
	board_free(board_copy);
	return moves[best_idx];
}