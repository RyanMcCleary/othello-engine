#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

enum game_result {
    GAME_RESULT_BLACK_WIN = 0,
    GAME_RESULT_WHITE_WIN,
    GAME_RESULT_IN_PROGRESS,
    GAME_RESULT_TIE
};


enum square_state {
    SQUARE_EMPTY = 0, SQUARE_BLACK, SQUARE_WHITE
};

enum player {
    PLAYER_BLACK = 0, PLAYER_WHITE
};

enum player other_player(enum player current_player);

struct game_state {
    enum square_state board[8][8];
    enum player current_player;
};

struct square_index {
    int rank;
    int file;
};

enum game_result check_win(enum square_state **board); 

size_t count_moves(enum square_state **board, enum player current_player);

bool is_valid_move(enum square_state **board, enum player current_player,
			       int rank, int file);

void make_move(enum square_state **board, enum player current_player, int rank,
			   int file);

size_t list_moves(enum square_state **board, enum player current_player, 
                             struct square_index *output_array);
							 
void square_index_set(struct square_index *index, int rank, int file);

void copy_make_move(enum square_state **src_board, enum square_state **dst_board,
			        enum player current_player, struct square_index index);
					

enum square_state **board_alloc(void) ;

#endif
