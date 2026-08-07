// Shared, localStorage-backed settings used by the game and the Settings page.

export const LEVELS = {
  fast:   { label: 'Fast',   timeMs: 100 },
  medium: { label: 'Medium', timeMs: 600 },
  strong: { label: 'Strong', timeMs: 2000 },
  max:    { label: 'Max',    timeMs: 5000 },
};

export type PlayerColor = 'black' | 'white';
export type ColorSetting = PlayerColor | 'random';
export type LevelKey = keyof typeof LEVELS;

export type Settings = {
  color: ColorSetting;
  level: LevelKey;
  showHints: boolean;
  animate: boolean;
  soundEffects: boolean;
};

export const SETTINGS_KEY = 'othello-settings';

const DEFAULTS: Settings = {
  color: 'black',   // 'black' | 'white' | 'random'  (the human's color)
  level: 'medium',  // key into LEVELS
  showHints: true,
  animate: true,
  soundEffects: true,
};

export function loadSettings(): Settings {
  try {
    return { ...DEFAULTS, ...JSON.parse(localStorage.getItem(SETTINGS_KEY) || '{}') };
  } catch {
    return { ...DEFAULTS };
  }
}

export function saveSettings(settings: Partial<Settings>) {
  localStorage.setItem(SETTINGS_KEY, JSON.stringify({ ...DEFAULTS, ...settings }));
}

export function levelMs(level: string) {
  return (LEVELS[level as LevelKey] || LEVELS.medium).timeMs;
}

/** Resolve 'random' to a concrete color for a new game. */
export function resolveColor(color: ColorSetting): PlayerColor {
  if (color === 'random') return Math.random() < 0.5 ? 'black' : 'white';
  return color;
}
