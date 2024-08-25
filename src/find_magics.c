#include "move_generation.h"
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>

uint64_t random_uint64(void) {
	return ((uint64_t)(rand() & 0xFFFF) |
        ((uint64_t)(rand() & 0xFFFF) << 16) |
        ((uint64_t)(rand() & 0xFFFF) << 32) |
	    ((uint64_t)(rand() & 0xFFFF) << 48));
}

void find_magic(bitboard *table, struct magic_info *minfo,
                    struct square_index square, bitboard mask,
                    uint8_t num_bits) {
    puts("Entered find_magic()");
    minfo->mask = mask;
    minfo->shift = 64 - num_bits;
    while (1) {
        for (size_t i = 0; i < (1 << num_bits); i++) {
            table[i] = ~UINT64_C(0);
        }
        minfo->magic[0] = random_uint64() & random_uint64() & random_uint64();
        minfo->magic[1] = random_uint64() & random_uint64() & random_uint64();
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

void find_diagonal_magics(struct magic_info *magics) {
    bitboard table[512];
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            size_t idx = 8 * rank + file;
            struct square_index square = { .rank = rank, .file = file };
            find_magic(table, &magics[idx], square,
                diagonal_mask(rank, file), 9);
        }
    }
}

void print_magic_initializer(const char *name, struct magic_info *magics) {
    printf("const bitboard %s[64] = {\n", name);
    for (int i = 0; i < 15; i++) {
        printf("   ");
        for (int j = 0; j < 4; j++) {
            printf(" C64(0x%016" PRIx64 "),", magics[4 * i + j].magic[0]);
        }
        putchar('\n');
    }
    printf("   ");
    for (int j = 0; j < 3; j++) {
        printf(" C64(0x%016" PRIx64 "),", magics[60 + j].magic[0]);
    }
    printf(" C64(0x%016" PRIx64 ")};\n", magics[63].magic[0]);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    struct magic_info o_magics[64];
    struct magic_info d_magics[64];
    find_orthogonal_magics(o_magics);
    find_diagonal_magics(d_magics);
    print_magic_initializer("d_magic_values", d_magics);
    return 0;
}