#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "board.h"
#include "search.h"
#include "perft.h"

static void print_move(bitboard move) {
    for (uint8_t i = 0; i < 64; i++) {
        if (move & (UINT64_C(1) << i)) {
            printf("%c%d", 'a' + (i % 8), (i / 8) + 1);
            return;
        }
    }
    printf("(none)");
}

int main(void) {
    Board b = board_initial();
    print_board(b.player, b.opponent); /* player is Black at the start */

    bitboard move = best_move(b, 6);
    printf("\nBest move (depth 6): ");
    print_move(move);
    printf("\n\n");

    /* Perft benchmark for NPS. */
    int depth = 9;
    clock_t start = clock();
    uint64_t nodes = perft(b, depth);
    double secs = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("perft(%d) = %" PRIu64 " in %.3fs (%.1f Mnodes/s)\n",
           depth, nodes, secs, nodes / secs / 1e6);
    return 0;
}
