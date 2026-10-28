---
title: Sources and Evidence
---

# Sources, Credits, and Evidence Policy

## Acknowledgement: Eric Smith and Nonpareil

The first HP-15C pages rely heavily on the pioneering work of **Eric Smith**, creator of **Nonpareil**. His emulator and machine definitions provide the essential foundation for understanding the NUT CPU, memory organization, keyboard, and display.

The HP-15C ROM dump referred to here is the file [`ncd/15c/15c.rom`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom) from the Nonpareil repository (the file header records an extraction date of 2004-01-25). This project **links to that upstream dump but does not rehost it**.

Nonpareil's software authorship and the authorship or rights in HP's original firmware are separate matters. The availability of a ROM dump in a public source repository must not be interpreted as an unrestricted redistribution license.

This is an independent documentation effort. It is not affiliated with or endorsed by Eric Smith, Nonpareil, or HP.

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
