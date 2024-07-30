#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>

typedef uint64_t bitboard;

struct board {
	bitboard black;
	bitboard white;
};

bitboard bb_ref(bitboard bb, uint8_t rank, uint8_t file);

bitboard bb_set(bitboard bb, uint8_t rank, uint8_t file);

bitboard bb_flip(bitboard bb, uint8_t rank, uint8_t file);

void print_board(bitboard black, bitboard white);

#endif