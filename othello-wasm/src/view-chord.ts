export type ArrowKey = 'ArrowUp' | 'ArrowDown' | 'ArrowLeft' | 'ArrowRight';

export const ARROW_KEYS = new Set<ArrowKey>(['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight']);

type Input = {
  time: number;
  origin: number;
};

type ViewChordOptions = {
  getOrigin: () => number;
  getHeldArrowKeys: () => Iterable<string>;
  onLook: (origin: number, keys: Set<string>) => void;
  onViewStarted: () => void;
};

const VIEW_KEY = 'v';
// Keep inputs alive for the same duration as the settle timer so a tapped V
// followed by an arrow within the chord window still forms a valid look.
const VIEW_CHORD_WINDOW_MS = 500;
const VIEW_CHORD_SETTLE_MS = 500;

export class ViewChordController {
  private viewKeyHeld = false;
  private inputs = new Map<string, Input>();
  private timer: ReturnType<typeof setTimeout> | null = null;

  constructor(private readonly options: ViewChordOptions) {}

  keydown(e: KeyboardEvent): boolean {
    if (this.isViewKey(e)) {
      this.start(e);
      return true;
    }

    if (!isArrowKey(e.key)) return false;

    this.record(e.key);
    if (!this.viewKeyHeld && !this.inputs.has(VIEW_KEY)) return false;

    e.preventDefault();
    e.stopPropagation();
    this.options.onViewStarted();
    this.scheduleIfReady();
    return true;
  }

  keyup(e: KeyboardEvent): boolean {
    if (this.isViewKey(e)) {
      this.viewKeyHeld = false;
      if (this.arrowKeys().length) {
        // V released after the look direction was chosen: announce now.
        this.flush();
        this.clear();
      }
      // If V was released before any arrow, keep the V input alive so a
      // subsequent arrow can still complete a tap-V-then-arrow look.
      return true;
    }
    return isArrowKey(e.key);
  }

  clear() {
    this.viewKeyHeld = false;
    this.inputs.clear();
    if (this.timer) clearTimeout(this.timer);
    this.timer = null;
  }

  private start(e: KeyboardEvent) {
    e.preventDefault();
    e.stopPropagation();
    this.record(VIEW_KEY);
    for (const key of this.options.getHeldArrowKeys()) {
      if (isArrowKey(key)) this.record(key);
    }
    if (!this.viewKeyHeld) {
      this.viewKeyHeld = true;
      this.options.onViewStarted();
    }
    this.scheduleIfReady();
  }

  private record(key: string) {
    const now = performance.now();
    this.prune(now);
    this.inputs.set(key, { time: now, origin: this.options.getOrigin() });
    // Age the whole chord from the most recent input. This keeps a tapped V
    // alive long enough for the arrow to arrive and prevents one direction
    // of a slow diagonal from being pruned before the chord flushes.
    for (const input of this.inputs.values()) {
      input.time = now;
    }
  }

  private prune(now = performance.now()) {
    // While a chord is actively being built (V held or settle timer pending)
    // we never prune inputs; the chord flushes them all at once.
    if (this.viewKeyHeld || this.timer) return;
    for (const [key, input] of this.inputs) {
      if (now - input.time > VIEW_CHORD_WINDOW_MS) this.inputs.delete(key);
    }
  }

  private scheduleIfReady(): boolean {
    this.prune();
    if (!this.inputs.has(VIEW_KEY) || !this.arrowKeys().length) return false;
    if (this.timer) clearTimeout(this.timer);
    this.timer = setTimeout(() => this.flush(), VIEW_CHORD_SETTLE_MS);
    return true;
  }

  private flush() {
    if (this.timer) clearTimeout(this.timer);
    this.timer = null;
    // Don't prune here: the chord that just matured is about to be announced
    // and cleared. Pruning first risks deleting inputs that reached the exact
    // end of the settle window due to timer imprecision.
    if (!this.inputs.has(VIEW_KEY) || !this.arrowKeys().length) return;
    this.options.onLook(this.options.getOrigin(), this.directionKeys());
    this.inputs.clear();
  }

  private arrowKeys() {
    return [...this.inputs.keys()].filter(isArrowKey);
  }

  private directionKeys() {
    const arrows = this.arrowKeys();
    return new Set([
      this.latest(arrows.filter((key) => key === 'ArrowUp' || key === 'ArrowDown')),
      this.latest(arrows.filter((key) => key === 'ArrowLeft' || key === 'ArrowRight')),
    ].filter(Boolean));
  }

  private latest(keys: string[]) {
    let latestKey = '';
    let latestTime = -Infinity;
    for (const key of keys) {
      const time = this.inputs.get(key)?.time ?? -Infinity;
      if (time > latestTime) {
        latestKey = key;
        latestTime = time;
      }
    }
    return latestKey;
  }

  private origin() {
    // Use the current focus at flush time rather than the focus captured when
    // the chord started. This avoids describing a ray from the previous square
    // if a pending navigation move fired while the user was pressing V.
    return this.options.getOrigin();
  }

  private isViewKey(e: KeyboardEvent) {
    return !e.altKey && !e.ctrlKey && !e.metaKey && e.key.toLowerCase() === VIEW_KEY;
  }
}

export function isArrowKey(key: string): key is ArrowKey {
  return ARROW_KEYS.has(key as ArrowKey);
}
