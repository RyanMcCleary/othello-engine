#include <stdio.h>
#include "bitboard.h"
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define NOT_A_FILE UINT64_C(0xFEFEFEFEFEFEFEFE)
#define NOT_H_FILE UINT64_C(0x7F7F7F7F7F7F7F7F)

bitboard orthogonal_table[64][4096];

struct magic_info orthogonal_magics[64];

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

uint64_t random_uint64(void) {
	uint64_t result = (uint64_t)(rand() & 0xFFFF);
	result |= ((uint64_t)(rand() & 0xFFFF) << 16);
	result |= ((uint64_t)(rand() & 0xFFFF) << 32);
	return result | ((uint64_t)(rand() & 0xFFFF) << 48);
}

size_t magic_hash(bitboard bb, struct magic_info *minfo) {
    return (size_t)((bb * minfo->magic) >> minfo->shift);
}

bitboard *fill_table(bitboard *table, struct square_index square,
            struct magic_info *minfo) {
    bitboard subset = minfo->mask;
    bitboard square_mask = UINT64_C(1) << (8 * square.rank + square.file);
    while (subset) {
        size_t hash = magic_hash(subset, minfo);
        bitboard flipped = flip_all(subset, ~subset, square_mask);
        if (table[hash] != square_mask) {
            return NULL;
        }
        table[hash] = flipped;
        subset = (subset - 1) & minfo->mask;
        // _i++;
    }
    return table;
}

void find_magic(bitboard *table, struct magic_info *minfo,
                    struct square_index square, bitboard mask,
                    uint8_t num_bits) {
    minfo->mask = mask;
    minfo->shift = 64 - num_bits;
    while (1) {
        for (size_t i = 0; i < (1 << num_bits); i++) {
            table[i] = UINT64_C(1) << (8 * square.rank + square.file);
        }
        minfo->magic = random_uint64() & random_uint64() & random_uint64();
        if (fill_table(table, square, minfo)) {
            return;
        }
    }
}

void find_orthogonal_magics(bitboard (*table)[4096], struct magic_info *magics) {
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            size_t idx = 8 * rank + file;
            struct square_index square = { .rank = rank, .file = file };
            find_magic(table[idx], &magics[idx], square,
                orthogonal_mask(rank, file), 12);
        }
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    find_orthogonal_magics(orthogonal_table, orthogonal_magics);
    bitboard bb = 0;
    bb = bb_set(bb, 4, 4);
    print_board(flip_all(bb, ~bb, square_mask(3,4)), 0);
    puts("\n------\n");
    size_t square = 3*8+4;
    printf("{ .mask = 0x%" PRIx64 ", .magic = 0x%" PRIx64 ", .shift = %d }\n",
        orthogonal_magics[square].mask, orthogonal_magics[square].magic,
        orthogonal_magics[square].shift);
    printf("0x%" PRIx64 "\n", magic_hash(bb, &orthogonal_magics[square]));
    return 0;
}

