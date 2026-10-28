---
title: HP Voyager Internals
---

# HP Voyager Internals

**An open, low-level technical reference for the original HP Voyager calculators.**

This project starts with the **HP-15C**, a scientific programmable calculator from the Voyager family. Its purpose is to **collect, organize, cross-reference, and gradually document low-level technical knowledge** that is otherwise scattered across source repositories, manuals, ROM disassemblies, and historical resources.

Inspired by classic computer memory maps such as the [Commodore 64 Memory Map](https://sta.c64.org/cbm64mem.html), this site is intended to be a practical reference: *which addresses exist, what the hardware does, where important firmware routines live, and how the pieces work together.*

## A project built on Eric Smith's work

This project owes a substantial debt to **Eric Smith**, creator of [Nonpareil](https://github.com/brouhaha/nonpareil), an emulator of classic HP calculators.

Nonpareil provides the foundation for much of the initial HP-15C research:

- the **HP-15C ROM dump**, distributed in the Nonpareil repository as [`ncd/15c/15c.rom`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom);
- the **NUT CPU emulator**, which documents instruction behavior, registers, and memory accesses in executable form;
- the HP-15C **machine description**, including ROM/RAM ranges, keyboard codes, and LCD wiring;
- a **NUT disassembler** and related source files.

The goal here is **not to replace Nonpareil** or claim its original discoveries. It is to make technical information easier to find, understand, verify, and extend, always with attribution to the original authors.

This is an **independent community documentation project**, not affiliated with or endorsed by Eric Smith, HP, or the Nonpareil project. See [Sources, credits, and methodology](sources.md).

## Explore the HP-15C

| Reference | What you will find |
|---|---|
| [HP-15C memory map](hp15c/memory-map.md) | Instruction ROM, data RAM, device registers, keyboard codes, and display mapping |
| [NUT CPU architecture](hp15c/nut-cpu.md) | Registers, instruction format, addressing, and timing |
| [Firmware and reverse engineering](firmware/index.md) | ROM analysis, startup, diagnostics, and research notes |
| [Firmware tools](firmware/tools.md) | C disassembler, tests, and reproduction instructions |
| [Sources and methodology](sources.md) | Primary resources, credit, provenance, and evidence labels |

## What we hope to document

- **Hardware:** NUT CPU, ROM and RAM maps, registers, display, keyboard, and peripheral interfaces.
- **Firmware organization:** startup, keyboard handling, shared services, mathematical routines, and the user-program interpreter.
- **Diagnostics:** behavior of startup key combinations, including the memory-clear and self-test paths.
- **Algorithms and design choices:** how HP implemented advanced scientific functions within a very small ROM.

This is a **work in progress**, not a finished or authoritative hardware manual. Firmware labels and inferred purposes are explicitly marked *Tentative* until confirmed by machine-code evidence, control-flow analysis, or execution traces.

## About the ROM and licensing

The original HP firmware is **not redistributed by this site**. Links to upstream files are supplied for research and provenance; their copyright status is distinct from the licenses of the Nonpareil source code and of original documentation written here.

Corrections and contributions are welcome through the [GitHub repository](https://github.com/kerouanton/hp-voyager-internals).
