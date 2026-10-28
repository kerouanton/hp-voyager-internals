---
title: HP Voyager Internals
---

# HP Voyager Internals

A public, address-oriented reference for the original HP Voyager calculator family.

**Current focus: HP-15C.** The goal is to understand the *big picture* of the firmware and hardware without assuming that every disassembled word is executable code.

## HP-15C reference

| Reference | Contents |
|---|---|
| [Memory map](hp15c/memory-map.md) | Instruction ROM, data RAM, display registers, and address-space gaps |
| [NUT CPU](hp15c/nut-cpu.md) | CPU registers, word sizes, and addressing |
| [Sources and methodology](sources.md) | Upstream source files and evidence labels |

## Planned investigations

- Startup paths: normal power-on, ON + minus (memory clear), and ON + multiply (self-test).
- ROM call graph and heavily reused routines.
- Keyboard and LCD registers and their interactions with the firmware.
- Selected mathematical algorithms and ROM-access loops.

**Status:** Initial reference. Functional annotations are provisional until verified against control flow or execution traces.
