#include <stdio.h>
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
	return (0xFFULL << rank) | (0x0101010101010101ULL << file);
}

bitboard diagonal_mask(uint8_t rank, uint8_t file) {
	bitboard main_diag = 0x0102040810204080ULL;
	bitboard anti_diag = 0x8040201008040201ULL;
	mask = main_diag | anti_diag;
	return signed_shift(mask, (int8_t)rank - 3) | signed_shift(mask, (int8_t)file - 3);
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

/*
bitboard flip_all(bitboard disks_to_flip, bitboard move) {
	bitboard result = flip_north(disks_to_flip, move);
	result |= flip_south(disks_to_flip, move);
	result |= flip_east(disks_to_flip, move);
	result |= flip_west(disks_to_flip, move);
	result |= flip_northeast(disks_to_flip, move);
	result |= flip_northwest(disks_to_flip, move);
	result |= flip_southeast(disks_to_flip, move);
	return result | flip_southwest(disks_to_flip, move);
}
*/
