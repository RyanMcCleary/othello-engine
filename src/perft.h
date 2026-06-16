#ifndef PERFT_H_INCLUDED
#define PERFT_H_INCLUDED

#include <stdint.h>
#include "board.h"

/* Count the leaf nodes reachable in exactly `depth` plies from `b`.
 * A forced pass does not consume a ply (you can never pass twice in a row),
 * and a finished game counts as a single leaf. */
uint64_t perft(Board b, int depth);

#endif
