#ifndef MOVE_GENERATION_H_INCLUDED
#define MOVE_GENERATION_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef uint64_t bitboard;

bitboard square_mask(uint8_t rank, uint8_t file);

bitboard flip_north(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_south(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_east(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_west(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_northeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_southeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_northwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_southwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

bitboard flip_all(bitboard disks_to_flip, bitboard friendly_disks, bitboard move);

void print_board(bitboard black, bitboard white);

uint8_t count_ones(uint64_t bb);

bitboard all_moves(bitboard opponent, bitboard player);

#endif
