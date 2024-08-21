#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "bitboard.h"

uint64_t random_uint64(void) {
	uint64_t result = (uint64_t)(rand() & 0xFFFF);
	result |= ((uint64_t)(rand() & 0xFFFF) << 16);
	result |= ((uint64_t)(rand() & 0xFFFF) << 32);
	return result | ((uint64_t)(rand() & 0xFFFF) << 48);
}}

int main(int argc, char **argv) {
	
}