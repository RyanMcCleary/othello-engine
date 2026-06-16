#include <stdio.h>
#include "move_generation.h"
#include <inttypes.h>

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

/* Single-step shifts. Each masks away the file that would wrap around the
 * board edge, so they can be chained safely in the fills below. */
static inline bitboard shift_north(bitboard bb) { return bb >> 8; }
static inline bitboard shift_south(bitboard bb) { return bb << 8; }
static inline bitboard shift_east(bitboard bb)  { return (bb << 1) & NOT_A_FILE; }
static inline bitboard shift_west(bitboard bb)  { return (bb >> 1) & NOT_H_FILE; }
static inline bitboard shift_northeast(bitboard bb) { return (bb >> 7) & NOT_A_FILE; }
static inline bitboard shift_northwest(bitboard bb) { return (bb >> 9) & NOT_H_FILE; }
static inline bitboard shift_southeast(bitboard bb) { return (bb << 9) & NOT_A_FILE; }
static inline bitboard shift_southwest(bitboard bb) { return (bb << 7) & NOT_H_FILE; }

/*
 * Kogge-Stone occluded fill: flood `gen` along a direction through the cells
 * set in `pro`, in three doubling steps (shift by 1, 2, 4 cells) instead of the
 * six sequential steps of a Dumb7Fill. For diagonals/horizontals `pro` is
 * pre-masked to one edge file; the iterative `pro &= ...` then prevents wraps.
 */
static inline bitboard occl_n(bitboard g, bitboard p) {
    g |= p & (g >> 8);  p &= (p >> 8);
    g |= p & (g >> 16); p &= (p >> 16);
    return g | (p & (g >> 32));
}
static inline bitboard occl_s(bitboard g, bitboard p) {
    g |= p & (g << 8);  p &= (p << 8);
    g |= p & (g << 16); p &= (p << 16);
    return g | (p & (g << 32));
}
static inline bitboard occl_e(bitboard g, bitboard p) {
    p &= NOT_A_FILE;
    g |= p & (g << 1); p &= (p << 1);
    g |= p & (g << 2); p &= (p << 2);
    return g | (p & (g << 4));
}
static inline bitboard occl_w(bitboard g, bitboard p) {
    p &= NOT_H_FILE;
    g |= p & (g >> 1); p &= (p >> 1);
    g |= p & (g >> 2); p &= (p >> 2);
    return g | (p & (g >> 4));
}
static inline bitboard occl_ne(bitboard g, bitboard p) {
    p &= NOT_A_FILE;
    g |= p & (g >> 7);  p &= (p >> 7);
    g |= p & (g >> 14); p &= (p >> 14);
    return g | (p & (g >> 28));
}
static inline bitboard occl_nw(bitboard g, bitboard p) {
    p &= NOT_H_FILE;
    g |= p & (g >> 9);  p &= (p >> 9);
    g |= p & (g >> 18); p &= (p >> 18);
    return g | (p & (g >> 36));
}
static inline bitboard occl_se(bitboard g, bitboard p) {
    p &= NOT_A_FILE;
    g |= p & (g << 9);  p &= (p << 9);
    g |= p & (g << 18); p &= (p << 18);
    return g | (p & (g << 36));
}
static inline bitboard occl_sw(bitboard g, bitboard p) {
    p &= NOT_H_FILE;
    g |= p & (g << 7);  p &= (p << 7);
    g |= p & (g << 14); p &= (p << 14);
    return g | (p & (g << 28));
}

/*
 * Branchless flip. Flood from the move square through opponent discs; the discs
 * to flip are that run (gen minus the move square), kept only when the cell one
 * step past the run holds a friendly disc (the run is closed).
 */
#define DEFINE_FLIP(name, occl, shift)                                         \
bitboard name(bitboard disks_to_flip, bitboard friendly_disks, bitboard move) {\
    bitboard gen = occl(move, disks_to_flip);                                  \
    bitboard run = gen ^ move;                                                 \
    return (shift(gen) & friendly_disks) ? run : 0;                            \
}

DEFINE_FLIP(flip_north, occl_n, shift_north)
DEFINE_FLIP(flip_south, occl_s, shift_south)
DEFINE_FLIP(flip_east, occl_e, shift_east)
DEFINE_FLIP(flip_west, occl_w, shift_west)
DEFINE_FLIP(flip_northeast, occl_ne, shift_northeast)
DEFINE_FLIP(flip_northwest, occl_nw, shift_northwest)
DEFINE_FLIP(flip_southeast, occl_se, shift_southeast)
DEFINE_FLIP(flip_southwest, occl_sw, shift_southwest)

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

/*
 * Branchless move generation. For each direction, flood from the player's discs
 * through contiguous opponent discs (Kogge-Stone); the run is `gen & opponent`,
 * and the empty square one step past it is a legal move.
 */
#define MOVES_RAY(occl, shift) \
    (moves |= shift(occl(player, opponent) & opponent) & empty)

bitboard all_moves(bitboard opponent, bitboard player) {
    bitboard empty = ~(opponent | player);
    bitboard moves = 0;
    MOVES_RAY(occl_n, shift_north);
    MOVES_RAY(occl_s, shift_south);
    MOVES_RAY(occl_e, shift_east);
    MOVES_RAY(occl_w, shift_west);
    MOVES_RAY(occl_ne, shift_northeast);
    MOVES_RAY(occl_nw, shift_northwest);
    MOVES_RAY(occl_se, shift_southeast);
    MOVES_RAY(occl_sw, shift_southwest);
    return moves;
}

bitboard neighbors(bitboard bb) {
    bitboard ew = ((bb << 1) & NOT_A_FILE) | ((bb >> 1) & NOT_H_FILE);
    bitboard row = bb | ew;
    return ew | (row << 8) | (row >> 8);
}
