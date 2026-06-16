#include <string.h>
#include "tt.h"

/* Table size in entries = 2^TT_BITS. 2^20 * 16 B = 16 MB (override with
 * -DTT_BITS=N; lower it for very memory-constrained browser targets). */
#ifndef TT_BITS
#define TT_BITS 20
#endif

#define TT_SIZE (UINT64_C(1) << TT_BITS)
#define TT_MASK (TT_SIZE - 1)

static TTEntry table[TT_SIZE];

static inline uint64_t mix64(uint64_t x) {
    x ^= x >> 33;
    x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33;
    x *= UINT64_C(0xc4ceb9fe1a85ec53);
    x ^= x >> 33;
    return x;
}

uint64_t board_hash(Board b) {
    /* Asymmetric combine so (player,opponent) != (opponent,player). */
    return mix64(b.player) ^
           (mix64(b.opponent) * UINT64_C(0x9E3779B97F4A7C15) + 1);
}

void tt_clear(void) {
    memset(table, 0, sizeof table);
}

TTEntry *tt_get(uint64_t key) {
    return &table[key & TT_MASK];
}

void tt_store(uint64_t key, int depth, int score, int flag, uint8_t move_sq,
              int kind) {
    TTEntry *slot = &table[key & TT_MASK];
    /* Replace empty slots, the same position, or shallower entries. */
    if (slot->key != key && slot->key != 0 && slot->depth > depth) {
        return;
    }
    slot->key     = key;
    slot->score   = (int32_t)score;
    slot->depth   = (uint8_t)depth;
    slot->flag    = (uint8_t)flag;
    slot->move_sq = move_sq;
    slot->kind    = (uint8_t)kind;
}
