import { defineConfig } from 'vite';
import { cpSync, existsSync, mkdirSync, rmSync } from 'node:fs';
import { resolve } from 'node:path';

const runtimeAssets = ['engine-worker.js', 'othello.js', 'othello.wasm', 'sounds'];

export default defineConfig({
  base: '/othello/',
  build: {
    rollupOptions: {
      input: {
        index: 'index.html',
        settings: 'settings.html',
        about: 'about.html',
        rules: 'rules.html',
      },
    },
  },
  server: {
    host: '127.0.0.1',
  },
  plugins: [{
    name: 'copy-runtime-assets',
    closeBundle() {
      const outDir = resolve('dist');
      mkdirSync(outDir, { recursive: true });
      for (const asset of runtimeAssets) {
        const from = resolve(asset);
        const to = resolve(outDir, asset);
        if (!existsSync(from)) {
          throw new Error(`Missing runtime asset: ${asset}`);
        }
        rmSync(to, { recursive: true, force: true });
        cpSync(from, to, { recursive: true });
      }
    },
  }],
});
