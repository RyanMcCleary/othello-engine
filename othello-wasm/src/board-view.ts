import { bitAt, discAt, firstBit, sqName } from './bitboard';
import type { MoveSummary, Position } from './types';

type BoardViewOptions = {
  boardEl: HTMLElement;
  onCellClick: (sq: number) => void;
  onCellKeydown: (e: KeyboardEvent, sq: number) => void;
};

type RenderOptions = {
  state: Position;
  legalMoves: bigint;
  move: MoveSummary | null;
  playableNow: boolean;
  showHints: boolean;
  animationsEnabled: boolean;
  flipDelay: (index: number) => number;
  squareLabel: (sq: number, legalMoves: bigint) => string;
};

export class BoardView {
  private cells: HTMLDivElement[] = [];
  private deferFocusedCellAccessibility = false;

  focusedSq = 0;

  constructor(private readonly options: BoardViewOptions) {
    this.buildBoard();
  }

  focusSquare(sq: number) {
    const previousSq = this.focusedSq;
    this.focusedSq = Math.max(0, Math.min(63, sq));
    this.updateTabStops();
    this.cells[this.focusedSq]?.focus({ preventScroll: true });
    if (this.focusedSq !== previousSq) this.flushDeferredFocusedCellAccessibility(previousSq);
  }

  containsActiveElement() {
    return this.options.boardEl.contains(document.activeElement);
  }

  deferFocusedCellAccessibilityUntilNavigation() {
    if (this.containsActiveElement()) this.deferFocusedCellAccessibility = true;
  }

  flushDeferredFocusedCellAccessibility(sq = this.focusedSq, legalMoves = 0n, squareLabel?: (sq: number, legalMoves: bigint) => string) {
    if (!this.deferFocusedCellAccessibility) return;
    this.deferFocusedCellAccessibility = false;
    if (squareLabel) this.setCellAccessibility(sq, legalMoves, squareLabel);
  }

  render(options: RenderOptions) {
    const { state, legalMoves, move, playableNow, showHints, animationsEnabled, flipDelay, squareLabel } = options;
    const animatedMove = animationsEnabled ? move : null;
    const flipped = new Map((animatedMove?.flips || []).map((sq, i) => [sq, i]));

    if (!this.containsActiveElement()) {
      const firstLegal = playableNow ? firstBit(legalMoves) : -1;
      if (firstLegal >= 0) this.focusedSq = firstLegal;
    }

    for (let sq = 0; sq < 64; sq++) {
      const cell = this.cells[sq];
      const occupant = discAt(state, sq);
      const canPlay = playableNow && !occupant && !!bitAt(legalMoves, sq);

      cell.className = 'cell';
      cell.replaceChildren();
      cell.removeAttribute('style');
      cell.tabIndex = sq === this.focusedSq ? 0 : -1;
      cell.setAttribute('aria-disabled', canPlay ? 'false' : 'true');
      if (!this.shouldPreserveFocusedCellAccessibility(sq, cell)) {
        this.setCellAccessibility(sq, legalMoves, squareLabel);
      }

      if (occupant) {
        const disc = document.createElement('div');
        disc.className = `disc ${occupant}`;
        disc.setAttribute('aria-hidden', 'true');
        if (animatedMove?.sq === sq) disc.classList.add('placed');
        if (flipped.has(sq)) {
          disc.classList.add('flipped', `to-${occupant}`);
          disc.style.setProperty('--flip-delay', `${flipDelay(flipped.get(sq) ?? 0)}ms`);
        }
        cell.append(disc);
      } else if (canPlay) {
        cell.classList.add('playable');
        if (showHints) cell.classList.add('hint');
      }

      if (sq === state.lastMove) cell.classList.add('last');
    }
  }

  private buildBoard() {
    for (let row = 0; row < 8; row++) {
      const rowEl = document.createElement('div');
      rowEl.className = 'board-row';

      for (let col = 0; col < 8; col++) {
        const sq = row * 8 + col;
        const cell = document.createElement('div');
        cell.id = `sq-${sqName(sq)}`;
        cell.className = 'cell';
        cell.tabIndex = sq === this.focusedSq ? 0 : -1;
        cell.addEventListener('click', () => this.options.onCellClick(sq));
        cell.addEventListener('keydown', (e) => this.options.onCellKeydown(e, sq));
        rowEl.appendChild(cell);
        this.cells.push(cell);
      }

      this.options.boardEl.appendChild(rowEl);
    }
  }

  private updateTabStops() {
    for (let sq = 0; sq < 64; sq++) {
      this.cells[sq].tabIndex = sq === this.focusedSq ? 0 : -1;
    }
  }

  private shouldPreserveFocusedCellAccessibility(sq: number, cell: HTMLElement) {
    return this.deferFocusedCellAccessibility && sq === this.focusedSq && document.activeElement === cell;
  }

  private setCellAccessibility(sq: number, legalMoves: bigint, squareLabel: (sq: number, legalMoves: bigint) => string) {
    const cell = this.cells[sq];
    if (!cell) return;
    cell.setAttribute('aria-label', squareLabel(sq, legalMoves));
  }
}
