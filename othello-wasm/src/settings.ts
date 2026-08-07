import { SETTINGS_KEY, LEVELS, loadSettings, saveSettings, type ColorSetting, type LevelKey } from './config';

const levelSel = getElement<HTMLSelectElement>('set-level');
const colorSel = getElement<HTMLSelectElement>('set-color');
const hints = getElement<HTMLInputElement>('set-hints');
const animate = getElement<HTMLInputElement>('set-animate');
const sound = getElement<HTMLInputElement>('set-sound');
const saved = getElement<HTMLElement>('saved');
const reset = getElement<HTMLButtonElement>('reset');

for (const [key, value] of Object.entries(LEVELS)) {
  levelSel.add(new Option(`${value.label} (${value.timeMs} ms)`, key));
}

let settings = loadSettings();
fill();

function fill() {
  levelSel.value = settings.level;
  colorSel.value = settings.color;
  hints.checked = settings.showHints;
  animate.checked = settings.animate;
  sound.checked = settings.soundEffects;
}

function flash() {
  saved.textContent = 'Saved ✓';
  setTimeout(() => { saved.textContent = ''; }, 1200);
}

function update() {
  settings = {
    level: levelSel.value as LevelKey,
    color: colorSel.value as ColorSetting,
    showHints: hints.checked,
    animate: animate.checked,
    soundEffects: sound.checked,
  };
  saveSettings(settings);
  flash();
}

[levelSel, colorSel, hints, animate, sound].forEach((el) => el.addEventListener('change', update));
reset.addEventListener('click', () => {
  localStorage.removeItem(SETTINGS_KEY);
  settings = loadSettings();
  fill();
  flash();
});

function getElement<T extends HTMLElement>(id: string): T {
  const element = document.getElementById(id);
  if (!element) throw new Error(`Missing #${id}`);
  return element as T;
}
