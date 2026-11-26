#!/usr/bin/env python3
"""Gabungkan bagian-bagian analisis menjadi satu ANALISIS_KODE_XAPK.md."""
from pathlib import Path

ROOT = Path('/home/kunbu8u7b/NBT-EDITOR-P')
FINAL = ROOT / 'ANALISIS_KODE_XAPK.md'
PARTS = ROOT / 'analisis_parts'

# Ambil intro + daftar isi dari file yang sudah ada (sampai sebelum "## Bagian 1").
old = FINAL.read_text(encoding='utf-8')
intro = old.split('## Bagian 1 — Arsitektur & alur keseluruhan', 1)[0]
intro = intro.rstrip() + '\n\n---\n\n'

# Ambil Bagian 6 (PairIP) yang sudah ditulis inline di file lama.
if '## Bagian 6 — PairIP' in old:
    b6 = '## Bagian 6 — PairIP' + old.split('## Bagian 6 — PairIP', 1)[1]
    b6 = b6.split('\n---\n', 1)[0].rstrip() + '\n'
else:
    b6 = (PARTS / '06_pairip.md').read_text(encoding='utf-8')

order = ['01_arsitektur.md', '02_ui.md', '03_engine_nbt.md',
         '04_world.md', '05_resources.md']
chunks = [intro]
for name in order:
    p = PARTS / name
    if not p.exists():
        raise SystemExit(f'Bagian belum tersedia: {p}')
    chunks.append(p.read_text(encoding='utf-8').rstrip() + '\n\n---\n\n')
chunks.append(b6 + '\n\n---\n\n')
summary = PARTS / '07_summary.md'
if summary.exists():
    chunks.append(summary.read_text(encoding='utf-8').rstrip() + '\n')
FINAL.write_text(''.join(chunks), encoding='utf-8')
print(f'OK: {FINAL} ({FINAL.stat().st_size} bytes, {len(FINAL.read_text().splitlines())} baris)')
