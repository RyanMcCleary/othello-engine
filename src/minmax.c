#include "minmax.h"

#define EVAL_MAX 10.0f
#define EVAL_MIN -10.0f

float max(float a, float b) {
	return (a > b) ? a : b;
}

float min(float a, float b) {
	return (a < b) ? a : b;
}

float mobility_hueristic(enum square_state **board) {
	int white_moves = count_moves(board, PLAYER_WHITE);
	int black_moves = count_moves(board, PLAYER_BLACK);
	return (white_moves - black_moves) / ((float)(white_moves + black_moves));
}

struct square_index corner_table[4][4] = {
	{ { .rank = 0, .file = 0}, { .rank = 0, .file = 1}, { .rank = 1, .file = 0},
		{ .rank = 1, .file = 1} },
	{ { .rank = 0, .file = 7}, { .rank = 1, .file = 7}, { .rank = 0, .file = 6},
		{ .rank = 1, .file = 6}},
	{ { .rank = 7, .file = 0}, { .rank = 6, .file = 0}, { .rank = 7, .file = 1},
		{ .rank = 6, .file = 1}},
	{ { .rank = 7, .file = 7}, { .rank = 6, .file = 7}, { .rank = 7, .file = 6},
		{ .rank = 6, .file = 6}}};

float corner_hueristic(enum square_state **board) {
	float result = 0.0;
	for (int i = 0; i < 4; i++) {
		int r0 = corner_table[i][0].rank, f0 = corner_table[i][0].file;
		if (board[r0][f0] == SQUARE_WHITE) {
			result += 1.0;
		}
		else if (board[r0][f0] == SQUARE_BLACK) {
			result -= 1.0;
		}
		int r1 = corner_table[i][1].rank, f1 = corner_table[i][1].file;
		int r2 = corner_table[i][2].rank, f2 = corner_table[i][2].file;
		if (board[r1][f1] == SQUARE_WHITE) {
			result -= 0.5;
		}
		else if (board[r1][f1] == SQUARE_BLACK) {
			result += 0.5;
		}
		if (board[r2][f2] == SQUARE_WHITE) {
			result -= 0.5;
		}
		else if (board[r2][f2] == SQUARE_BLACK) {
			result += 0.5;
		}
		int r3 = corner_table[i][3].rank, f3 = corner_table[i][3].file;
		if (board[r3][f3] == SQUARE_WHITE) {
			result -= 0.75;
		}
		else if (board[r3][f3] == SQUARE_BLACK) {
			result += 0.75;
		}
	}
	return result;
}

float hueristic_eval(enum square_state **board) {
	return mobility_hueristic(board) + corner_hueristic(board);
}

float minmax(enum square_state **board,
			 enum player current_player, size_t depth) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board);
	}
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == PLAYER_WHITE) {
		float value = EVAL_MIN;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = max(value, minmax(board_copy, other_player(current_player),
				depth - 1));
		}
		board_free(board_copy);
		return value;
	}
	else {
		float value = EVAL_MAX;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = min(value, minmax(board_copy, other_player(current_player),
				depth - 1));
		}
		board_free(board_copy);
		return value;
	}
}

float alphabeta(enum square_state **board, enum player current_player,
			    size_t depth, float alpha, float beta) {
	if (depth < 1 || check_win(board) != GAME_RESULT_IN_PROGRESS) {
		return hueristic_eval(board);
	}
	struct square_index moves[64];
	enum square_state **board_copy = board_alloc();
	size_t num_moves = list_moves(board, current_player, moves);
	if (current_player == PLAYER_WHITE) {
		float value = EVAL_MIN;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = max(value, alphabeta(board_copy, other_player(current_player),
				depth - 1, alpha, beta));
			alpha = max(value, alpha);
			if (value >= beta) {
				break;
			}
		}
		board_free(board_copy);
		return value;
	}
	else {
		float value = EVAL_MAX;
		for (size_t i = 0; i < num_moves; i++) {
			copy_make_move(board, board_copy, current_player, moves[i]);
			value = min(value, alphabeta(board_copy, other_player(current_player),
				depth - 1, alpha, beta));
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
	float best_eval = EVAL_MIN;
	for (size_t i = 0; i < num_moves; i++) {
		copy_make_move(board, board_copy, current_player, moves[i]);
		float alphabeta_eval = alphabeta(board_copy, current_player, depth, -10.0, 10.0);
		if (alphabeta_eval > best_eval) {
			best_idx = i;
			best_eval = alphabeta_eval;
		}
	}
	board_free(board_copy);
	return moves[best_idx];
}