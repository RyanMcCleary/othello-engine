#include <stdio.h>
#include "move_generation.h"
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define NOT_A_FILE UINT64_C(0xFEFEFEFEFEFEFEFE)
#define NOT_H_FILE UINT64_C(0x7F7F7F7F7F7F7F7F)

struct magic_info orthogonal_magics[64];

void print_board(bitboard black, bitboard white) {
	for (uint8_t rank = 0; rank < 8; rank++) {
		for (uint8_t file = 0; file < 8; file++) {
			if (black & square_mask(rank, file)) {
				putchar('B');
			} else if (white & square_mask(rank, file)) {
				putchar('W');
			} else {
				putchar('*');
			}
		}
		putchar('\n');
	}
}

bitboard square_mask(uint8_t rank, uint8_t file) {
    return UINT64_C(1) << (8 * rank + file);
}

bitboard signed_shift(bitboard bb, int shift) {
	if (shift > 0) {
		return bb << (unsigned) shift;
	} else {
		return bb >> (unsigned)(-shift);
	}
}

bitboard orthogonal_mask(uint8_t rank, uint8_t file) {
	return ((UINT64_C(0x7E) << 8 * rank) |
        (UINT64_C(0x0001010101010100) << file));
}

bitboard diagonal_mask(uint8_t rank, uint8_t file) {
	return (signed_shift(UINT64_C(0x0040201008040200), 8 * (rank - file)) |
        signed_shift(UINT64_C(0x0002040810204000), 8 * (rank - file + 1)));
}

bitboard generalized_ray_flip(bitboard disks_to_flip, bitboard friendly_disks,
            bitboard move, bitboard no_wrap, int shift) {
	bitboard result = 0;
	bitboard gen = move;
	bitboard next;
	bitboard pro = disks_to_flip & no_wrap;
	while ((next = signed_shift(gen, shift)) & pro) {
		result |= next;
		gen = next & pro;
	}
	if (next & no_wrap & friendly_disks) {
		return result;
	}
	else {
		return 0;
	}
}

bitboard flip_north(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		~UINT64_C(0), -8);
}

bitboard flip_south(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		~UINT64_C(0), 8);
}

bitboard flip_east(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_A_FILE, 1);
}

bitboard flip_west(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_H_FILE, -1);
}

bitboard flip_northeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_A_FILE, -7);
}

bitboard flip_northwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_H_FILE, -9);
}

bitboard flip_southeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_A_FILE, 9);
}

bitboard flip_southwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		NOT_H_FILE, 7);
}

bitboard flip_all(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	bitboard result = flip_north(disks_to_flip, friendly_disks, move);
	result |= flip_south(disks_to_flip, friendly_disks, move);
	result |= flip_east(disks_to_flip, friendly_disks, move);
	result |= flip_west(disks_to_flip, friendly_disks, move);
	result |= flip_northeast(disks_to_flip, friendly_disks, move);
	result |= flip_northwest(disks_to_flip, friendly_disks, move);
	result |= flip_southeast(disks_to_flip, friendly_disks, move);
	return result | flip_southwest(disks_to_flip, friendly_disks, move);
}

size_t magic_hash(bitboard bb, struct magic_info *minfo) {
    return (size_t)((bb * minfo->magic) >> minfo->shift);
}

bitboard *fill_table(bitboard *table, struct square_index square,
            struct magic_info *minfo) {
    bitboard subset = minfo->mask;
    bitboard empty = ~UINT64_C(0);
    while (subset) {
        size_t hash = magic_hash(subset, minfo);
        bitboard flipped = flip_all(subset, ~subset,
            square_mask(square.rank, square.file));
        if (table[hash] == empty) {
            table[hash] = flipped;
        } else if (table[hash] != flipped) {
            return NULL;
        }
        subset = (subset - 1) & minfo->mask;
    }
    return table;
}

bitboard magic_lookup(bitboard *table, struct magic_info *minfo,
            bitboard bb) {
    return table[magic_hash(bb, minfo)];
}

bitboard magic_flip_orthogonally(bitboard (*tables)[4096], struct magic_info magics,
            bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t rank, uint8_t file) {
    bitboard o_mask = orthogonal_mask(rank, file);
    size_t square = 8 * rank + file;
    return (magic_lookup(tables[square], &magics[square], disks_to_flip & o_mask) &
        magic_lookup(tables[square], &magics[square], ~friendly_disks | o_mask));
}

bitboard magic_flip_diagonally(bitboard (*tables)[4096], struct magic_info magics,
            bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t rank, uint8_t file) {
    bitboard d_mask = diagonal_mask(rank, file);
    size_t square = 8 * rank + file;
    return (magic_lookup(tables[square], &magics[square], disks_to_flip & d_mask) &
        magic_lookup(tables[square], &magics[square], ~friendly_disks | d_mask));
}

bitboard magic_flip(bitboard (*tables)[4096], struct magic_info magics,
            bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t rank, uint8_t file) {
    return (magic_flip_orthogonally(tables, magics, disks_to_flip, friendly_disks, rank, file) |
        magic_flip_diagonally(tables, magics, disks_to_flip, friendly_disks, rank, file));
}
            