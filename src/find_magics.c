#include "move_generation.h"
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>

uint64_t random_uint64(void) {
	uint64_t result = (uint64_t)(rand() & 0xFFFF);
	result |= ((uint64_t)(rand() & 0xFFFF) << 16);
	result |= ((uint64_t)(rand() & 0xFFFF) << 32);
	return result | ((uint64_t)(rand() & 0xFFFF) << 48);
}

void find_magic(bitboard *table, struct magic_info *minfo,
                    struct square_index square, bitboard mask,
                    uint8_t num_bits) {
    minfo->mask = mask;
    minfo->shift = 64 - num_bits;
    while (1) {
        for (size_t i = 0; i < (1 << num_bits); i++) {
            table[i] = ~UINT64_C(0);
        }
        minfo->magic = random_uint64() & random_uint64() & random_uint64();
        if (fill_table(table, square, minfo)) {
            return;
        }
    }
}

void find_orthogonal_magics(struct magic_info *magics) {
    bitboard table[4096];
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            size_t idx = 8 * rank + file;
            struct square_index square = { .rank = rank, .file = file };
            find_magic(table, &magics[idx], square,
                orthogonal_mask(rank, file), 12);
        }
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    struct magic_info o_magics[64];
    find_orthogonal_magics(o_magics);
    for (int i = 0; i < 64; i++) {
        printf("magics[%d] = 0x%" PRIx64 "\n", i, o_magics[i].magic);
    }
    return 0;
}