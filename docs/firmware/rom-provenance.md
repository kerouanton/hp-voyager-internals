---
title: HP-15C Microcode ROM Provenance
---

# HP-15C Microcode ROM Provenance

[Home]({{ '/index.html' | relative_url }}) · [Microcode overview]({{ '/firmware/index.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [Community notes]({{ '/hp15c/community-findings.html' | relative_url }})

## What “microcode” means here

This reference uses **microcode** as HP calculator researchers commonly do: the native **10-bit NUT instruction stream** held in the calculator's ROM and executed by the processor. This should not be confused with a separate internal CPU control-store layer in some other computer architectures. The more general word *firmware* may appear in older writing, external source titles, existing directory URLs, or when discussing embedded software generically; **microcode is preferred for the original HP calculator ROM**.

A user-entered HP-15C program is different: it is stored in data RAM in calculator-specific program format and **interpreted by the built-in microcode**. It is not an executable sequence of NUT machine instructions.

## Where the image in this project comes from

Our original-HP-15C address studies use Eric Smith's publicly hosted [Nonpareil HP-15C ROM image, pinned to a source revision](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom). The file header records **25 January 2004** as its extraction date. The corresponding [HP-15C machine description](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl) maps **12,288 ten-bit words** to `0000–17FF` and `2000–37FF`.

This project **links** to the upstream image but **does not redistribute** the ROM file or publish a full derived listing. The original HP microcode and Nonpareil's software source have separate rights and provenance.

## How the Voyager ROM was extracted

**Evidence:** Eric Smith described a **ROMsucker** apparatus and his earlier partial wiring for Voyager dumps in his [15 May 2024 forum reply about the NUT pinout](https://www.hpmuseum.org/forum/thread-21739.html). His historical HHC 2004 paper, *Microcode-Level Simulation of Hewlett-Packard Calculators* (also circulated as *Microcode-Level Calculator Simulation*), gives additional background; the original personal-site PDF is presently **not a dependable download link**, so we do not link to it.

**Reported (first-hand, Eric Smith):** the Voyager ROM data were obtained by **observing a real calculator's bus traffic** during its **ROM self-test**, rather than requiring a standalone commercial ROM programmer or decapsulation. The crucial high-level process is:

1. Open an original calculator sufficiently to attach a passive probe to the relevant chip signals.
2. Trigger the built-in **ROM checksum/self-test** path. The NUT instruction **`CXISA`** reads a ROM word as data, enabling the test microcode to visit the ROM contents.
3. Capture synchronized bus traffic. Smith's 2024 pinout notes explicitly name **phase 1**, **phase 2**, **`SYNC`**, and **`ISA`** for his Voyager ROM-dumping connections; phase 2 was not strictly necessary for the dump.
4. Decode the instruction words by address into an image suitable for microcode-level simulation.

**Important boundary:** the forum documents the signals he connected and his method, but it does **not** provide a complete verified electrical schematic, sample capture, or reproducible USB hardware design for our particular HP-15C specimen. We have **not** reproduced the extraction. The date, extracted image, and data-map compatibility are supported by the pinned Nonpareil repository; the historical physical procedure is a separately attributed first-hand account.

The technique works because **ROM checksum microcode reads stored words that might otherwise never be fetched by ordinary calculator operations**. It is different from recording only the instructions executed while pressing keys, which could leave large unused or rarely reached ROM regions uncaptured.

## Why the source distinction matters

| Source | What it can establish | What it cannot establish alone |
|---|---|---|
| Original HP-15C ROM image | Exact 10-bit values, addresses, loops, calls, constants | Electrical pin assignments or analog bus behavior |
| Pinned Nonpareil I code | Reproducible CPU/RAM/LCD **emulation** behavior | Unmeasured silicon behavior on every chip revision |
| Eric Smith's direct hardware/forum reports | Historical extraction method, observed hardware details, discovered edge cases | Independent reproduction by this project |
| Nonpareil II development reports | New investigative observations (e.g. reads of absent RAM) | Availability of an open-source V2 implementation |
| User-program traces and our disassembler | Focused interpretations of the original microcode | Universal correctness for all states without replayable trace evidence |

**Reproducibility:** we use a pinned GitHub commit for code and ROM references. We do not cite inaccessible copies of the original HHC PDF or an unavailable `brouhaha.com` path as though they were live archives.

[ROM map and validation]({{ '/hp15c/rom-map.html' | relative_url }}) · [Hardware variations]({{ '/hp15c/community-findings.html' | relative_url }}#hardware-revisions-three-chip-and-two-chip-hp-15c) · [Sources and rights]({{ '/sources.html' | relative_url }})
