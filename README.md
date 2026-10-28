# HP Voyager Internals

**An open technical reference for the original HP Voyager calculators.**

The project begins with the **HP-15C**: NUT CPU architecture, memory maps, keyboard and LCD hardware, and evidence-based firmware analysis.

**[Read the documentation](docs/index.md)** · **[HP-15C memory map](docs/hp15c/memory-map.md)**

## Scope

Inspired by the address-oriented memory maps of classic 8-bit computers, this project aims to document both hardware addresses and important firmware entry points. The HP-15C is the first target; other Voyager models may be added later.

Documentation is written in English. Address values are hexadecimal unless otherwise stated.

## Evidence and limitations

Entries distinguish between:

- **Confirmed (configuration):** stated by the model-specific Nonpareil configuration.
- **Confirmed (emulation):** implemented in Nonpareil's NUT or device emulation.
- **Observed (ROM):** supported by original ROM words / disassembly.
- **Tentative:** a functional interpretation not yet confirmed by an execution trace.

An emulator's model is not, by itself, proof of an undocumented property of physical hardware.

## Copyright and contributions

This repository contains original documentation and links to upstream sources, **not redistributed HP calculator ROM images**. The copyrights and licenses of upstream implementations and firmware remain separate. A documentation license is yet to be selected.

Corrections, sources, and precise counterexamples are welcome via GitHub issues or pull requests.
