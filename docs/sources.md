---
title: Sources and Evidence
---

# Sources and Evidence Policy

## Primary technical source: Nonpareil

- [Nonpareil source repository](https://github.com/brouhaha/nonpareil), by Eric Smith.
- [HP-15C machine description](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.ncd.tmpl): chip types, oscillator, ROM/RAM ranges, LCD wiring, keyboard keycodes.
- [NUT CPU header](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.h): emulator CPU state.
- [NUT CPU implementation](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.c): instruction behavior, memory access and control flow.
- [Voyager LCD driver](https://github.com/brouhaha/nonpareil/blob/main/src/voyager_lcd.c): display memory and driver behavior.
- [NUT disassembler](https://github.com/brouhaha/nonpareil/blob/main/src/dis_nut.c): instruction decoding.

The initial examination used Nonpareil commit `c347bc1ab20170c253512042f7aac0d952f304ea` as the source reference for the standalone disassembler. Links above point to the moving `main` branch and may evolve; individual claims should eventually link to pinned revisions.

## Interpretation labels

| Label | Meaning |
|---|---|
| Confirmed (configuration) | Explicitly present in the machine description |
| Confirmed (emulation) | Behavior implemented in Nonpareil source |
| Observed (ROM) | Instruction words or disassembly support the claim |
| Tentative | Functional interpretation; trace or independent evidence pending |

The categories are **about evidence, not certainty that physical HP silicon operates identically to the emulator**.

## Rights and provenance

Nonpareil source code, machine definitions, firmware dumps, and this project's original explanatory writing may have **different copyrights and licenses**. Consult the file-level notices and upstream licensing documents before reproducing code or redistributing ROM data. This repository initially contains only original documentation and source links.

## Corrections

Please provide the affected address, the original 10-bit ROM word(s), the hypothesized control path, and preferably a link to a supporting source or execution trace.
