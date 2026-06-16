#include "board.h"

Board board_initial(void) {
    /* Black on d5/e4, White on d4/e5; Black (player) to move. */
    Board b;
    b.player   = square_mask(4, 3) | square_mask(3, 4); /* d5, e4 */
    b.opponent = square_mask(3, 3) | square_mask(4, 4); /* d4, e5 */
    return b;
}

bitboard board_moves(Board b) {
    return all_moves(b.opponent, b.player);
}

bool board_has_moves(Board b) {
    return board_moves(b) != 0;
}

bool board_terminal(Board b) {
    /* Over only when *neither* side can move. */
    return all_moves(b.opponent, b.player) == 0 &&
           all_moves(b.player, b.opponent) == 0;
}

Board board_play(Board b, bitboard move) {
    bitboard flipped = flip_all(b.opponent, b.player, move);
    bitboard new_player   = b.player | move | flipped;
    bitboard new_opponent = b.opponent & ~flipped;
    /* Swap: the opponent becomes the side to move. */
    Board next;
    next.player   = new_opponent;
    next.opponent = new_player;
    return next;
}

Board board_pass(Board b) {
    Board next;
    next.player   = b.opponent;
    next.opponent = b.player;
    return next;
}

int board_disc_diff(Board b) {
    return (int)count_ones(b.player) - (int)count_ones(b.opponent);
}
