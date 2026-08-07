// Smoke test for the WASM build, run directly under Node (no worker):
//   make wasm && node othello-wasm/node-test.mjs
import createOthello from './othello.js';

const m = await createOthello();
const hex = (x) => x.toString(16).padStart(16, '0');

let black = m._othello_initial_black();
let white = m._othello_initial_white();
console.log('initial   black', hex(black), 'white', hex(white));

const moves = m._othello_legal_moves(black, white, 1);
console.log('opening legal-move count:', m._othello_count(moves), '(expect 4)');

const sq = m._othello_best_move(black, white, 1, 12, 300);
console.log('best move square (time-bounded 300ms):', sq);

m._othello_apply(black, white, 1, sq);
black = m._othello_result_black();
white = m._othello_result_white();
console.log('after move black', hex(black), 'white', hex(white),
            '-> total discs', m._othello_count(black | white), '(expect 5)');

const passed = m._othello_count(moves) === 4 &&
  m._othello_count(black | white) === 5;
console.log(passed ? 'WASM smoke test PASSED' : 'WASM smoke test FAILED');
if (!passed) process.exitCode = 1;
