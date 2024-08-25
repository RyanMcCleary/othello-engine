#include "search.h"
#include <stdio.h>

int main(int argc, char **argv) {
	bitboard black = square_mask(3, 3) | square_mask(4, 4);
    bitboard white = square_mask(3, 4) | square_mask(4, 3);
	print_board(black, white);
    bitboard move = best_move(white, black, 1);
    puts("\n--------\n");
    print_board(move, 0);
	return 0;
}