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

bitboard flip_north(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_south(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_east(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_west(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_northeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_southeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_northwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_southwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

void print_board(bitboard black, bitboard white);

#endif