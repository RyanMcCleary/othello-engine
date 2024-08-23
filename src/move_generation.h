#ifndef MOVE_GENERATION_H_INCLUDED
#define MOVE_GENERATION_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef uint64_t bitboard;

struct board {
	bitboard black;
	bitboard white;
};

struct magic_info {
    bitboard magic;
    bitboard mask;
    uint8_t shift;
};

struct square_index {
    uint8_t rank;
    uint8_t file;
};

bitboard square_mask(uint8_t rank, uint8_t file);

bitboard orthogonal_mask(uint8_t rank, uint8_t file);

size_t magic_hash(bitboard bb, struct magic_info *minfo);

bitboard *fill_table(bitboard *table, struct square_index square,
            struct magic_info *minfo);

bitboard diagonal_mask(uint8_t rank, uint8_t file);

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

#endif
