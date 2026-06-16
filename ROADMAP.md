# Othello Engine — Performance & Strength Roadmap

## Context

Goal: generate **strong** Othello moves **efficiently enough to run in a browser
on modest phones/tablets**. The engine is currently a minimal, clean C core
(~360 lines): bitboard move generation, ray-based flipping, minmax/alpha-beta
with a mobility-only float heuristic, and a trivial test `main`. There is no
build system (a compiled `a.out` is checked in), and `othello-wasm/` is empty.

Constraint: get strong using **classical techniques only** — bitboards,
alpha-beta/PVS, transposition tables, an exact endgame solver, and a
hand-crafted (optionally self-play-tuned) evaluation. **No ML / no external
datasets** (so no learned pattern weights à la Edax/Egaroucid).

Reference engines for technique (not for data): Edax (abulmo/edax-reversi) and
Egaroucid — both bitboard alpha-beta engines; Egaroucid runs in the browser via
WASM. Bitboard/flip tricks: okuhara's bitboard page.

---

## Known issues to fix (found in review)

**Correctness**
- `best_move` loop condition tests `moves` not `move` → the **last legal move is
  never evaluated** (`src/search.c:114`).
- `best_move` recurses with `maximizing=true` for the child and isn't
  negamax-consistent → **inverted turn handling** (`src/search.c:116`).
- **No pass handling**: when the side to move has no moves but the game isn't
  over, the move loop body never runs and the node returns `EVAL_MIN/MAX`
  instead of passing to the opponent (`src/search.c` minmax/alphabeta).
- `mobility_hueristic` **divides by zero → NaN** when neither side has a move
  (`src/search.c:39`).
- Eval must be **side-to-move relative** (negamax); today it is computed from a
  fixed perspective while the search swaps player/opponent each ply.

**Performance**
- `generalized_ray_flip` walks each direction with a `while` loop — replace with
  a **branchless flip** (the search's hottest function).
- `all_moves` recomputed up to 3× per node (`check_win` calls it twice + the node
  body). Compute once and thread it through.
- `float` eval → use **int** for speed and exact comparisons.

**Infra**
- Remove committed `src/a.out`; add `.gitignore`. Add a real build system.

---

## Phase 0 — Foundations, correctness, measurement

- **Build system**: `Makefile` with a native dev/test target (`-O3
  -march=native`) and an Emscripten target. Remove `a.out`, add `.gitignore`.
- **Core types/API**: a `Board { bitboard player, opponent; }` and a clean,
  reusable move API — `legal_moves(board)`, `flip(board, sq)`, `play(board, sq)`,
  `count_ones` (already present). Single source of truth for move gen.
- **Negamax refactor**: convert search to negamax with an **int**, side-to-move
  relative eval. Fix the `best_move` last-move + turn bugs and add pass handling
  (pass = swap sides with no flips; two passes in a row = terminal).
- **Perft harness**: leaf-node count to depth N from the start position — both a
  correctness check (validate the small values: perft 1=4, 2=12, 3=56, 4=244,
  5=1396, 6=8200, and compare deeper values against a published reference) and
  the primary NPS benchmark. Egaroucid/Edax use perft the same way.
- **Unit tests**: flip in all 8 directions, edge/wrap cases, a few known
  positions; wire into `make test`.

_Exit criteria: `make test` green, perft numbers match reference, baseline NPS
recorded._

## Phase 1 — Fast move generation & flip

- Replace the ray-loop flip with a **branchless flip**:
  - Portable default: **Kogge-Stone parallel-prefix** flood (O(log) shifts, no
    loops/branches) for both flip and mobility.
  - Keep a clean scalar implementation as the reference/fallback.
- Optionally add a **WASM SIMD128** (`wasm_simd128.h`) path computing multiple
  directions in parallel (Edax reports ~1.5× from SIMD), guarded behind a build
  flag with the scalar fallback.
- Re-run perft to confirm identical results and measure the NPS gain.

_Exit criteria: identical perft, measurable NPS improvement over Phase 0._

## Phase 2 — Search

- **NegaScout/PVS** with null-window search on top of alpha-beta.
- **Transposition table**: Zobrist hashing; store best move, bound type, depth;
  depth-preferred replacement. Size configurable (keep modest for mobile, e.g.
  8–64 MB).
- **Move ordering**: TT move first, then a cheap static order (corner
  preference, fewest opponent replies / opponent mobility, position weights).
  **Iterative deepening** to seed the TT and ordering, and to enable time limits.
- **Exact endgame solver**: when empties ≤ N (tune ~10–16), switch to a
  specialized full-depth solver (no eval — solve for disc differential / win-loss)
  with optimized "last few empties" generation and **parity-based** ordering.
  This is where the engine plays perfectly at the end.
- (Deferred / optional) Multi-ProbCut forward pruning — its cut thresholds come
  from **self-play** shallow-vs-deep correlation (no external data needed), but
  defer until the rest is solid.

_Exit criteria: beats the Phase 0 engine decisively in self-play at equal time;
endgame solver returns exact results on known positions._

## Phase 3 — Evaluation (hand-crafted, no ML)

Side-to-move relative, integer, **phase-blended** (opening → endgame):
- Mobility (current **and** potential mobility).
- Corner occupancy; X/C-square (corner-adjacency) penalties.
- **Stable discs** (discs that can never be flipped) — strong, especially edges.
- Parity (who is forced to play last in empty regions).
- Disc-count differential, weighted up only in the late game.
- Weights hand-tuned first; optionally **self-play tuned** later (still no
  external data).

_Exit criteria: noticeably stronger play in self-play vs Phase 2's simple eval._

## Phase 4 — WASM packaging & JS API

- **Emscripten** build → `othello.wasm` + JS glue in `othello-wasm/`.
- Export a small C ABI via `cwrap`/`EXPORTED_FUNCTIONS` (or embind):
  `init`, `legalMoves`, `applyMove`, `evaluate`,
  `bestMove(blackBB, whiteBB, sideToMove, timeMs|depth)`.
- Run the search in a **Web Worker** so the UI never blocks; use **time-limited
  iterative deepening** for responsiveness on phones.
- Keep TT modest and configurable. Default to **single-thread** WASM for
  compatibility; SIMD optional, threads (which need cross-origin isolation)
  optional later.

_Exit criteria: a JS demo calls `bestMove` off the main thread and returns a
legal, strong move within a target time budget on a throttled mobile profile._

## Phase 5 — Minimal web UI

- Static HTML/JS board, click-to-play, engine in the worker, difficulty mapped to
  time/depth. Just enough to demo and play-test the engine in a browser.

---

## Verification (end to end)

- `make test` → unit tests + perft (validate against reference perft values).
- Benchmark NPS native (Phase 0 baseline vs after each phase).
- Self-play match between engine versions at equal time control to confirm each
  phase is a real strength gain.
- Endgame solver checked against known exact positions.
- Browser: load the demo page, play a full game, measure per-move latency under a
  throttled CPU profile (simulated mid-range phone).

## Rough performance targets (modest phone, single-thread WASM)

- Midgame: iterative-deepening best move within ~1 s.
- Endgame: exact solve from ~18–22 empties within a second or two (device
  dependent; improves with TT + parity ordering + stability cutoffs).
