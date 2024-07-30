#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

uint64_t random_uint64(void) {
	uint64_t result = (uint64_t)(rand() & 0xFFFF);
	result |= ((uint64_t)(rand() & 0xFFFF) << 16);
	result |= ((uint64_t)(rand() & 0xFFFF) << 32);
	return result | ((uint64_t)(rand() & 0xFFFF) << 48);
}

int main(int argc, char **argv) {
	int counts[64];
	for (int i = 0; i < 64; i++) counts[i] = 0;
	srand(time(NULL));
	uint64_t mask = 0x01;
	for (int i = 0; i < 1000000; i++) {
		uint64_t r = random_uint64();
		for (int bit = 0; bit < 64; bit++) {
			if (r & (mask << bit)) counts[bit]++;
		}
	}
	for (int i = 0; i < 64; i++) {
		printf("%d\n", counts[i]);
	}
}