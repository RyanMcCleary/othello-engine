import { bitAt } from './bitboard';
import type { DiscColor, MoveSummary, Position, PositionBits } from './types';

export function buildMove(before: Position, after: PositionBits, sq: number, blackToMove: boolean): MoveSummary {
  const color: DiscColor = blackToMove ? 'black' : 'white';
  const flipped: number[] = [];
  const opponentBefore = blackToMove ? before.white : before.black;
  const moverAfter = blackToMove ? after.black : after.white;

  for (let i = 0; i < 64; i++) {
    if (i !== sq && bitAt(opponentBefore, i) && bitAt(moverAfter, i)) flipped.push(i);
  }

  flipped.sort((a, b) => flipDistance(a, sq) - flipDistance(b, sq) || a - b);
  return { sq, color, flips: flipped, black: after.black, white: after.white };
}

export function flipDistance(a: number, b: number) {
  return Math.max(Math.abs((a % 8) - (b % 8)), Math.abs(Math.floor(a / 8) - Math.floor(b / 8)));
}
