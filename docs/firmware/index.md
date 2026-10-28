---
title: HP-15C Microcode
---

# HP-15C Microcode

[Home]({{ '/index.html' | relative_url }}) · [Memory map]({{ '/hp15c/memory-map.html' | relative_url }}) · [Tools]({{ '/firmware/tools.html' | relative_url }})

The original HP-15C microcode occupies **12,288 10-bit instruction words** mapped to `0000–17FF` and `2000–37FF` in the Nonpareil machine definition.

This section aims to describe the **big picture**: startup and diagnostics, shared services, keyboard handling, user-program interpretation, numerical algorithms, and memory management.

[Address and topic index]({{ '/hp15c/reference-index.html' | relative_url }}) — quick entry points for ROM ranges, memory regions, and diagnostics.

**[Hardware architecture overview]({{ '/hp15c/hardware-architecture.html' | relative_url }})** explains how the NUT processor retrieves microcode instructions from the two R2D2 memory circuits and accesses data memory and the LCD.

## Original ROM provenance and community discoveries

[How the HP-15C ROM was extracted]({{ '/firmware/rom-provenance.html' | relative_url }}) recounts Eric Smith's ROMsucker method and identifies the exact image used in this project. The [community research index]({{ '/hp15c/community-findings.html' | relative_url }}) collects first-hand forum findings without pretending that missing historical PDFs are accessible.

## Address-oriented microcode reference

The [HP-15C ROM Map]({{ '/hp15c/rom-map.html' | relative_url }}) documents all populated ROM ranges, verified dispatch data, startup and initialization sequences, and diagnostic paths. It distinguishes confirmed instruction effects from tentative functional labels. The companion [Complete Memory Map]({{ '/hp15c/memory-map.html' | relative_url }}) covers the full data-register address space, special I/O, LCD wiring, and keyboard interface.

These reference pages were first researched and drafted using **ChatGPT Work** and published with interactive editorial assistance from ChatGPT in manual (*agentless*) mode, building on **Eric Smith's Nonpareil**.

## RAM register allocation and access traces

The [HP-15C RAM register use map]({{ '/hp15c/ram-register-map.html' | relative_url }}) lists all 80 ordinary RAM addresses, experimentally observed read/write instruction sites, the real-mode stack mapping, and initial R0–R9 storage addressing. **ChatGPT Work** produced the research and first draft using an instrumented Nonpareil-derived CPU harness. The raw trace logs and harness were not supplied for independent replay; this limitation is documented on that page.

## Start with the disassembler

Our [standalone C11 NUT disassembler](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/README.md) reproduces the NUT instruction decoder adapted from **Eric Smith's Nonpareil** and generates:

- a **linear** listing with raw instruction words;
- an **every-address** listing that helps investigate alternate entry points and computed jumps.

The tools and tests are published under [`tools/hp15c-disassembler/`](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/README.md). The **original HP ROM and its derived listings are not hosted here**. See [Tools and reproduction]({{ '/firmware/tools.html' | relative_url }}) and the [development and validation history]({{ '/firmware/disassembler-history.html' | relative_url }}).

## Investigation map

| Area | Status | What remains to establish |
|---|---|---|
| Boot and startup key combinations | Preliminary | Confirm reachable paths and the input register state |
| ON + minus memory clearing | Preliminary | Record all effective RAM writes and subsequent initialization |
| ON + multiply self-test | Preliminary | Confirm ROM checksum loop, tested devices and timing |
| Shared low-level routines | Not mapped | Collect actual callers and register effects |
| Scientific functions and program interpreter | Not mapped | Identify entry points and reusable algorithms |

A decoded instruction is not necessarily executed microcode code: ROMs may contain tables and embedded constants. Functional labels will be added only with trace or control-flow evidence.

## References

- [HP-15C ROM and machine description in Nonpareil](https://github.com/brouhaha/nonpareil/tree/main/ncd/15c)
- [Nonpareil NUT CPU execution code](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.c)
- [Nonpareil NUT disassembler](https://github.com/brouhaha/nonpareil/blob/main/src/dis_nut.c)
- [Sources and evidence policy]({{ '/sources.html' | relative_url }})
