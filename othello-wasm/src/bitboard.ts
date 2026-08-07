import type { DiscColor, PositionBits } from './types';

// Treat bitboards as unsigned 64-bit; a signed BigInt would make popcount loop forever.
export const u64 = (bb: bigint) => BigInt.asUintN(64, bb);

export const bitAt = (bb: bigint, sq: number) => (u64(bb) >> BigInt(sq)) & 1n;

export function popcount(bb: bigint) {
  bb = u64(bb);
  let count = 0;
  while (bb) {
    bb &= bb - 1n;
    count++;
  }
  return count;
}

export function sqName(sq: number) {
  return sq < 0 ? 'pass' : String.fromCharCode(97 + (sq % 8)) + (Math.floor(sq / 8) + 1);
}

export function discAt(state: PositionBits, sq: number): DiscColor | null {
  if (bitAt(state.black, sq)) return 'black';
  if (bitAt(state.white, sq)) return 'white';
  return null;
}

export function firstBit(bb: bigint) {
  for (let sq = 0; sq < 64; sq++) {
    if (bitAt(bb, sq)) return sq;
  }
  return -1;
}
