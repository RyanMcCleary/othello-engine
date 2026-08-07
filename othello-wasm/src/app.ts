import { OthelloEngine } from './othello-engine';
import { loadSettings, saveSettings, levelMs, resolveColor, LEVELS, type ColorSetting, type LevelKey, type Settings } from './config';
import { ViewChordController, isArrowKey } from './view-chord';
import { bitAt, discAt, popcount, sqName, u64 } from './bitboard';
import { buildMove } from './moves';
import { MoveAudio, type Family as SoundFamily } from './move-audio';
import { arrowDelta, directionName, directionalLookSummary } from './board-look';
import { BoardNavigationController } from './board-navigation';
import { BoardView } from './board-view';
import {
  colorName,
  moveClause,
  plural,
  potentialMoveSummary,
  scoreText,
  sideName,
  spokenList,
  spokenSqName,
} from './announcements';
import type { MoveActor, MoveSummary, PendingHumanMove, Position } from './types';

const engine = new OthelloEngine();

// ---- DOM ----
const boardEl = getElement<HTMLElement>('board');
const announcerEl = getElement<HTMLElement>('sr-announcer');
const statusEl = getElement<HTMLElement>('status');
const bannerEl = getElement<HTMLElement>('banner');
const blackNum = getElement<HTMLElement>('black-num');
const whiteNum = getElement<HTMLElement>('white-num');
const blackScore = getElement<HTMLElement>('black-score');
const whiteScore = getElement<HTMLElement>('white-score');
const movesEl = getElement<HTMLElement>('moves');
const undoBtn = getElement<HTMLButtonElement>('undo');
const redoBtn = getElement<HTMLButtonElement>('redo');
const hintBtn = getElement<HTMLButtonElement>('hint');
const newBtn = getElement<HTMLButtonElement>('new-game');
const levelSel = getElement<HTMLSelectElement>('level');
const colorSel = getElement<HTMLSelectElement>('color');

function getElement<T extends HTMLElement>(id: string): T {
  const element = document.getElementById(id);
  if (!element) throw new Error(`Missing #${id}`);
  return element as T;
}

// ---- helpers ----
const delay = (ms: number) => new Promise((resolve) => setTimeout(resolve, ms));

const PLACE_DURATION_MS = 180;
const FLIP_DELAY_MS = 220;
const FLIP_STAGGER_MS = 420;
const FLIP_DURATION_MS = 320;
const TURN_SOUND_GAP_MS = 1000;
const PLACE_SOUND_START_MS = 30;
const SAMPLE_HIT_DURATION_MS = 720;
const SOUND_FINISH_PAD_MS = 120;

const moveAudio = new MoveAudio({
  soundEnabled: () => settings.soundEffects,
  sampleHitDurationMs: SAMPLE_HIT_DURATION_MS,
  placeSoundStartMs: PLACE_SOUND_START_MS,
  flipDelayMs: FLIP_DELAY_MS,
  flipStaggerMs: FLIP_STAGGER_MS,
  soundFinishPadMs: SOUND_FINISH_PAD_MS,
});

const reduceMotion = window.matchMedia?.('(prefers-reduced-motion: reduce)');

function animationsEnabled() {
  return settings.animate && !reduceMotion?.matches;
}

function applySettingsToBody() {
  document.body.classList.toggle('no-anim', !animationsEnabled());
}

// ---- board input and view ----
const boardView = new BoardView({
  boardEl,
  onCellClick,
  onCellKeydown,
});

const boardNavigation = new BoardNavigationController({
  getFocusedSq: () => boardView.focusedSq,
  focusSquare: (sq) => boardView.focusSquare(sq),
});

const viewChords = new ViewChordController({
  // The look command should describe the ray from the currently focused cell,
  // not from wherever a pending board-navigation chord started.
  getOrigin: () => boardView.focusedSq,
  getHeldArrowKeys: () => boardNavigation.heldKeys(),
  onLook: (origin, keys) => announceDirectionalLook(origin, keys),
  onViewStarted: () => boardNavigation.cancelPending(),
});

function onCellKeydown(e: KeyboardEvent, sq: number) {
  if (isArrowKey(e.key)) {
    boardNavigation.keydown(e);
    return;
  }

  const row = Math.floor(sq / 8);
  const col = sq % 8;
  let next = sq;

  switch (e.key) {
    case 'Home': next = e.ctrlKey || e.metaKey ? 0 : row * 8; break;
    case 'End': next = e.ctrlKey || e.metaKey ? 63 : row * 8 + 7; break;
    case 'Enter':
    case ' ':
      e.preventDefault();
      onCellClick(boardView.focusedSq);
      return;
    default:
      if (handleLegalMoveShortcut(e)) return;
      return;
  }

  e.preventDefault();
  boardView.focusSquare(next);
}

function announceDirectionalLook(sq: number, keys: Set<string>) {
  const { dRow, dCol } = arrowDelta(keys);
  if (!dRow && !dCol) return;
  announce(`${directionalLookSummary(cur(), sq, dRow, dCol)} ${directionName(dRow, dCol)}.`);
}

function handleLegalMoveShortcut(e: KeyboardEvent) {
  if (e.altKey || e.ctrlKey || e.metaKey) return false;

  const key = e.key.toLowerCase();
  if (key === 'n' || key === ']') {
    e.preventDefault();
    focusLegalMove(1);
    return true;
  }
  if (key === 'p' || key === '[') {
    e.preventDefault();
    focusLegalMove(-1);
    return true;
  }
  if (key === 'f') {
    if (!canHumanMoveNow() || !bitAt(currentLegalMoves, boardView.focusedSq)) return false;
    e.preventDefault();
    void announcePotentialFlips(boardView.focusedSq);
    return true;
  }
  if (key === 'z') {
    e.preventDefault();
    undo();
    return true;
  }
  if (key === 'y') {
    e.preventDefault();
    redo();
    return true;
  }
  return false;
}

async function announcePotentialFlips(sq: number) {
  if (!canHumanMoveNow() || !bitAt(currentLegalMoves, sq)) return;

  const myGen = gen;
  const before = cur();
  const after = await engine.apply(before.black, before.white, before.blackToMove, sq);
  if (myGen !== gen || cur() !== before || !canHumanMoveNow() || !bitAt(currentLegalMoves, sq)) return;

  announce(potentialMoveSummary(buildMove(before, after, sq, before.blackToMove)));
}

function focusLegalMove(delta: number) {
  const moves = legalMoveSquares();
  if (!moves.length) {
    announce('No legal moves.', 'assertive');
    return;
  }

  const currentIndex = moves.indexOf(boardView.focusedSq);
  if (currentIndex >= 0) {
    boardView.focusSquare(moves[(currentIndex + delta + moves.length) % moves.length]);
    return;
  }

  if (delta > 0) {
    const nextIndex = moves.findIndex((moveSq) => moveSq > boardView.focusedSq);
    boardView.focusSquare(moves[nextIndex >= 0 ? nextIndex : 0]);
    return;
  }

  for (let i = moves.length - 1; i >= 0; i--) {
    if (moves[i] < boardView.focusedSq) {
      boardView.focusSquare(moves[i]);
      return;
    }
  }
  boardView.focusSquare(moves[moves.length - 1]);
}

function legalMoveSquares() {
  const moves = [];
  for (let sq = 0; sq < 64; sq++) {
    if (bitAt(currentLegalMoves, sq)) moves.push(sq);
  }
  return moves;
}

function clearHeldArrowKeys() {
  boardNavigation.clear();
  viewChords.clear();
}

function deferFocusedCellAccessibilityUntilNavigation() {
  boardView.deferFocusedCellAccessibilityUntilNavigation();
}

function flushDeferredFocusedCellAccessibility(sq = boardView.focusedSq) {
  boardView.flushDeferredFocusedCellAccessibility(sq, currentLegalMoves, squareLabel);
}

// ---- game state ----
let settings: Settings = loadSettings();
let humanIsBlack = true;
let history: Position[] = [];
let cursor = 0;
let busy = false;
let gameOver = false;
let gen = 0;        // bumps on New Game to cancel stale async work
let currentLegalMoves = 0n;
let pendingHumanMove: PendingHumanMove | null = null;

const cur = () => history[cursor];
const atHead = () => cursor === history.length - 1;
const isHumanTurn = () => history.length > 0 && cur().blackToMove === humanIsBlack;
const canHumanMoveNow = () => history.length > 0 && !gameOver && !busy && isHumanTurn();

function pushPly(black: bigint, white: bigint, blackToMove: boolean, lastMove: number) {
  history = history.slice(0, cursor + 1);
  history.push({ black, white, blackToMove, lastMove });
  cursor = history.length - 1;
}

async function newGame() {
  const myGen = ++gen;
  busy = true; gameOver = false; currentLegalMoves = 0n;
  pendingHumanMove = null;
  clearHeldArrowKeys();
  settings = loadSettings();
  applySettingsToBody();
  syncControls();
  humanIsBlack = resolveColor(settings.color) === 'black';
  bannerEl.hidden = true;
  setStatus('Loading engine...', true);

  const { black, white } = await engine.initial();
  if (myGen !== gen) return;
  history = [{ black, white, blackToMove: true, lastMove: -1 }];
  cursor = 0;
  boardView.focusedSq = 0;
  busy = false;
  render(0n);
  announce(`New game. You are ${humanIsBlack ? 'Black' : 'White'}. Black moves first.`);
  tick({ quietStatus: true });
}

async function tick({ quietStatus = false } = {}) {
  const myGen = gen;
  if (gameOver) return;
  const s = cur();
  const myMoves = await engine.legalMoves(s.black, s.white, s.blackToMove);
  if (myGen !== gen) return;

  if (!atHead() && !isHumanTurn()) {
    render(0n);
    setStatus('Review mode. Redo to continue.');
    return;
  }

  if (myMoves === 0n) {
    const oppMoves = await engine.legalMoves(s.black, s.white, !s.blackToMove);
    if (myGen !== gen) return;
    if (oppMoves === 0n) { endGame(); return; }
    setStatus(`${sideName(s.blackToMove)} passes because there is no legal move.`, false, !quietStatus);
    announcePass(s.blackToMove);
    pushPly(s.black, s.white, !s.blackToMove, -1);
    render(0n);
    setTimeout(() => tick({ quietStatus: true }), 700);
    return;
  }

  if (isHumanTurn()) {
    render(myMoves);
    setStatus(`Your move for ${sideName(s.blackToMove)}. ${plural(popcount(myMoves), 'legal move')}.`, false, !quietStatus);
  } else {
    busy = true;
    setStatus(`Computer thinking for ${sideName(s.blackToMove)}.`, true, !quietStatus);
    render(0n);
    const sq = await engine.bestMove(s.black, s.white, s.blackToMove,
                                     { depth: 24, timeMs: levelMs(settings.level) });
    if (myGen !== gen) return;
    if (sq < 0) { busy = false; tick(); return; }
    const after = await engine.apply(s.black, s.white, s.blackToMove, sq);
    if (myGen !== gen) return;
    const move = buildMove(s, after, sq, s.blackToMove);
    pushPly(after.black, after.white, !s.blackToMove, sq);
    render(0n, { move });
    await playMoveSoundAndAnimation(move, 'computer');
    if (myGen !== gen) return;
    announceComputerMove(move);
    busy = false;
    tick({ quietStatus: true });
  }
}

async function onCellClick(sq: number) {
  boardView.focusSquare(sq);

  if (gameOver || busy || !isHumanTurn()) {
    announce(unavailableMessage(sq, currentLegalMoves), 'assertive');
    return;
  }

  const myGen = gen;
  const s = cur();
  const moves = await engine.legalMoves(s.black, s.white, s.blackToMove);
  if (myGen !== gen) return;
  render(moves);

  if (!bitAt(moves, sq)) {
    announce(unavailableMessage(sq, moves), 'assertive');
    return;
  }

  busy = true;
  const after = await engine.apply(s.black, s.white, s.blackToMove, sq);
  if (myGen !== gen) return;
  const move = buildMove(s, after, sq, s.blackToMove);
  pushPly(after.black, after.white, !s.blackToMove, sq);
  deferFocusedCellAccessibilityUntilNavigation();
  render(0n, { move });
  pendingHumanMove = { move, actor: 'You' };
  await playMoveSoundAndAnimation(move, 'player');
  if (myGen !== gen) return;
  await delay(TURN_SOUND_GAP_MS);
  if (myGen !== gen) return;
  busy = false;
  tick({ quietStatus: true });
}

async function engineHint() {
  if (gameOver || busy || !isHumanTurn()) return;
  const myGen = gen;
  const s = cur();
  setStatus('Finding hint...', true);
  busy = true;
  render(0n);
  const sq = await engine.bestMove(s.black, s.white, s.blackToMove,
                                   { depth: 24, timeMs: levelMs(settings.level) });
  if (myGen !== gen || sq < 0) return;
  const after = await engine.apply(s.black, s.white, s.blackToMove, sq);
  if (myGen !== gen) return;
  const move = buildMove(s, after, sq, s.blackToMove);
  pushPly(after.black, after.white, !s.blackToMove, sq);
  render(0n, { move });
  pendingHumanMove = { move, actor: 'Hint' };
  await playMoveSoundAndAnimation(move, 'player');
  if (myGen !== gen) return;
  await delay(TURN_SOUND_GAP_MS);
  if (myGen !== gen) return;
  busy = false;
  tick({ quietStatus: true });
}

function undo() {
  if (busy) return;
  const target = previousUndoCursor();
  if (target < 0) return;
  cursor = target;
  gameOver = false; bannerEl.hidden = true;
  pendingHumanMove = null;
  announce(`Undo to move ${cursor}. ${sideName(cur().blackToMove)} to move.`);
  tick();
}

function redo() {
  if (busy) return;
  const target = nextRedoCursor();
  if (target < 0) return;
  cursor = target;
  pendingHumanMove = null;
  announce(`Redo to move ${cursor}. ${sideName(cur().blackToMove)} to move.`);
  tick();
}

function previousUndoCursor() {
  for (let i = cursor - 1; i >= 0; i--) {
    if (history[i].blackToMove === humanIsBlack) return i;
  }
  return -1;
}

function nextRedoCursor() {
  for (let i = cursor + 1; i < history.length; i++) {
    if (history[i].blackToMove === humanIsBlack) return i;
  }
  return -1;
}

// ---- move animation and sound ----
function flipDelay(index: number) {
  return FLIP_DELAY_MS + index * FLIP_STAGGER_MS;
}

function moveAnimationMs(move: MoveSummary) {
  if (!animationsEnabled()) return 0;
  if (!move.flips.length) return PLACE_DURATION_MS + 80;
  return flipDelay(move.flips.length - 1) + FLIP_DURATION_MS + 90;
}

async function playMoveSoundAndAnimation(move: MoveSummary, soundFamily: SoundFamily) {
  const sound = moveAudio.play(move, soundFamily);
  await Promise.all([sound.finished, delay(moveAnimationMs(move))]);
}

window.addEventListener('pointerdown', () => moveAudio.unlock(), { capture: true });
window.addEventListener('keydown', () => moveAudio.unlock(), { capture: true });

// ---- rendering ----
function render(legalMoves = 0n, { move = null }: { move?: MoveSummary | null } = {}) {
  const s = cur();
  currentLegalMoves = u64(legalMoves);
  const playableNow = canHumanMoveNow();
  const showHints = settings.showHints && playableNow;
  boardView.render({
    state: s,
    legalMoves: currentLegalMoves,
    move,
    playableNow,
    showHints,
    animationsEnabled: animationsEnabled(),
    flipDelay,
    squareLabel,
  });

  const b = popcount(s.black), w = popcount(s.white);
  blackNum.textContent = String(b);
  whiteNum.textContent = String(w);
  blackScore.classList.toggle('turn', s.blackToMove && !gameOver);
  whiteScore.classList.toggle('turn', !s.blackToMove && !gameOver);

  renderMoves();
  undoBtn.disabled = busy || previousUndoCursor() < 0;
  redoBtn.disabled = busy || nextRedoCursor() < 0;
  hintBtn.disabled = !isHumanTurn() || gameOver || busy;
}

function squareLabel(sq: number, legalMoves: bigint) {
  const s = cur();
  const occupant = discAt(s, sq);
  const parts = [spokenSqName(sq)];

  if (occupant) {
    parts.push(`${colorName(occupant)} disc.`);
  } else if (canHumanMoveNow()) {
    parts.push(bitAt(legalMoves, sq)
      ? `Empty. Legal for ${sideName(s.blackToMove)}.`
      : `Empty. Not legal for ${sideName(s.blackToMove)}.`);
  } else if (gameOver) {
    parts.push('Empty. Game over.');
  } else if (busy) {
    parts.push('Empty. Current move in progress.');
  } else if (!isHumanTurn()) {
    parts.push(`Empty. ${sideName(s.blackToMove)} to move.`);
  } else {
    parts.push('Empty. Not playable right now.');
  }

  if (sq === s.lastMove) parts.push('Last move.');
  return parts.join(' ');
}

function unavailableMessage(sq: number, legalMoves: bigint) {
  const s = cur();
  const occupant = discAt(s, sq);

  if (gameOver) return 'Game over. Start a new game to play again.';
  if (busy) return 'Wait for the current move.';
  if (!isHumanTurn()) return `${sideName(s.blackToMove)} to move. Wait for computer.`;
  if (occupant) return `${spokenSqName(sq)} has a ${colorName(occupant)} disc.`;
  if (!bitAt(legalMoves, sq)) return `${spokenSqName(sq)} is empty and not legal for ${sideName(s.blackToMove)}.`;
  return `${spokenSqName(sq)} is playable.`;
}

function renderMoves() {
  const list = document.createElement('ol');
  // history[0] is the start position; plies begin at index 1.
  for (let i = 1; i < history.length; i++) {
    const li = document.createElement('li');
    const moverIsBlack = history[i - 1].blackToMove;
    const moveSq = history[i].lastMove;
    const span = document.createElement('span');
    span.className = 'mv';
    span.textContent = `${moverIsBlack ? 'B' : 'W'} ${sqName(moveSq)}`;
    li.setAttribute('aria-label', moveSq < 0
      ? `${sideName(moverIsBlack)} passed`
      : `${sideName(moverIsBlack)} played ${spokenSqName(moveSq)}`);
    if (i === cursor) {
      li.style.color = 'var(--accent)';
      li.setAttribute('aria-current', 'step');
    }
    li.append(span);
    list.append(li);
  }
  movesEl.replaceChildren(list);
  movesEl.scrollTop = movesEl.scrollHeight;
}

function endGame() {
  gameOver = true;
  const s = cur();
  const b = popcount(s.black), w = popcount(s.white);
  render(0n);
  const youWon = (humanIsBlack && b > w) || (!humanIsBlack && w > b);
  const who = b === w ? 'Draw' : b > w ? 'Black wins' : 'White wins';
  const tag = b === w ? '' : (youWon ? ' - you win!' : ' - engine wins');
  const pending = takePendingHumanMove();
  const prefix = pending ? `${moveClause(pending.move, pending.actor, true)}. ` : '';
  const message = `${prefix}Game over. ${who}, ${b} to ${w}${tag}`;
  setStatus('Game over', false, false);
  bannerEl.textContent = message;
  bannerEl.hidden = false;
  announce(message, 'assertive');
}

function announceMove(move: MoveSummary, actor: MoveActor) {
  announce(`${moveClause(move, actor, true)}. ${scoreText(move.black, move.white)}`);
}

function announceComputerMove(move: MoveSummary) {
  const pending = takePendingHumanMove();
  if (pending) {
    announce(`${moveClause(pending.move, pending.actor, true)} and ${moveClause(move, 'Computer')}. ${scoreText(move.black, move.white)}`);
  } else {
    announceMove(move, 'Computer');
  }
}

function announcePass(blackToMove: boolean) {
  const pending = takePendingHumanMove();
  const passText = `${sideName(blackToMove)} passes because there is no legal move.`;
  if (pending) announce(`${moveClause(pending.move, pending.actor, true)} and ${passText} ${scoreText(pending.move.black, pending.move.white)}`);
  else announce(passText);
}

function takePendingHumanMove() {
  const pending = pendingHumanMove;
  pendingHumanMove = null;
  return pending;
}

function announce(text: string, politeness: 'polite' | 'assertive' = 'polite') {
  announcerEl.setAttribute('aria-live', politeness);
  announcerEl.textContent = '';
  setTimeout(() => { announcerEl.textContent = text; }, 20);
}

function setStatus(text: string, thinking = false, live = true) {
  statusEl.setAttribute('aria-live', live ? 'polite' : 'off');
  statusEl.textContent = text;
  statusEl.classList.toggle('thinking', thinking);
}

function syncControls() {
  // populate level select once
  if (!levelSel.options.length) {
    for (const [key, v] of Object.entries(LEVELS)) {
      levelSel.add(new Option(`${v.label} (${v.timeMs} ms)`, key));
    }
  }
  levelSel.value = settings.level;
  colorSel.value = settings.color;
}

// ---- wiring ----
window.addEventListener('keyup', (e) => {
  if (viewChords.keyup(e)) {
    if (!isArrowKey(e.key)) return;
  }
  if (!isArrowKey(e.key)) return;
  boardNavigation.keyup(e);
}, { capture: true });
window.addEventListener('keydown', (e) => {
  if (!boardEl.contains(document.activeElement)) return;
  if (viewChords.keydown(e)) {
    return;
  }
}, { capture: true });
window.addEventListener('blur', clearHeldArrowKeys);
document.addEventListener('visibilitychange', () => {
  if (document.hidden) clearHeldArrowKeys();
});
boardEl.addEventListener('focusout', (e) => {
  if (!e.relatedTarget || !boardEl.contains(e.relatedTarget as Node)) {
    clearHeldArrowKeys();
    flushDeferredFocusedCellAccessibility();
  }
});
newBtn.addEventListener('click', newGame);
undoBtn.addEventListener('click', undo);
redoBtn.addEventListener('click', redo);
hintBtn.addEventListener('click', engineHint);
levelSel.addEventListener('change', () => {
  settings.level = levelSel.value as LevelKey; saveSettings(settings);
});
colorSel.addEventListener('change', () => {
  settings.color = colorSel.value as ColorSetting; saveSettings(settings); newGame();
});

applySettingsToBody();
newGame();
