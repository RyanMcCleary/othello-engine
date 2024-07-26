#ifndef RANDOM_UINT64
#define RANDOM_UINT64

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct random_generator;

struct random_generator *new_random_generator(void);

uint64_t random_uint64(struct random_generator *gen);

void free_random_generator(struct random_generator *gen);

#ifdef __cplusplus
}
#endif

#endif
