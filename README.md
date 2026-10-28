# HP Voyager Internals

**An open, low-level technical reference for the original HP Voyager calculators, starting with the HP-15C.**

The project aims to collect and cross-reference memory maps, the NUT architecture, hardware interfaces, firmware entry points, and reverse-engineering notes. It is inspired by the address-oriented reference style of classic home-computer memory maps.

**[Visit the website](https://kerouanton.github.io/hp-voyager-internals/)** · **[Documentation](docs/index.md)** · **[HP-15C memory map](docs/hp15c/memory-map.md)**

## Acknowledgement

This work builds on the foundational research of **Eric Smith**, in particular his [Nonpareil calculator simulator](https://github.com/brouhaha/nonpareil), the [HP-15C ROM dump (`15c.rom`)](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom), the NUT CPU implementation, and the HP-15C hardware description in the same repository.

This project is independent and is not affiliated with or endorsed by Eric Smith, HP, or Nonpareil. Please see the [sources and methodology](docs/sources.md) for precise attribution and evidence criteria.

## Contributing

Please share corrections, original documentation, trace results, and references via issues or pull requests. Distinguish observed facts from hypotheses; provide ROM addresses or upstream source locations when possible.

**No HP firmware ROM images are redistributed in this repository.** Upstream source code, firmware, and this project's original documentation have separate copyright and licensing considerations.
