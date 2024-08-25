#include <stdio.h>
#include "move_generation.h"
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define NOT_A_FILE UINT64_C(0xFEFEFEFEFEFEFEFE)
#define NOT_H_FILE UINT64_C(0x7F7F7F7F7F7F7F7F)


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

uint8_t count_ones(uint64_t bb) {
    bb =  bb - ((bb >> 1) & 0x5555555555555555ULL);
    bb = (bb & 0x3333333333333333ULL) + ((bb >> 2) & 0x3333333333333333ULL);
    bb = (bb + (bb >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (uint8_t)((bb * 0x0101010101010101ULL) >> 56);
}

bitboard shift_north(bitboard bb) {
    return bb >> 8;
}

bitboard shift_south(bitboard bb) {
    return bb << 8;
}

bitboard shift_east(bitboard bb) {
    return (bb << 1) & NOT_A_FILE;
}

bitboard shift_west(bitboard bb) {
    return (bb >> 1) & NOT_H_FILE;
}

bitboard shift_northeast(bitboard bb) {
    return (bb >> 7) & NOT_A_FILE;
}

bitboard shift_northwest(bitboard bb) {
    return (bb >> 9) & NOT_H_FILE;
}

bitboard shift_southeast(bitboard bb) {
    return (bb << 9) & NOT_A_FILE;
}

bitboard shift_southwest(bitboard bb) {
    return (bb << 7) & NOT_H_FILE;
}

bitboard signed_shift(bitboard bb, int shift) {
	if (shift > 0) {
		return bb << (unsigned) shift;
	} else {
		return bb >> (unsigned)(-shift);
	}
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

bitboard all_moves(bitboard opponent, bitboard player) {
    bitboard moves = 0;
    bitboard empty = ~(opponent | player);
    for (bitboard n = opponent & shift_north(player); n; n = opponent & shift_north(n))
        moves |= empty & shift_north(n);
    for (bitboard s = opponent & shift_south(player); s; s = opponent & shift_south(s))
        moves |= empty & shift_south(s);
    for (bitboard e = opponent & shift_east(player); e; e = opponent & shift_east(e))
        moves |= empty & shift_east(e);
    for (bitboard w = opponent & shift_west(player); w; w = opponent & shift_west(w))
        moves |= empty & shift_west(w);
    for (bitboard ne = opponent & shift_northeast(player); ne; ne = opponent & shift_northeast(ne))
        moves |= empty & shift_northeast(ne);
    for (bitboard nw = opponent & shift_northwest(player); nw; nw = opponent & shift_northwest(nw))
        moves |= empty & shift_northwest(nw);
    for (bitboard se = opponent & shift_southeast(player); se; se = opponent & shift_southeast(se))
        moves |= empty & shift_southeast(se);
    for (bitboard sw = opponent & shift_southwest(player); sw; sw = opponent & shift_southwest(sw))
        moves |= empty & shift_southwest(sw);
    return moves;
}
