#ifndef BITBOARD_H
#define BITBOARD_H

#include <stddef.h>

typedef bitboard uint64_t;

struct board {
	bitboard black;
	bitboard white;
};

bitboard bb_ref(bitboard bb, uint8_t rank, uint8_t file) {
	return bb >> (rank * 8 + file);
}

bitboard bb_set(bitboard bb, uint8_t rank, uint8_t file) {
	return bb | (((bitboard) 0x01) << (rank * 8 + file));
}

bitboard bb_flip(bitboard bb, uint8_t rank, uint8_t file) {
	return bb ^ (((bitboard) 0x01) << (rank * 8 + file));
}


#endif