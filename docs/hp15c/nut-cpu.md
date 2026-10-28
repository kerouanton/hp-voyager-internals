---
title: NUT CPU Architecture
---

# NUT CPU Architecture — HP-15C

[Address and topic index]({{ '/hp15c/reference-index.html' | relative_url }}) · [Home]({{ '/index.html' | relative_url }})

This page records the CPU state represented by Nonpareil's NUT implementation. It is a guide to interpreting the HP-15C ROM, not a claim that every implementation detail has been verified on physical silicon.

[Home]({{ '/index.html' | relative_url }}) · [Memory map]({{ '/hp15c/memory-map.html' | relative_url }})

## Registers

| Register / field | Representation in Nonpareil | Role |
|---|---|---|
| `A`, `B`, `C`, `M`, `N` | 14 nibbles each | Arithmetic / temporary registers |
| `G` | 2 nibbles | Auxiliary register |
| `P`, `Q` | 4 bits each | Digit pointers |
| `q_sel` | Boolean | Selects active digit pointer |
| `S` | 14 Boolean flags | Status bits |
| `FO` | 8 bits | Flag output register |
| `PC` | 16 bits | Program counter |
| Return stack | 4 × 16-bit addresses | Subroutine returns |
| `carry`, `prev_carry` | Boolean | Current / preceding instruction carry |
| `decimal` | Boolean | Decimal or hexadecimal arithmetic selection |
| `ram_addr` | 10 bits | Selected data-memory address |
| `pf_addr` | 8 bits | Selected peripheral address |

Source: [`src/proc_nut.h`](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.h), `nut_reg_t`, and [`src/proc_nut.c`](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.c).

For a complete first-word overview of instruction families, field variants and opcode formulas, see the [NUT Instruction Set Reference]({{ '/hp15c/instruction-set.html' | relative_url }}).

## Instruction encoding

NUT instruction words are **10 bits**. The low two opcode bits select four broad decoding categories:

| Bits 1:0 | Category |
|---|---|
| `00` | Miscellaneous / I/O / control instructions |
| `01` | Two-word absolute branches or calls |
| `10` | Register arithmetic and comparisons |
| `11` | One-word relative branches |

The miscellaneous `LDI` instruction (`0x130`) also consumes a second 10-bit word.

A NUT instruction list needs to distinguish the selected-peripheral protocol state from normal decoding. A linear sweep alone cannot establish true instruction boundaries when code and data share ROM.

## Address spaces

- Instruction ROM has word addresses and 10-bit words.
- Data RAM contains 14-nibble registers, not bytes.
- The HP-15C model declares ROM ranges `0000–17FF` and `2000–37FF`.
- The data-memory map is described in the [HP-15C memory map]({{ '/hp15c/memory-map.html' | relative_url }}).

## Timing

The HP-15C model declares an **LC oscillator frequency of 215,000 Hz** in `15c.ncd.tmpl`. This must not be mistaken for an independently verified rate of 215,000 *instructions* per second; Nonpareil derives timing from architecture parameters.

## Areas for future verification

- Exact cycle counts per instruction and branch behavior.
- Reset state of each internal register and flag.
- Effects of peripheral selection and display control.
- Real hardware differences from the emulator.

[Back to index]({{ '/index.html' | relative_url }})
