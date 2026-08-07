import type { MoveSummary } from './types';

type MoveAudioOptions = {
  soundEnabled: () => boolean;
  sampleHitDurationMs: number;
  placeSoundStartMs: number;
  flipDelayMs: number;
  flipStaggerMs: number;
  soundFinishPadMs: number;
};

const FAMILIES = ['player', 'computer'] as const;
export type Family = (typeof FAMILIES)[number];

// Row 3 is the base sample pitch. Higher/lower rows shift by just-intonation
// major-scale intervals, so the board still sounds musical without needing
// 64 separate pitched samples.
const BASE_ROW = 3;
const SCALE = [1, 9 / 8, 5 / 4, 4 / 3, 3 / 2, 5 / 3, 15 / 8, 2];
const ROW_RATE = SCALE.map((r) => r / SCALE[BASE_ROW]);

// Column 0 is smooth/dull; column 7 is bright/driven. We use a resonant
// lowpass filter whose cutoff sweeps across that range.
function columnFrequency(col: number): number {
  const min = 800;
  const max = 12000;
  return min * Math.pow(max / min, col / 7);
}

function columnQ(col: number): number {
  return 0.5 + (col / 7) * 2.5;
}

export class MoveAudio {
  private ctx: AudioContext | null = null;
  private buffers: Partial<Record<Family, AudioBuffer>> = {};
  private loading: Promise<void> | null = null;
  private warningShown = false;

  constructor(private readonly options: MoveAudioOptions) {}

  unlock() {
    if (!this.options.soundEnabled()) return;
    if (this.ctx && this.ctx.state === 'suspended') {
      void this.ctx.resume().catch(() => {});
      return;
    }
    void this.ensureReady().catch(() => {});
  }

  play(move: MoveSummary, family: Family) {
    if (!this.options.soundEnabled()) return noMoveSound();

    void this.ensureReady()
      .then(() => this.schedule(move, family))
      .catch((err) => this.warn(String(err)));

    const durationMs = this.durationMs(move);
    return { durationMs, finished: delay(durationMs) };
  }

  private async ensureReady() {
    if (this.loading) return this.loading;
    this.loading = this.init();
    return this.loading;
  }

  private async init() {
    const Ctx =
      (window as unknown as { AudioContext?: typeof AudioContext }).AudioContext ??
      (window as unknown as { webkitAudioContext?: typeof AudioContext }).webkitAudioContext;
    if (!Ctx) {
      throw new Error('Web Audio API is not supported in this browser.');
    }

    this.ctx = new Ctx();
    if (this.ctx.state === 'suspended') {
      await this.ctx.resume();
    }

    for (const family of FAMILIES) {
      const response = await fetch(`sounds/${family}.wav`);
      if (!response.ok) {
        throw new Error(`Failed to load sounds/${family}.wav (${response.status})`);
      }
      const arrayBuffer = await response.arrayBuffer();
      this.buffers[family] = await this.ctx.decodeAudioData(arrayBuffer);
    }
  }

  private schedule(move: MoveSummary, family: Family) {
    const t0 = this.options.placeSoundStartMs / 1000;
    this.playTone(family, move.sq, 'place', t0);

    move.flips.forEach((sq, i) => {
      const start =
        (this.options.flipDelayMs + i * this.options.flipStaggerMs) / 1000;
      this.playTone(family, sq, 'flip', start);
    });
  }

  private playTone(
    family: Family,
    sq: number,
    kind: 'place' | 'flip',
    whenOffset: number
  ) {
    if (!this.ctx) return;
    const buffer = this.buffers[family];
    if (!buffer) return;

    const ctx = this.ctx;
    const row = Math.floor(sq / 8);
    const col = sq % 8;
    const now = ctx.currentTime + whenOffset;

    const source = ctx.createBufferSource();
    source.buffer = buffer;
    source.playbackRate.value = ROW_RATE[row] ?? 1;

    const filter = ctx.createBiquadFilter();
    filter.type = 'lowpass';
    filter.frequency.value = columnFrequency(col);
    filter.Q.value = columnQ(col);

    const pan = ctx.createStereoPanner();
    pan.pan.value = (col - 3.5) / 3.5;

    const gain = ctx.createGain();
    const volume = kind === 'place' ? 0.95 : 0.82;
    const hitDur = this.options.sampleHitDurationMs / 1000;
    gain.gain.setValueAtTime(volume, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + hitDur);

    // Keep overlapping flips from clipping without squashing the tone.
    const compressor = ctx.createDynamicsCompressor();
    compressor.threshold.value = -10;
    compressor.knee.value = 3;
    compressor.ratio.value = 4;
    compressor.attack.value = 0.003;
    compressor.release.value = 0.1;

    source.connect(filter);
    filter.connect(pan);
    pan.connect(gain);
    gain.connect(compressor);
    compressor.connect(ctx.destination);

    source.start(now);
    source.stop(now + hitDur + 0.05);
  }

  private durationMs(move: MoveSummary) {
    let end = this.options.placeSoundStartMs + this.options.sampleHitDurationMs;
    if (move.flips.length) {
      const lastFlipStart =
        this.options.flipDelayMs + (move.flips.length - 1) * this.options.flipStaggerMs;
      end = Math.max(end, lastFlipStart + this.options.sampleHitDurationMs);
    }
    return end + this.options.soundFinishPadMs;
  }

  private warn(message: string) {
    if (this.warningShown) return;
    this.warningShown = true;
    console.warn(message);
  }
}

function noMoveSound() {
  return { durationMs: 0, finished: Promise.resolve(false) };
}

function delay(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}
