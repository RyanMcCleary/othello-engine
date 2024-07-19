#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "game_state.h"
#include "debug.h"

bool in_bounds(int rank, int file);

enum square_state opposite_color(enum player current_player);

enum square_state current_color(enum player current_player);

bool is_valid_direction(enum square_state **board, enum player current_player,
				     int rank, int file, int rank_delta, int file_delta) {
    if (!in_bounds(rank, file) ||
        !in_bounds(rank + rank_delta, file + file_delta)) {
        return false;
    }
    if (board[rank + rank_delta][file + file_delta] != opposite_color(current_player)) {
        return false;
    }
    rank += rank_delta;
    file += file_delta;
    while (in_bounds(rank, file) &&
           board[rank][file] == opposite_color(current_player)) {
        rank += rank_delta;
        file += file_delta;
    }
    if (in_bounds(rank, file) &&
        board[rank][file] == current_color(current_player)) {
        return true;
    }
    else {
        return false;
    }
}

bool in_bounds(int rank, int file) {
    return (0 <= rank && rank < 8 && 0 <= file && file < 8);
}

enum square_state current_color(enum player current_player) {	
    if(current_player == PLAYER_BLACK) {
        return SQUARE_BLACK;
    }
    else {
        return SQUARE_WHITE;
    }
}

enum square_state opposite_color(enum player current_player) {	
    if (current_player == PLAYER_BLACK) {
        return SQUARE_WHITE;
    }
    else {
        return SQUARE_BLACK;
    }
}

bool flip_direction(enum square_state **board, enum player current_player,
			        int rank, int file, int rank_delta, int file_delta) {
    if (!is_valid_direction(board, current_player, rank, file, rank_delta, file_delta)) {
        return false;
    }
    rank += rank_delta;
    file += file_delta;
    while (in_bounds(rank, file) &&
           board[rank][file] == opposite_color(current_player)) {
        board[rank][file] = current_color(current_player);
        rank += rank_delta;
        file += file_delta;
    }
    return true;
}

bool is_valid_move(enum square_state **board, enum player current_player,
	            int rank, int file) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0) && is_valid_direction(board, current_player, rank, file, i, j)) {
                return true;
            }
        }
    }
    return false;
}

void make_move(enum square_state **board, enum player current_player, int rank, int file) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0)) flip_direction(board, current_player, rank, file, i, j);
        }
    }
	board[rank][file] = current_color(current_player);
}

void square_index_set(struct square_index *index, int rank, int file) {
	index->rank = rank;
	index->file = file;
}

size_t list_moves(enum square_state **board, enum player current_player, 
                             struct square_index *output_array) {
    size_t out_idx = 0;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (is_valid_move(board, current_player, rank, file)) {
                if (output_array != NULL) {
					square_index_set(&output_array[out_idx], rank, file);
				}
				out_idx++;
            }
        }
    }
    return out_idx;
}

size_t count_moves(enum square_state **board, enum player current_player) {
	return list_moves(board, current_player, NULL);
}

enum game_result check_win(enum square_state **board) {
	int black_disks = 0;
    int white_disks = 0;    
	int black_moves = count_moves(board, PLAYER_BLACK);
	int white_moves = count_moves(board, PLAYER_WHITE);
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (board[rank][file] == SQUARE_BLACK) {
                black_disks++;
            }
            else if (board[rank][file] == SQUARE_WHITE) {
                white_disks++;
            }
        }
    }
	enum game_result disk_count_result;
	if (black_disks > white_disks) {
		disk_count_result = GAME_RESULT_BLACK_WIN;
	}
	else if (white_disks > black_disks) {
		disk_count_result = GAME_RESULT_WHITE_WIN;
	}
	else {
		disk_count_result = GAME_RESULT_TIE;
	}
	
	if (black_moves == 0 && white_moves == 0) {
		return disk_count_result;
	}
	else {
		return GAME_RESULT_IN_PROGRESS;
	}
}

void copy_make_move(enum square_state **src_board, enum square_state **dst_board,
			        enum player current_player, struct square_index index) {
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			dst_board[rank][file] = src_board[rank][file];
		}
	}
	make_move(dst_board, current_player, index.rank, index.file);
}

enum square_state **board_alloc(void)  {
	enum square_state **board = malloc(sizeof(enum square_state *) * 8);
	for (int i = 0; i < 8; i++) {
		board[i] = malloc(sizeof(enum square_state) * 8);
	}
	return board;
}

enum player other_player(enum player current_player) {
	if (current_player == PLAYER_BLACK) {
		return PLAYER_WHITE;
	}
	else {
		return PLAYER_BLACK;
	}
}
