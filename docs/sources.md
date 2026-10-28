---
title: Sources and Evidence
---

# Sources, Credits, and Evidence Policy

## Acknowledgement: Eric Smith and Nonpareil

The first HP-15C pages rely heavily on the pioneering work of **Eric Smith**, creator of **Nonpareil**. His emulator and machine definitions provide the essential foundation for understanding the NUT CPU, memory organization, keyboard, and display.

The HP-15C ROM dump referred to here is the file [`ncd/15c/15c.rom`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom) from the Nonpareil repository (the file header records an extraction date of 2004-01-25). This project **links to that upstream dump but does not rehost it**.

Nonpareil's software authorship and the authorship or rights in HP's original firmware are separate matters. The availability of a ROM dump in a public source repository must not be interpreted as an unrestricted redistribution license.

This is an independent documentation effort. It is not affiliated with or endorsed by Eric Smith, Nonpareil, or HP.

## Authorship and development workflow

| Contributor / workflow | Contribution |
|---|---|
| **Eric Smith / Nonpareil** | Original NUT disassembler and CPU emulator, HP-15C model definition, and source repository containing the `15c.rom` dump. |
| **ChatGPT Work** | Standalone C11 adaptation, regression tests, verification helper, original tool documentation, and generated ROM listings. |
| **Project maintainer** | Project conception and direction, selection of investigations, review and publication; successful compilation and testing of the program on Cygwin. |
| **OpenAI ChatGPT — manual ("agentless") mode** | Interactive research assistance, technical synthesis, drafting, documentation structure, and preparation of GitHub changes. |

**Agentless** describes the human-directed conversational documentation workflow, not the absence of software tools or repository actions. It is distinct from the **ChatGPT Work** workflow used to create the standalone C disassembler.

AI assistance is part of the provenance, not evidence of correctness: claims must still be checked against original sources and traces.

See the [disassembler development and validation history]({{ '/firmware/disassembler-history.html' | relative_url }}).

## Primary technical source: Nonpareil

- [Nonpareil source repository](https://github.com/brouhaha/nonpareil), by Eric Smith.
- [HP-15C machine description](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.ncd.tmpl): chip types, oscillator, ROM/RAM ranges, LCD wiring, keyboard keycodes.
- [NUT CPU header](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.h): emulator CPU state.
- [NUT CPU implementation](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.c): instruction behavior, memory access and control flow.
- [Voyager LCD driver](https://github.com/brouhaha/nonpareil/blob/main/src/voyager_lcd.c): display memory and driver behavior.
- [NUT disassembler](https://github.com/brouhaha/nonpareil/blob/main/src/dis_nut.c): instruction decoding.

The initial examination used Nonpareil commit `c347bc1ab20170c253512042f7aac0d952f304ea` as the source reference for the standalone disassembler. Links above point to the moving `main` branch and may evolve; individual claims should eventually link to pinned revisions.

## Community reports, lost pages, and original ROM provenance

The [community research notes]({{ '/hp15c/community-findings.html' | relative_url }}) index selected first-hand posts from the [Museum of HP Calculators forum](https://www.hpmuseum.org/forum/) with author, date, scope and uncertainty. Particularly relevant threads cover the [R2D2/NUT hardware variants](https://www.hpmuseum.org/forum/thread-21739.html), [missing-RAM behavior during R↑/R↓](https://www.hpmuseum.org/forum/thread-21336.html), [NUT RAM-chip instruction `2B0`](https://www.hpmuseum.org/forum/thread-24718.html), [Nonpareil development history](https://www.hpmuseum.org/forum/thread-18940.html), [Nonpareil II's closed-source plans and testing](https://www.hpmuseum.org/forum/thread-21213.html), and [the 2026 personal-Web-server outage](https://www.hpmuseum.org/forum/thread-23279.html).

The [microcode ROM provenance page]({{ '/firmware/rom-provenance.html' | relative_url }}) documents Eric Smith's account of physical Voyager ROM extraction during self-test and distinguishes the historical account from the source-controlled ROM image and its header. The historical 2004 paper was hosted on now-inaccessible `brouhaha.com` infrastructure. **We do not claim an archive or an active replacement download for it.** Do not cite a dead PDF or a missing Wayback capture as a verified working reference.

For a source claim taken from the forum we add **Reported (first-hand)** rather than upgrading it automatically to a silicon measurement, independent trace, or ROM observation by this project. The rights and licenses of historic papers, photographs, ROMs and microcode patches are not assumed from their public availability.

## Interpretation labels

| Label | Meaning |
|---|---|
| Confirmed (configuration) | Explicitly present in the machine description |
| Confirmed (emulation) | Behavior implemented in Nonpareil source |
| Observed (ROM) | Instruction words or disassembly support the claim |
| Tentative | Functional interpretation; trace or independent evidence pending |

The categories are **about evidence, not certainty that physical HP silicon operates identically to the emulator**.

## Rights and provenance

Nonpareil source code, machine definitions, firmware dumps, and this project's original explanatory writing may have **different copyrights and licenses**. Consult the file-level notices and upstream licensing documents before reproducing code or redistributing ROM data. This repository contains original documentation and a GPL-3.0-only derivative of the Nonpareil NUT decoder, but does not redistribute the HP firmware ROM or derived ROM listings.

## Corrections

Please provide the affected address, the original 10-bit ROM word(s), the hypothesized control path, and preferably a link to a supporting source or execution trace.
