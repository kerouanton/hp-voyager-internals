---
title: HP-15C Reference Index
---

# HP-15C Reference Index

[Home]({{ '/index.html' | relative_url }}) · [Physical memory map]({{ '/hp15c/memory-map.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [RAM register uses]({{ '/hp15c/ram-register-map.html' | relative_url }})

This is a **navigation index**, not a new set of hardware claims. Follow the linked pages for source evidence, confidence labels and interpretation limits. All addresses in these tables are **hexadecimal**. ROM addresses refer to 10-bit instruction words; RAM addresses refer to 56-bit data registers, not byte offsets.

## Understand the hardware first

**[HP-15C Hardware Architecture: NUT and R2D2]({{ '/hp15c/hardware-architecture.html' | relative_url }})** explains the three-chip design, internal CPU registers, the two R2D2 memory devices, LCD and keyboard, and the distinction between HP firmware and user programs.

## ROM address lookup

The 1,024-word ranges below are **navigation bins**, not established firmware module boundaries.

| ROM address | Reference |
|---|---|
| `0000–03FF` | [Startup, key dispatch and initialization]({{ '/hp15c/rom-map.html' | relative_url }}#address-oriented-entry-point-map) |
| `0400–07FF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `0800–0BFF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `0C00–0FFF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `1000–13FF` | [Sleep signature and keyboard helper]({{ '/hp15c/rom-map.html' | relative_url }}#address-oriented-entry-point-map) |
| `1400–17FF` | [Diagnostics, ROM checksums, RAM tests]({{ '/hp15c/rom-map.html' | relative_url }}#diagnostics-and-self-test) |
| `1800–1FFF` | [Unmapped ROM region]({{ '/hp15c/memory-map.html' | relative_url }}#instruction-rom) |
| `2000–23FF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `2400–27FF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `2800–2BFF` | [RAM helpers and clearing loop]({{ '/hp15c/rom-map.html' | relative_url }}#address-oriented-entry-point-map) |
| `2C00–2FFF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `3000–33FF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `3400–37FF` | [Coverage and unresolved areas]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index) |
| `3800–FFFF` | [No ROM declared]({{ '/hp15c/memory-map.html' | relative_url }}#instruction-rom) |

## RAM and special-register lookup

| Data addresses | Resource | Where to look |
|---|---|---|
| `000–007` | Ordinary RAM, R2D2 index 0 | [Individual RAM registers]({{ '/hp15c/ram-register-map.html' | relative_url }}#complete-80-register-inventory) |
| `008–00A` | Special I/O and display bitmap | [Special-register behavior]({{ '/hp15c/memory-map.html' | relative_url }}#special-register-access-behavior) |
| `00B–00F` | Unmapped | [Complete data bus]({{ '/hp15c/memory-map.html' | relative_url }}#complete-data-bus-map) |
| `010–017` | Ordinary RAM, R2D2 index 1 | [Individual RAM registers]({{ '/hp15c/ram-register-map.html' | relative_url }}#complete-80-register-inventory) |
| `018–01A` | Special I/O (no second LCD connected) | [Special-register behavior]({{ '/hp15c/memory-map.html' | relative_url }}#special-register-access-behavior) |
| `01B–0BF` | Unmapped | [Complete data bus]({{ '/hp15c/memory-map.html' | relative_url }}#complete-data-bus-map) |
| `0C0–0DF` | Ordinary RAM, R2D2 index 1 | [Individual RAM registers]({{ '/hp15c/ram-register-map.html' | relative_url }}#complete-80-register-inventory) |
| `0E0–0FF` | Ordinary RAM, R2D2 index 0 | [Individual RAM registers]({{ '/hp15c/ram-register-map.html' | relative_url }}#complete-80-register-inventory) |
| `100–3FF` | Unmapped | [Complete data bus]({{ '/hp15c/memory-map.html' | relative_url }}#complete-data-bus-map) |

## Jump to an investigation

| Topic | Reference |
|---|---|
| Startup and wake | [ROM Map — Reset and startup]({{ '/hp15c/rom-map.html' | relative_url }}#reset-startup-and-wake) |
| ON + minus, memory clearing | [ROM Map — Initialization and clearing]({{ '/hp15c/rom-map.html' | relative_url }}#initialization-and-memory-clearing) |
| ON + multiply, self-test | [ROM Map — Diagnostics]({{ '/hp15c/rom-map.html' | relative_url }}#diagnostics-and-self-test) |
| Keyboard dispatch table | [ROM Map — Keycode-indexed dispatch]({{ '/hp15c/rom-map.html' | relative_url }}#keycode-indexed-dispatch-table) |
| ROM checksum | [ROM Map — Checksum rule and results]({{ '/hp15c/rom-map.html' | relative_url }}#rom-checksum-rule-and-results) |
| LCD register/segment mapping | [Memory Map — LCD]({{ '/hp15c/memory-map.html' | relative_url }}#lcd-segment-map) |
| CPU instructions and opcodes | [NUT Instruction Set]({{ '/hp15c/instruction-set.html' | relative_url }}) |
| CPU and arithmetic fields | [Memory Map — CPU state]({{ '/hp15c/memory-map.html' | relative_url }}#cpu-internal-registers) |
| Y/Z/T and storage R0–R9 | [RAM Register Uses — Findings]({{ '/hp15c/ram-register-map.html' | relative_url }}#evidence-supported-functional-findings) |
| RAM descriptor at `015` | [RAM Register Uses — Descriptor]({{ '/hp15c/ram-register-map.html' | relative_url }}#ram-015-address-descriptor-source) |
| Unresolved allocations | [RAM Register Uses — Unknowns]({{ '/hp15c/ram-register-map.html' | relative_url }}#unknown-allocations) |
| Firmware disassembler | [Firmware Tools]({{ '/firmware/tools.html' | relative_url }}) |
| Sources and attribution | [Sources and Credits]({{ '/sources.html' | relative_url }}) |

## Reading the evidence

A **populated physical address** is not the same as a known firmware function. Nonpareil's emulation behavior and a tested execution trace are not identical to electrical measurements of original hardware. The detail pages distinguish **Confirmed (configuration)**, **Confirmed (emulation)**, **Observed (ROM)** and **Tentative**.

[Return to the homepage]({{ '/index.html' | relative_url }})
