#include "bitboard.h"

int main(int argc, char **argv) {
	bitboard black = 0, white = 0;
	black = bb_set(black, 3, 3);
	black = bb_set(black, 4, 4);
	white = bb_set(white, 3, 4);
	white = bb_set(white, 4, 3);
	print_board(black, white);
	return 0;
}