import { popcount, sqName } from './bitboard';
import type { DiscColor, MoveActor, MoveSummary } from './types';

export const spokenSqName = sqName;

export const sideName = (blackToMove: boolean) => blackToMove ? 'Black' : 'White';

export const colorName = (color: DiscColor | string) => color === 'black' ? 'Black' : 'White';

export const plural = (n: number, word: string) => `${n} ${word}${n === 1 ? '' : 's'}`;

export function scoreText(black: bigint, white: bigint) {
  return `Score Black ${popcount(black)}. White ${popcount(white)}.`;
}

export function moveClause(move: MoveSummary, actor: MoveActor, sentenceStart = false) {
  const subject = actorSubject(actor, sentenceStart);
  const verb = actor === 'Hint' ? 'played' : 'put';
  return `${subject} ${verb} a ${colorName(move.color).toLowerCase()} disk on ${spokenSqName(move.sq)}, ${directionalFlipSummary(move, false)}`;
}

export function potentialMoveSummary(move: MoveSummary) {
  return `${directionalFlipSummary(move)}.`;
}

export function directionalFlipSummary(move: MoveSummary, sentenceStart = true) {
  const counts = new Map<string, number>();
  for (const sq of move.flips) {
    const direction = flipDirection(move.sq, sq);
    counts.set(direction, (counts.get(direction) || 0) + 1);
  }

  const verb = sentenceStart ? 'Flips' : 'flipping';
  const parts = DIRECTION_ORDER
    .filter((direction) => counts.has(direction))
    .map((direction) => `${plural(counts.get(direction) || 0, 'disk')} ${direction}`);
  return parts.length ? `${verb} ${spokenList(parts)}` : `${verb} no disks`;
}

export function spokenList(items: string[], empty = 'nothing') {
  if (items.length <= 1) return items[0] || empty;
  if (items.length === 2) return `${items[0]} and ${items[1]}`;
  return `${items.slice(0, -1).join(', ')}, and ${items[items.length - 1]}`;
}

function actorSubject(actor: MoveActor, sentenceStart = false) {
  if (actor === 'You') return sentenceStart ? 'You' : 'you';
  if (actor === 'Computer') return sentenceStart ? 'The computer' : 'the computer';
  return sentenceStart ? 'The hint' : 'the hint';
}

const DIRECTION_ORDER = [
  'north', 'northeast', 'east', 'southeast',
  'south', 'southwest', 'west', 'northwest',
];

function flipDirection(fromSq: number, toSq: number) {
  const fromRow = Math.floor(fromSq / 8);
  const fromCol = fromSq % 8;
  const toRow = Math.floor(toSq / 8);
  const toCol = toSq % 8;
  const rowPart = toRow < fromRow ? 'north' : toRow > fromRow ? 'south' : '';
  const colPart = toCol < fromCol ? 'west' : toCol > fromCol ? 'east' : '';
  return rowPart + colPart;
}
