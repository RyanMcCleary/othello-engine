#include "bitboard.h"
#include <stdio.h>

int main(int argc, char **argv) {
	bitboard black = 0, white = 0;
	black = bb_set(black, 3, 3);
	black = bb_set(black, 4, 4);
	white = bb_set(white, 3, 4);
	white = bb_set(white, 4, 3);
	print_board(black, white);
	printf("black = 0x%016lx, white = 0x%016lx\n", black, white);
	bitboard move1 = 0x0000000000100000ULL;
	bitboard flipped = flip_south(white, black, move1);
	black |= flipped | move1;
	printf("flip_south: 0x%016lx\n", flipped);
	white &= ~black;
	print_board(black, white);
	bitboard move2 = 0x0000000000200000ULL;
	flipped = flip_southwest(black, white, move2);
	white |= flipped | move2;
	printf("flip_southwest: 0x%016lx\n", flipped);
	black &= ~white;
	print_board(black, white);
	return 0;
}