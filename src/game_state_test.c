#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "game_state.h"
#include "minmax.h"

int othello_assert(bool condition, const char *msg) {
    if (!condition) {
        fprintf(stderr, "%s test FAILED. :(\n", msg);
        return 0;
    } else {
        printf("%s test PASSED! :)\n", msg);
        return 1;
    }
}

void print_board(enum square_state **board) {
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			if (board[rank][file] == SQUARE_BLACK) putchar('B');
			else if (board[rank][file] == SQUARE_WHITE) putchar('W');
			else putchar('*');
		}
		putchar('\n');
	}
}

void board_init(enum square_state **board) {
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			board[rank][file] = SQUARE_EMPTY;
		}
	}
	board[3][3] = board[4][4] = SQUARE_BLACK;
	board[3][4] = board[4][3] = SQUARE_WHITE;
}

void board_free(enum square_state **board) {
	for (int i = 0; i < 8; i++) {
		free(board[i]);
	}
	free(board);
}

void print_check_win(enum square_state **board) {
	printf("check_win => ");
	enum game_result result = check_win(board);
	if (result == GAME_RESULT_BLACK_WIN) {
		puts("GAME_RESULT_BLACK_WIN");
	}
	else if (result == GAME_RESULT_WHITE_WIN) {
		puts("GAME_RESULT_WHITE_WIN");
	}
	else if (result == GAME_RESULT_TIE) {
		puts("GAME_RESULT_TIE");
	}
	else {
		puts("GAME_RESULT_IN_PROGRESS");
	}
}

void print_moves(enum square_state **board) {
	struct square_index moves[64];
	puts("White moves:");
	size_t num_white_moves = list_moves(board, PLAYER_WHITE, moves);
	for (size_t i = 0; i < num_white_moves; i++) {
		printf("(%d, %d)\n", moves[i].rank, moves[i].file);
	}
	puts("Black moves:");
	size_t num_black_moves = list_moves(board, PLAYER_BLACK, moves);
	for (size_t i = 0; i < num_black_moves; i++) {
		printf("(%d, %d)\n", moves[i].rank, moves[i].file);
	}
	
}

void print_square_index(struct square_index square) {
	printf("(%d, %d)\n", square.rank, square.file);
}

int main(int argc, char **argv) {
	enum square_state **board = board_alloc();
	board_init(board);

	while (1) {
		print_board(board);
		int rank, file;
		printf("Enter move: ");
		if (scanf("%d %d", &rank, &file) != 2) {
			exit(1);
		}
		make_move(board, PLAYER_BLACK, rank, file);
		puts("Board after making move:");
		print_board(board);
		puts("\nSearching for a move...");
		struct square_index engine_move = best_move(board, PLAYER_WHITE, 8);
		make_move(board, PLAYER_WHITE, engine_move.rank, engine_move.file);
		printf("%f\n", alphabeta(board, PLAYER_WHITE, 8, -10.0, 10.0));
	}
	board_free(board);
    return 0;
}

