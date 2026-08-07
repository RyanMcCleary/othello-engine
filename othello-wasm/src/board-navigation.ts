import { arrowTarget } from './board-look';
import { isArrowKey } from './view-chord';

type BoardNavigationOptions = {
  getFocusedSq: () => number;
  focusSquare: (sq: number) => void;
};

const ARROW_CHORD_DELAY_MS = 150;

export class BoardNavigationController {
  private heldArrowKeys = new Set<string>();
  private chordStartSq: number | null = null;
  private pendingMove: { origin: number; keys: Set<string>; timer: ReturnType<typeof setTimeout> } | null = null;

  constructor(private readonly options: BoardNavigationOptions) {}

  keydown(e: KeyboardEvent) {
    if (!isArrowKey(e.key)) return false;
    e.preventDefault();
    this.handleArrowKeydown(e);
    return true;
  }

  keyup(e: KeyboardEvent) {
    if (!isArrowKey(e.key)) return false;
    this.heldArrowKeys.delete(e.key);
    if (this.heldArrowKeys.size) this.chordStartSq = this.options.getFocusedSq();
    else if (!this.pendingMove) this.chordStartSq = null;
    return true;
  }

  clear() {
    this.heldArrowKeys.clear();
    this.cancelPending();
    this.chordStartSq = null;
  }

  cancelPending() {
    if (!this.pendingMove) return;
    clearTimeout(this.pendingMove.timer);
    this.pendingMove = null;
  }

  origin() {
    return this.pendingMove?.origin ?? this.chordStartSq ?? this.options.getFocusedSq();
  }

  heldKeys() {
    return this.heldArrowKeys;
  }

  private handleArrowKeydown(e: KeyboardEvent) {
    if (!this.heldArrowKeys.size && this.pendingMove) this.runPendingMove();

    const wasHoldingKey = this.heldArrowKeys.has(e.key);
    const heldCount = this.heldArrowKeys.size;
    if (!heldCount) this.chordStartSq = this.options.getFocusedSq();
    this.heldArrowKeys.add(e.key);

    if (e.repeat || wasHoldingKey) {
      this.cancelPending();
      this.focusTarget(this.options.getFocusedSq());
      this.chordStartSq = this.options.getFocusedSq();
      return;
    }

    if (heldCount === 0) {
      this.scheduleSingleArrowMove();
      return;
    }

    this.focusTarget(this.origin());
    this.cancelPending();
    this.chordStartSq = this.options.getFocusedSq();
  }

  private scheduleSingleArrowMove() {
    this.cancelPending();
    this.pendingMove = {
      origin: this.chordStartSq ?? this.options.getFocusedSq(),
      keys: new Set(this.heldArrowKeys),
      timer: setTimeout(() => this.runPendingMove(), ARROW_CHORD_DELAY_MS),
    };
  }

  private runPendingMove() {
    const pending = this.pendingMove;
    if (!pending) return;
    this.pendingMove = null;
    this.options.focusSquare(arrowTarget(pending.origin, pending.keys));
    this.chordStartSq = this.heldArrowKeys.size ? this.options.getFocusedSq() : null;
  }

  private focusTarget(origin: number) {
    this.options.focusSquare(arrowTarget(origin, this.heldArrowKeys));
  }
}
