# HP Voyager Internals

**An open, low-level technical reference for the original HP Voyager calculators, starting with the HP-15C.**

The project aims to collect and cross-reference memory maps, the NUT architecture, hardware interfaces, microcode entry points, and reverse-engineering notes. It is inspired by the address-oriented reference style of classic home-computer memory maps.

**[RAM 015 research](docs/hp15c/ram015-allocation.md)** · **[NUT status bits](docs/hp15c/nut-status-bits.md)** · **[Community discoveries](docs/hp15c/community-findings.md)** · **[ROM provenance](docs/microcode/rom-provenance.md)** · **[Hardware architecture](docs/hp15c/hardware-architecture.md)** · **[Address index](docs/hp15c/reference-index.md)** · **[Visit the website](https://kerouanton.github.io/hp-voyager-internals/)** · **[Documentation](docs/index.md)** · **[Complete memory map](docs/hp15c/memory-map.md)** · **[ROM map](docs/hp15c/rom-map.md)** · **[Instruction set](docs/hp15c/instruction-set.md)** · **[RAM register uses](docs/hp15c/ram-register-map.md)**

## Project authorship and AI assistance

This documentation is developed collaboratively by the **project maintainer** and **OpenAI ChatGPT in manual ("agentless") mode**. The maintainer directs the project, selects research questions, reviews material, and approves publication; ChatGPT helps with source inspection, technical interpretation, writing, and organization. "Agentless" describes the interactive, human-directed workflow—not an independent verification of all hardware claims.

The **standalone C11 disassembler, test suite, and original listings** were created separately with **ChatGPT Work** using Nonpareil source code. The maintainer subsequently compiled and tested the delivered tool under **Cygwin**. See the [development and validation history](docs/microcode/disassembler-history.md) and [credits](docs/sources.md).

## Acknowledgement

This work builds on the foundational research of **Eric Smith**, in particular his [Nonpareil calculator simulator](https://github.com/brouhaha/nonpareil), the [HP-15C ROM dump (`15c.rom`)](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom), the NUT CPU implementation, and the HP-15C hardware description in the same repository.

This project is independent and is not affiliated with or endorsed by Eric Smith, HP, or Nonpareil. Please see the [sources and methodology](docs/sources.md) for precise attribution and evidence criteria.

## Contributing

Please share corrections, original documentation, trace results, and references via issues or pull requests. Distinguish observed facts from hypotheses; provide ROM addresses or upstream source locations when possible.

**No HP microcode ROM images are redistributed in this repository.** Upstream source code, microcode, and this project's original documentation have separate copyright and licensing considerations.
