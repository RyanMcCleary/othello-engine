#include "bitboard.h"

void print_board(bitboard black, bitboard white) {
	for (uint8_t rank = 0; rank < 8; rank++) {
		for (uint8_t file = 0; file < 8; file++) {
			if (bb_ref(black, rank, file)) {
				putchar('B');
			} else if (bb_ref(white, rank, file)) {
				putchar('W');
			} else {
				putchar('*');
			}
		}
		putchar('\n');
	}
}

bitboard bb_ref(bitboard bb, uint8_t rank, uint8_t file) {
	return (bb >> (rank * 8 + file)) & 1ULL;
}

bitboard bb_set(bitboard bb, uint8_t rank, uint8_t file) {
	return bb | (((bitboard) 0x01) << (rank * 8 + file));
}

bitboard bb_flip(bitboard bb, uint8_t rank, uint8_t file) {
	return bb ^ (((bitboard) 0x01) << (rank * 8 + file));
}
