import { discAt } from './bitboard';
import type { PositionBits } from './types';

export function arrowTarget(sq: number, keys: Iterable<string>) {
  const { dRow, dCol } = arrowDelta(keys);
  const row = Math.floor(sq / 8);
  const col = sq % 8;
  if (!dRow && !dCol) return sq;
  const nextRow = Math.max(0, Math.min(7, row + dRow));
  const nextCol = Math.max(0, Math.min(7, col + dCol));
  return nextRow * 8 + nextCol;
}

export function arrowDelta(keys: Iterable<string>) {
  const keySet = keys instanceof Set ? keys : new Set(keys);
  return {
    dRow: (keySet.has('ArrowDown') ? 1 : 0) - (keySet.has('ArrowUp') ? 1 : 0),
    dCol: (keySet.has('ArrowRight') ? 1 : 0) - (keySet.has('ArrowLeft') ? 1 : 0),
  };
}

export function directionName(dRow: number, dCol: number) {
  const rowPart = dRow < 0 ? 'north' : dRow > 0 ? 'south' : '';
  const colPart = dCol < 0 ? 'west' : dCol > 0 ? 'east' : '';
  return rowPart + colPart;
}

export function directionalLookSummary(state: PositionBits, sq: number, dRow: number, dCol: number) {
  const runs: string[] = [];
  let current: string | null = null;
  let count = 0;
  let row = Math.floor(sq / 8) + dRow;
  let col = (sq % 8) + dCol;

  while (row >= 0 && row < 8 && col >= 0 && col < 8) {
    const occupant = discAt(state, row * 8 + col);
    const kind = occupant ? `${occupant} disk` : 'empty square';
    if (kind === current) {
      count++;
    } else {
      if (current) runs.push(runPhrase(count, current));
      current = kind;
      count = 1;
    }
    row += dRow;
    col += dCol;
  }

  if (current) runs.push(runPhrase(count, current));
  return runs.length ? runs.join(' followed by ') : 'Board edge';
}

function runPhrase(count: number, singular: string) {
  const noun = count === 1 ? singular : singular.replace('disk', 'disks').replace('square', 'squares');
  return `${spokenCount(count)} ${noun}`;
}

function spokenCount(count: number) {
  return count === 1 ? 'one' : String(count);
}
