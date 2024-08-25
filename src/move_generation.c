#include <stdio.h>
#include "move_generation.h"
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define NOT_A_FILE UINT64_C(0xFEFEFEFEFEFEFEFE)
#define NOT_H_FILE UINT64_C(0x7F7F7F7F7F7F7F7F)

#define C64 UINT64_C

struct magic_info orthogonal_magics[64];
struct magic_info diagonal_magics[64];

bitboard orthogonal_table[64][4096];
bitboard diagonal_table[64][512];

const bitboard o_magic_values[64] = {
    C64(0x0080008040006118), C64(0x104000200010000d), C64(0x0100182000400490), C64(0x2200101402202840),
    C64(0x0200100208040120), C64(0x0100040003058208), C64(0x0400024102040090), C64(0x4200011040820024),
    C64(0x1010800040001424), C64(0x0601001180084000), C64(0x0405200009042090), C64(0x0042100002080381),
    C64(0x0800804800042001), C64(0x2040a00a00010280), C64(0x8860800200408100), C64(0x8020800040902700),
    C64(0x4850208000400490), C64(0x0040029000082000), C64(0x30064008002c9000), C64(0x00040008103a0104),
    C64(0x00102a0008112010), C64(0x0240020010089306), C64(0x0281004000800120), C64(0x88520a000580250c),
    C64(0x004225108000c000), C64(0x801080290008c004), C64(0x1444480120004214), C64(0x2080042280081000),
    C64(0x0420340200100801), C64(0x0701008020012408), C64(0x0402100040008102), C64(0x0080852a8000c100),
    C64(0x0203004482001020), C64(0x1120104000190008), C64(0x000001013a100020), C64(0xc048500442001018),
    C64(0x8a24000910080068), C64(0x8000008400200122), C64(0x0020002824800211), C64(0x802000a0820008c3),
    C64(0x0804400130208000), C64(0x0020583020c44000), C64(0x000e080110029202), C64(0x0000041040e20002),
    C64(0x4004302428090002), C64(0x2220040200010400), C64(0x080200004800a006), C64(0x14060890a0410006),
    C64(0x4830800460410008), C64(0x00c0000590224080), C64(0x0820082043040020), C64(0x0880201448900300),
    C64(0x0020002280090850), C64(0x2606040000810010), C64(0x0a6810020a800c08), C64(0x81200880a2004300),
    C64(0x0140204090810a02), C64(0x1084102009024082), C64(0x2000184420001041), C64(0x4450880402300021),
    C64(0x000000081401e006), C64(0x140ac09802040001), C64(0x0001008450020804), C64(0x6004808100402412)};

const bitboard d_magic_values[64] = {
    C64(0x00004402280100e0), C64(0x21008a0800082002), C64(0x0840220200c04403), C64(0x0881864428080010),
    C64(0x0208404000000810), C64(0x0001220140802824), C64(0x0044b40022044020), C64(0x4048084402020080),
    C64(0x2091040422005008), C64(0x6020004080260043), C64(0xc801100c00400280), C64(0x1400010103020284),
    C64(0x2008012a05000009), C64(0x0002050821040104), C64(0x00121010840020a0), C64(0x0040082022003040),
    C64(0x2405012004108080), C64(0x8016250101115220), C64(0x0800801041210200), C64(0x0102090041000802),
    C64(0x040400044044c900), C64(0x000a00000c2c2008), C64(0x00044401341080c4), C64(0x0102802009041000),
    C64(0x0401080000890050), C64(0x11300583004102c0), C64(0x0284401541049084), C64(0x0020280001004008),
    C64(0x0000848004006000), C64(0x0082400800a40140), C64(0x0010220080401400), C64(0x1012200200825900),
    C64(0x600809584000a120), C64(0x8002044c00200800), C64(0x0400040100220830), C64(0x8001004040080810),
    C64(0xce004080a0120200), C64(0x0004081223020220), C64(0x2882000d02840400), C64(0x001204a110024514),
    C64(0x220c08827002010c), C64(0x1000440018058090), C64(0x4100420140400080), C64(0x80880a0e01084024),
    C64(0x0801400088405022), C64(0x40401421b1400080), C64(0x0801811022200115), C64(0x008400024f085008),
    C64(0x0003420104004200), C64(0x8000181100200000), C64(0x010a100481091003), C64(0x8008210417041348),
    C64(0x0410100404022042), C64(0x0090020041010002), C64(0x010280022a018081), C64(0x0010002504000240),
    C64(0x2081188023c42280), C64(0x8400004180100160), C64(0x0040001640212525), C64(0x2002404500a82641),
    C64(0x0000110100412400), C64(0x00000001500101d0), C64(0x00704010040a2205), C64(0x00c0001122000560)};

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
    bitboard result = 0;
    for (int8_t r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++) {
        result |= square_mask(r, f);
    }
    for (int8_t r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--) {
        result |= square_mask(r, f);
    }
    for (int8_t r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++) {
        result |= square_mask(r, f);
    }
    for (int8_t r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--) {
        result |= square_mask(r, f);
    }
    return result;
    // TODO: fix the below version; using a simple loop for now
	// return (signed_shift(UINT64_C(0x0040201008040200), 8 * (rank - file)) |
    //    signed_shift(UINT64_C(0x0002040810204000), 8 * (rank - file + 1)));
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

size_t magic_hash(bitboard disks_to_flip, bitboard friendly_disks,
        struct magic_info *minfo) {
    return (size_t)(((disks_to_flip * minfo->magic[0]) ^
        (friendly_disks * minfo->magic[1])) >> minfo->shift);
}

bitboard *fill_table(bitboard *table, struct square_index square,
            struct magic_info *minfo) {
    bitboard disks_to_flip = minfo->mask;
    bitboard empty = ~UINT64_C(0);
    while (disks_to_flip) {
        bitboard friendly_disks = ~disks_to_flip;
        while(friendly_disks) {
            size_t hash = magic_hash(disks_to_flip, friendly_disks, minfo);
            bitboard flipped = flip_all(disks_to_flip, friendly_disks,
                square_mask(square.rank, square.file));
            if (table[hash] == empty) {
                table[hash] = flipped;
            } else if (table[hash] != flipped) {
                return NULL;
            }
            friendly_disks = (friendly_disks - 1) & ~disks_to_flip;
        }
        disks_to_flip = (disks_to_flip - 1) & minfo->mask;
    }
    return table;
}

void clear_table(bitboard *table, size_t len) {
    for (size_t i = 0; i < len; i++) {
        table[i] = ~UINT64_C(0);
    }
}

/*
bitboard magic_lookup(bitboard *table, struct magic_info *minfo,
            bitboard bb) {
    return table[magic_hash(bb, minfo)];
}*/

/*
bitboard magic_flip_orthogonally(bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t square) {
    bitboard *table = orthogonal_table[square];
    struct magic_info *minfo = &orthogonal_magics[square];
    return (magic_lookup(table, minfo, disks_to_flip & minfo->mask) &
        magic_lookup(table, minfo, ~friendly_disks & minfo->mask));
}

bitboard magic_flip_diagonally(bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t square) {
    bitboard *table = diagonal_table[square];
    struct magic_info *minfo = &diagonal_magics[square];
    return (magic_lookup(table, minfo, disks_to_flip & minfo->mask) &
        magic_lookup(table, minfo, ~friendly_disks & minfo->mask));
}

bitboard magic_flip(bitboard disks_to_flip, bitboard friendly_disks,
            uint8_t square) {
    return (magic_flip_orthogonally(disks_to_flip, friendly_disks, square) |
        magic_flip_diagonally(disks_to_flip, friendly_disks, square));
}
*/

// int main(int argc, char **argv) {
    // for (int i = 0; i < 64; i++) {
        // struct square_index square = { .rank = i / 8, .file = i % 8 };
        // orthogonal_magics[i].mask = orthogonal_mask(square.rank, square.file);
        // orthogonal_magics[i].magic = o_magic_values[i];
        // orthogonal_magics[i].shift = 52;
        // diagonal_magics[i].mask = diagonal_mask(square.rank, square.file);
        // diagonal_magics[i].magic = d_magic_values[i];
        // diagonal_magics[i].shift = 55;
        // clear_table(orthogonal_table[i], 4096);
        // clear_table(diagonal_table[i], 512);
        // fill_table(orthogonal_table[i], square, &orthogonal_magics[i]);
        // fill_table(diagonal_table[i], square, &diagonal_magics[i]);
        // /*
        // for (int j = 0; j < 4096; j++) {
            // if (orthogonal_table[i][j] != 0) {
                // puts("Nonzero!");
            // }
        // }
        // */
    // }
    // bitboard black = square_mask(3, 3) | square_mask(4, 4);
    // bitboard white = square_mask(3, 4) | square_mask(4, 3);
    // print_board(black, white);
    // puts("\n--------\n");
    // bitboard move = square_mask(2, 4);
    // bitboard flipped = magic_flip(white, black, 2 * 8 + 4);
    // print_board(black | flipped | move, white & ~flipped); 
    // printf("0x%" PRIx64 "\n", flipped);
    // return 0;
// }*/
            