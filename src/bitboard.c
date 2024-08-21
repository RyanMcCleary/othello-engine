#include <stdio.h>
#include "bitboard.h"
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

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
	return bb | (1ULL << (rank * 8 + file));
}

bitboard bb_flip(bitboard bb, uint8_t rank, uint8_t file) {
	return bb ^ (1ULL << (rank * 8 + file));
}

bitboard signed_shift(bitboard bb, int shift) {
	if (shift > 0) {
		return bb << (unsigned) shift;
	} else {
		return bb >> (unsigned)(-shift);
	}
}

bitboard orthogonal_mask(uint8_t rank, uint8_t file) {
	return (~(UINT64_C(1) << 8 * rank + file) &
        ((UINT64_C(0x7E) << 8 * rank) |
        (UINT64_C(0x0001010101010100) << file)));
}

bitboard diagonal_mask(uint8_t rank, uint8_t file) {
	return (~(UINT64_C(1) << 8 * rank + file) &
        (signed_shift(UINT64_C(0x0040201008040200), 8 * (rank - file))  |
        signed_shift(UINT64_C(0x0002040810204000), 8 * (rank - file + 1))));
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
		0xFFFFFFFFFFFFFFFFULL, -8);
}

bitboard flip_south(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0xFFFFFFFFFFFFFFFFULL, 8);
}

bitboard flip_east(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0x7F7F7F7F7F7F7F7FULL, 1);
}

bitboard flip_west(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0xFEFEFEFEFEFEFEFEULL, -1);
}

bitboard flip_northeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0x7F7F7F7F7F7F7F7FULL, -7);
}

bitboard flip_northwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0xFEFEFEFEFEFEFEFEULL, -9);
}

bitboard flip_southeast(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0x7F7F7F7F7F7F7F7FULL, 9);
}

bitboard flip_southwest(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {
	return generalized_ray_flip(disks_to_flip, friendly_disks, move,
		0xFEFEFEFEFEFEFEFEULL, 7);
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

uint64_t random_uint64(void) {
	uint64_t result = (uint64_t)(rand() & 0xFFFF);
	result |= ((uint64_t)(rand() & 0xFFFF) << 16);
	result |= ((uint64_t)(rand() & 0xFFFF) << 32);
	return result | ((uint64_t)(rand() & 0xFFFF) << 48);
}

void foreach_subset(bitboard set, void (*fn)(bitboard)) {
    for (bitboard subset = set; subset; subset = (subset - 1) & set) fn(subset);
}

void subset_fn(bitboard subset) {
    printf("0x%" PRIx64 "\n", subset);
}

bitboard *fill_table(bitboard *table, struct square_index square,
            struct magic_info *minfo) {
    bitboard subset = minfo->mask;
    bitboard square_mask = UINT64_C(1) << (8 * square.rank + square.file);
    while (subset) {
        size_t hash = (size_t)((subset * minfo->magic) >> (64 - minfo->num_bits));
        bitboard flipped = flip_all(minfo->mask, ~minfo->mask, square_mask);
        if (table[hash] != square_mask) {
            return NULL;
        }
        table[hash] = flipped;
        subset = (subset - 1) & minfo->mask;
    }
    return table;
}

struct magic_info find_magic(bitboard *table, struct square_index square,
                    bitboard mask, uint8_t num_bits) {
    struct magic_info minfo = { .mask = mask, .num_bits = num_bits };
    while (1) {
        for (size_t i = 0; i < (1 << num_bits); i++) {
            table[i] = UINT64_C(1) << (8 * square.rank + square.file);
        }
        minfo.magic = random_uint64() & random_uint64() & random_uint64();
        if (fill_table(table, square, &minfo)) {
            return minfo;
        }
    }
}

int main(int argc, char **argv) {
    struct square_index square = { .rank = 4, .file = 4 };
    bitboard table[2048];
    struct magic_info minfo = find_magic(table, square, orthogonal_mask(4, 4), 11);
    printf("filled table: magic = 0x%" PRIx64 "\n", minfo.magic);
    return 0;
}

