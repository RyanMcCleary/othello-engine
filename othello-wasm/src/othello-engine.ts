/// <reference types="vite/client" />

// Main-thread API for the Othello engine. Wraps the worker in promises.
//
//   import { OthelloEngine } from './othello-engine.js';
//   const engine = new OthelloEngine();
//   let { black, white } = await engine.initial();
//   const sq = await engine.bestMove(black, white, true, { timeMs: 1000 });
//   ({ black, white } = await engine.apply(black, white, true, sq));
//
// Bitboards are BigInt (bit i = file (i%8), rank (i/8); a1 = 0, h8 = 63).
type PositionBits = { black: bigint; white: bigint };

type PendingCall<T = unknown> = {
  resolve: (value: T) => void;
  reject: (reason?: unknown) => void;
};

export class OthelloEngine {
  private worker: Worker;
  private id = 0;
  private pending = new Map<number, PendingCall>();

  constructor(workerUrl = new URL(`${import.meta.env.BASE_URL}engine-worker.js`, import.meta.url)) {
    this.worker = new Worker(workerUrl, { type: 'module' });
    this.worker.onmessage = (e) => {
      const { id, result, error } = e.data;
      const p = this.pending.get(id);
      if (!p) return;
      this.pending.delete(id);
      if (error) p.reject(new Error(error));
      else p.resolve(result);
    };
  }

  private call<T>(type: string, payload = {}): Promise<T> {
    const id = ++this.id;
    return new Promise((resolve, reject) => {
      this.pending.set(id, { resolve, reject } as PendingCall);
      this.worker.postMessage({ id, type, ...payload });
    });
  }

  /** Starting position: { black, white } as BigInt bitboards. */
  initial(): Promise<PositionBits> {
    return this.call('initial');
  }

  /** Legal moves for the side to move, as a BigInt bitboard. */
  legalMoves(black: bigint, white: bigint, blackToMove: boolean): Promise<bigint> {
    return this.call('legalMoves', { black, white, blackToMove });
  }

  /** Static evaluation (side-to-move relative). */
  evaluate(black: bigint, white: bigint, blackToMove: boolean): Promise<number> {
    return this.call('evaluate', { black, white, blackToMove });
  }

  /**
   * Best move square (0..63), or -1 to pass. Time-bounded by `timeMs`
   * (capped at `depth` plies); set timeMs = 0 for a fixed-depth search.
   */
  bestMove(black: bigint, white: bigint, blackToMove: boolean, { depth = 20, timeMs = 1000 } = {}): Promise<number> {
    return this.call('bestMove', { black, white, blackToMove, depth, timeMs });
  }

  /** Apply move `sq`, returning the new { black, white }. */
  apply(black: bigint, white: bigint, blackToMove: boolean, sq: number): Promise<PositionBits> {
    return this.call('apply', { black, white, blackToMove, sq });
  }

  terminate() {
    this.worker.terminate();
  }
}
