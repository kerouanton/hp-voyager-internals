---
title: HP-15C Memory Map
---

# HP-15C Memory Map

An address-oriented reference for the **original HP-15C**, based principally on the Nonpareil model description and NUT emulation.

**Notation:** all addresses and ROM words below are hexadecimal; ROM addresses refer to **10-bit instruction words**, not byte offsets. Data addresses identify **14-nibble registers** (56 bits per register), not bytes.

[Home](../index.md) · [NUT CPU](nut-cpu.md) · [Sources](../sources.md)

## Instruction memory (ROM)

| NUT address range | Size | Device | Status |
|---|---:|---|---|
| `0000–17FF` | 6,144 × 10-bit words | Voyager R2D2 chip 0, part `1LE2-0321` | Confirmed (configuration) |
| `1800–1FFF` | 2,048 addresses | Not populated in the HP-15C model | Confirmed (configuration) |
| `2000–37FF` | 6,144 × 10-bit words | Voyager R2D2 chip 1, part `1LH1-0302` | Confirmed (configuration) |
| `3800–FFFF` | — | No ROM declared by the HP-15C model | Confirmed (configuration) |

**Total declared ROM:** 12,288 10-bit words (122,880 bits). The 16-bit program counter and the page/bank machinery belong to the NUT CPU model; the existence of an address does not imply that a ROM device responds there.

Source: [`ncd/15c/15c.ncd.tmpl`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.ncd.tmpl).

## Data memory (RAM)

| NUT data address range | Registers | Chip | Status |
|---|---:|---|---|
| `00–07` | 8 | R2D2 chip 0 | Confirmed (configuration) |
| `08–0A` | 3 | Display-related I/O addresses installed by the R2D2 driver | Confirmed (emulation) |
| `10–17` | 8 | R2D2 chip 1 | Confirmed (configuration) |
| `18–1A` | 3 | Additional I/O-register addresses installed by the R2D2 driver | Confirmed (emulation) |
| `C0–DF` | 32 | R2D2 chip 1 | Confirmed (configuration) |
| `E0–FF` | 32 | R2D2 chip 0 | Confirmed (configuration) |

The configuration declares **80 ordinary RAM registers**. The R2D2 driver's installation path additionally creates **three special addresses per chip**; these **six I/O registers must not be counted as freely available user RAM**.

For each device index `i`, the driver computes `io_base = i * 0x10 + 0x08`. Its first such address has a read-as-zero / ignore-writes handler; the following two use an LCD bitmap read handler. Chip 1 contributes memory but **does not install a second display controller**.

Source: [`src/voyager_lcd.c`](https://github.com/brouhaha/nonpareil/blob/main/src/voyager_lcd.c), particularly `voyager_r2d2_install()`.

### Interpreting RAM accesses

The instruction displayed as `sel ram` selects a 10-bit data address from the low three nibbles of register C:

```text
RAM_ADDRESS = ((C[2] << 8) | (C[1] << 4) | C[0]) & 0x3FF
```

This is implemented by `op_c_to_dadd()` in [`src/proc_nut.c`](https://github.com/brouhaha/nonpareil/blob/main/src/proc_nut.c).

Other read/write operations may use the selected address or replace its low nibble with the instruction's register operand. Consequently, a static `DATA=C` or `C=DATA` does not identify a physical address without tracking the prior value of C.

## LCD

The Voyager display model comprises **11 positions** and **10 segment types per position** (digit segments plus decimal point, comma tail and annunciator).

The physical segment-to-register-bit mapping is given explicitly in the `<lcd>` element of [`15c.ncd.tmpl`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.ncd.tmpl). The emulator reads each bit through `nut_reg->ram[reg][bit / 4]` and a `1 << (bit & 3)` mask.

**To document next:** a complete table of each display segment and its register/bit source. Some positions contain unused segments marked with `reg="-1"` and `bit="-1"`.

## Keyboard codes

Hardware keycodes declared by the model description:

| Key | Hardware keycode |
|---|---|
| ON | `18` |
| Multiply (×) | `17` |
| Divide (÷) | `12` |
| Minus (−) | `14` |
| Plus (+) | `15` |
| ENTER | `84` |
| SIN | `70` |
| COS | `C0` |

These are **keyboard hardware codes, not ROM addresses**. The source is the `<keyboard>` section of [`15c.ncd.tmpl`](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.ncd.tmpl).

## Firmware entry points — research notes

The following addresses are useful *investigation anchors*, not yet a verified functional segmentation of all ROM:

| ROM address | Initial interpretation | Evidence status |
|---|---|---|
| `0000` | Startup entry / initial dispatch | Observed (ROM); detailed flow requires execution-state verification |
| `0019` | Keyboard-code reading sequence | Observed (ROM); preliminary |
| `0351` | Candidate initialization path reached during ON + minus handling | Tentative; control-flow trace pending |
| `172E` | Candidate self-test path reached during ON + multiply handling | Tentative; control-flow trace pending |
| `174E` | ROM read / accumulation loop including `CXISA` | Tentative checksum interpretation; bounds and expected result not yet verified |
| `2AE9` | Candidate RAM-clearing subroutine | Tentative; exact writes and post-reset state require tracing |

**Important:** a linear disassembly does not reliably separate code from embedded data. These labels must not be treated as final routine names.

## Hardware and firmware are different layers

The tables above describe **addressable hardware resources in Nonpareil's HP-15C model**. Their **firmware functions** (RPN stack, storage registers, program memory, matrix allocation, etc.) are not determined solely by the physical address map.

Future entries will attach ROM code and trace evidence before assigning such names.

[Back to index](../index.md)
