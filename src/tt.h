#ifndef TT_H_INCLUDED
#define TT_H_INCLUDED

#include <stdint.h>
#include "board.h"

/* Bound kinds for a stored score relative to the search window. */
enum { TT_EXACT, TT_LOWER, TT_UPPER };

/* Which search produced an entry. Heuristic (scaled) and exact-endgame (raw
 * disc-diff) scores share the table but must never be read across kinds. */
enum { TT_KIND_MID, TT_KIND_END };

#define TT_NO_MOVE 64

typedef struct {
    uint64_t key;     /* full hash of the position (0 = empty slot)        */
    int32_t  score;
    uint8_t  depth;
    uint8_t  flag;    /* TT_EXACT / TT_LOWER / TT_UPPER                     */
    uint8_t  move_sq; /* best move square 0..63, or TT_NO_MOVE              */
    uint8_t  kind;    /* TT_KIND_MID / TT_KIND_END                         */
} TTEntry;

/* Hash of a position (side-to-move relative, so player/opponent matters). */
uint64_t board_hash(Board b);

void tt_clear(void);

/* Slot for `key`; caller checks `slot->key == key` for a real hit. */
TTEntry *tt_get(uint64_t key);

/* Depth-preferred store. */
void tt_store(uint64_t key, int depth, int score, int flag, uint8_t move_sq,
              int kind);

#endif
