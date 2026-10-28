// Optional independent file-integrity/CLI checks (Node.js), GPL-3.0-only.
const fs = require('node:fs');
const assert = require('node:assert/strict');
const cp = require('node:child_process');
const crypto = require('node:crypto');
const original = fs.readFileSync('15c.rom');
const hash = crypto.createHash('sha1').update(`blob ${original.length}\0`).update(original).digest('hex');
assert.equal(hash, 'c276b93de235448e65b411412c579c1070cd8955');
const words = new Map([...original.toString().matchAll(/^([0-9a-f]{4}):([0-9a-f]{3})$/gm)].map(m => [parseInt(m[1],16),parseInt(m[2],16)]));
const restored = new Map();
for (const line of fs.readFileSync('15c_disassembled.rom','utf8').split('\n')) {
  const m = line.match(/^([0-9A-F]{4}): ([0-9A-F]{3})(?: ([0-9A-F]{3}))? /);
  if (!m) continue;
  const a = parseInt(m[1],16);
  for (let n=2;n<m.length;n++) if (m[n]) {
    assert(!restored.has(a+n-2)); restored.set(a+n-2,parseInt(m[n],16));
  }
}
assert.deepEqual(restored,words);
const exe = process.argv[2] || './hp15c_disasm';
const temp = '.verify-input.rom';
try {
  for (const invalid of ['', '0000:400\n', '10000:000\n', '0000:000\n0000:001\n', '0000:000 junk\n', '-1:000\n', '0:-1\n', 'x'.repeat(600)]) {
    fs.writeFileSync(temp,invalid);
    assert.equal(cp.spawnSync(exe,[temp]).status,1);
  }
  fs.writeFileSync(temp,'# CRLF\r\n0000:130\r\n0001:3ff\r\n0003:001\r\n');
  const run = cp.spawnSync(exe,[temp],{encoding:'utf8'});
  assert.equal(run.status,0); assert.match(run.stdout,/ldi 1777/);
  assert.match(run.stdout,/Missing continuation word/);
} finally { fs.rmSync(temp,{force:true}); }
console.log(`PASS: upstream Git blob hash; lossless listing of ${words.size} words; 9 CLI fixtures`);