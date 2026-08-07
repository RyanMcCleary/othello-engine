export type DiscColor = 'black' | 'white';
export type MoveActor = 'You' | 'Computer' | 'Hint';

export type Position = {
  black: bigint;
  white: bigint;
  blackToMove: boolean;
  lastMove: number;
};

export type PositionBits = {
  black: bigint;
  white: bigint;
};

export type MoveSummary = {
  sq: number;
  color: DiscColor;
  flips: number[];
  black: bigint;
  white: bigint;
};

export type PendingHumanMove = {
  move: MoveSummary;
  actor: 'You' | 'Hint';
};
