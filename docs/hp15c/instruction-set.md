---
title: NUT CPU Instruction Set Reference
---

# NUT CPU Instruction Set Reference

[Home]({{ '/index.html' | relative_url }}) · [Address index]({{ '/hp15c/reference-index.html' | relative_url }}) · [NUT architecture]({{ '/hp15c/nut-cpu.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }})

An address-oriented instruction-set guide for the **NUT CPU used by the original HP-15C**. Inspired by the [6502/6510 instruction reference at C64 OS](https://c64os.com/post/6502instructions), but based on **Eric Smith's Nonpareil NUT decoder and execution implementation** rather than transcribing a 6502 table.

**Status:** Reference to the pinned **Nonpareil emulator behavior**, not an independently measured silicon timing/errata specification. Most instruction names are the Nonpareil disassembler's editorial mnemonics. Some generic NUT opcodes represent HP-41 extensions or other peripheral behaviors **not wired into the HP-15C**.

## Quick index

[Encoding](#10-bit-encoding) · [Arithmetic (32 × 8)](#arithmetic-and-register-operations) · [Fields](#arithmetic-fields) · [Fixed instructions](#fixed-and-system-instructions) · [Parameterized instructions](#parameterized-miscellaneous-instructions) · [Branches](#relative-and-absolute-branches) · [Carry](#carry-and-conditional-execution) · [Examples](#examples-from-the-hp-15c-rom) · [Sources](#sources-and-attribution)

## 10-bit encoding

A NUT opcode is a **10-bit word** in the range `000–3FF` hex. The hexadecimal notation always has three digits; the leading two bits of the first hex digit are unused. There are 1,024 possible first words, but **not 1,024 distinct instruction names**: operand bitfields, decode state and second words matter.

| Low two bits | First-word pattern | Interpretation |
|---|---|---|
| `00` | `xxxxxxxx00` | Miscellaneous/system instruction. `130` (`LDI`) consumes a second word. |
| `01` | `xxxxxxxx01` | Two-word absolute branch or call. |
| `10` | `xxxxxxxx10` | Arithmetic, comparison, transfer or nibble shift. |
| `11` | `xxxxxxxx11` | One-word relative branch. |

**Length:** generally one 10-bit word; `LDI` and absolute calls/branches use two. `CXISA` makes a separate ROM read at an address formed from C, **not** an immediate second-word operand. `SELPF` changes how following words are decoded.

```text
Arithmetic first word:  OOOOO FFF 10
                         |   |
                         |   +-- field selector, 3 bits
                         +------ operation selector, 5 bits

opcode = (operation << 5) | (field << 2) | 0b10
```

Here `operation` is 0–31 and `field` is 0–7. The arithmetic table below gives the **W-field variant** (field code 3) of every operation. For another field, replace bits 4:2 with its field code.

## Arithmetic fields

The same 32 operations are parameterized by eight nibble ranges. Pointer references refer to the currently selected **P or Q** pointer.

| Field code | Mnemonic | Affected nibble(s) |
|---:|---|---|
| 0 | `p` | Active pointer only |
| 1 | `x` | 0–2 (not the user-visible RPN X) |
| 2 | `wp` | 0 through active pointer |
| 3 | `w` | 0–13 (all 14 nibbles) |
| 4 | `pq` | P through Q; if P > Q, P through 13 |
| 5 | `xs` | 2 |
| 6 | `m` | 3–12 |
| 7 | `s` | 13 |

Each arithmetic row represents **eight opcodes**: 32 operations × 8 fields = **256 10-bit encodings**.

## Arithmetic and register operations

The table shows **W (full-register) variant**, the exact first-word hex value and its full 10-bit binary representation. This includes all 32 operation selectors found in `nut_arith_info` in Nonpareil.

| Mnemonic (field W) | Hex | Binary (10 bits) | Effect in selected field |
|---|---|---|---|
| `a=0` | `00E` | `0000001110` | Clear A in the selected field. |
| `b=0` | `02E` | `0000101110` | Clear B in the selected field. |
| `c=0` | `04E` | `0001001110` | Clear C in the selected field. |
| `ab ex` | `06E` | `0001101110` | Exchange A and B in the selected field. |
| `b=a` | `08E` | `0010001110` | Copy A to B in the selected field. |
| `ac ex` | `0AE` | `0010101110` | Exchange A and C in the selected field. |
| `c=b` | `0CE` | `0011001110` | Copy B to C in the selected field. |
| `bc ex` | `0EE` | `0011101110` | Exchange B and C in the selected field. |
| `a=c` | `10E` | `0100001110` | Copy C to A in the selected field. |
| `a=a+b` | `12E` | `0100101110` | Add B to A in the selected field. |
| `a=a+c` | `14E` | `0101001110` | Add C to A in the selected field. |
| `a=a+1` | `16E` | `0101101110` | Increment A in the selected field. |
| `a=a-b` | `18E` | `0110001110` | Subtract B from A in the selected field. |
| `a=a-1` | `1AE` | `0110101110` | Decrement A in the selected field. |
| `a=a-c` | `1CE` | `0111001110` | Subtract C from A in the selected field. |
| `c=c+c` | `1EE` | `0111101110` | Double C in the selected field. |
| `c=a+c` | `20E` | `1000001110` | Add A to C in the selected field. |
| `c=c+1` | `22E` | `1000101110` | Increment C in the selected field. |
| `c=a-c` | `24E` | `1001001110` | Subtract C from A, writing the result to C. |
| `c=c-1` | `26E` | `1001101110` | Decrement C in the selected field. |
| `c=-c` | `28E` | `1010001110` | Negate C in the selected field using the selected arithmetic radix. |
| `c=-c-1` | `2AE` | `1010101110` | Form the radix-complement of C minus one in the field. |
| `? b<>0` | `2CE` | `1011001110` | Test whether B is nonzero in the field; result in carry. |
| `? c#0` | `2EE` | `1011101110` | Test whether C is nonzero in the field; result in carry. |
| `? a<c` | `30E` | `1100001110` | Test whether A is less than C in the field; result in carry. |
| `? a<b` | `32E` | `1100101110` | Test whether A is less than B in the field; result in carry. |
| `? a#0` | `34E` | `1101001110` | Test whether A is nonzero in the field; result in carry. |
| `? a#c` | `36E` | `1101101110` | Test whether A differs from C in the field; result in carry. |
| `a sr` | `38E` | `1110001110` | Shift A right by one nibble within the field. |
| `b sr` | `3AE` | `1110101110` | Shift B right by one nibble within the field. |
| `c sr` | `3CE` | `1111001110` | Shift C right by one nibble within the field. |
| `a sl` | `3EE` | `1111101110` | Shift A left by one nibble within the field. |

**Carry behavior:** arithmetic add, subtract, increment, negate and compare/test operations can set carry. Plain transfers, clear/exchange and shifts do not set carry in the decoder's model. The radix (10 or 16) is controlled by `set dec` / `set hex`; nibble storage remains four bits. Test mnemonics set carry according to the comparison; they are not branches by themselves.

## Fixed and system instructions

Hex and binary columns identify the **single first opcode word**. Unless noted as `LDI`, the instruction itself is one word. The display-control instructions are implemented by the Voyager driver, not an invented RAM-mapped control register.

| Hex | Binary (10 bits) | Mnemonic | Operational meaning |
|---|---|---|---|
| `000` | `0000000000` | `nop` | No operation. |
| `020` | `0000100000` | `pop stk` | Discard one address from the 4-deep return stack. |
| `060` | `0001100000` | `powoff` | Enter sleep/off state; on wake the PC starts at the reset/wake path. |
| `0A0` | `0010100000` | `sel p` | Select P as the active digit pointer. |
| `0E0` | `0011100000` | `sel q` | Select Q as the active digit pointer. |
| `120` | `0100100000` | `? p=q` | Compare P and Q; sets carry for conditional control flow. |
| `160` | `0101100000` | `lld` | Low-battery test (the examined emulator models battery good). |
| `1A0` | `0110100000` | `clr abc` | Clear the A, B and C registers. |
| `1E0` | `0111100000` | `goto c` | Load PC from the address in C[6:3]. |
| `220` | `1000100000` | `c=keys` | Read the keyboard latch into C[4:3]. |
| `260` | `1001100000` | `set hex` | Select hexadecimal arithmetic. |
| `2A0` | `1010100000` | `set dec` | Select decimal arithmetic. |
| `2E0` | `1011100000` | `disp off` | Disable the Voyager display via its driver. |
| `320` | `1100100000` | `disp toggle` | Toggle Voyager display enable. |
| `360` | `1101100000` | `?c rtn` | Return if previous carry is set. |
| `3A0` | `1110100000` | `?nc rtn` | Return if previous carry is clear. |
| `3E0` | `1111100000` | `rtn` | Pop the return stack into PC. |
| `030` | `0000110000` | `disp blink` | Enable Voyager display blinking. |
| `070` | `0001110000` | `n=c` | Copy C into N. |
| `0B0` | `0010110000` | `c=n` | Copy N into C. |
| `0F0` | `0011110000` | `c<>n` | Exchange C and N. |
| `130` | `0100110000` | `ldi` | Two-word literal load; place next ROM word in C[2:0]. |
| `170` | `0101110000` | `stk=c` | Push address formed from C[6:3]. |
| `1B0` | `0110110000` | `c=stk` | Pop return stack into C[6:3]. |
| `230` | `1000110000` | `goto keys` | Replace the low PC byte with latched keycode. |
| `270` | `1001110000` | `sel ram` | RAM selector ← low 10 bits encoded in C[2:0]. |
| `2F0` | `1011110000` | `data=c` | Write C to selected RAM data address. |
| `330` | `1100110000` | `cxisa` | Read ROM word addressed by C[6:3] into C[2:0]. |
| `370` | `1101110000` | `c=c|a` | Bitwise OR C with A. |
| `3B0` | `1110110000` | `c=c&a` | Bitwise AND C with A. |
| `3F0` | `1111110000` | `sel pfad` | Set peripheral selector from C[1:0]. |
| `038` | `0000111000` | `c=data` | Read selected RAM register into C. |
| `3C4` | `1111000100` | `clr s` | Clear low status bits as implemented by the NUT core. |
| `3C8` | `1111001000` | `clr kb` | Advance keyboard scanner release/debounce state. |
| `3CC` | `1111001100` | `? kb` | Test keyboard scanner state via carry. |
| `3D4` | `1111010100` | `dec pt` | Decrement active digit pointer, wrapping over 14 nibbles. |
| `3DC` | `1111011100` | `inc pt` | Increment active digit pointer, wrapping over 14 nibbles. |

**Context matters:** the NUT core's `clr s` / F transfers affect the low 8 status bits; indexed status instructions can address other S bits. `c=keys`, `? kb` and `clr kb` interact with the emulator keyboard state machine, not a memory-mapped keyboard matrix. `disp compensation` (`3FC`), `wrrom`, write-protect toggles and several bank/peripheral opcodes may appear in the generic decoder but do not automatically imply implemented HP-15C devices.

## An instruction word can target a memory chip: `2B0`

The generic Nonpareil disassembler labels 10-bit word **`2B0`** (octal **`1260`**) as `clr regs`. That name is **not proof of a NUT CPU instruction that clears all RAM**. In a [March 2026 Museum of HP Calculators discussion](https://www.hpmuseum.org/forum/thread-24718.html?highlight=r2d2), Eric Smith explains that the NUT CPU itself ignores this word: any effect belongs to a **RAM chip** that recognizes the command. Different calculator memory chips can implement it differently—or not at all.

In the pinned [Nonpareil NUT execution engine](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c), the CPU handler for `0x2b0` is **commented out**, even though the [disassembler prints `clr regs`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c). The pinned **original HP-15C ROM dump contains no word `2B0`**. Smith specifically did **not** claim to have tested its behavior on the HP-15C R2D2 chip types `1LE2` and `1LH1`.

**Status for the original HP-15C:** peripheral/RAM-chip command in historical NUT system documentation; **R2D2 hardware effect unverified**. Do not treat this as the mechanism behind the firmware's proven ON + minus clearing loop, which issues explicit RAM writes.

This illustrates an important architectural distinction: a *10-bit instruction word seen on the machine's instruction path* may have a function implemented by a connected device rather than by the NUT arithmetic/control core. The full cross-family NUT opcode vocabulary is broader than the subset of confirmed HP-15C hardware behaviors.

## Parameterized miscellaneous instructions

For `n` in 0–15, the formula `base + 0x40 × n` produces an exact **10-bit** opcode. Here `n` is a binary value, not a literal digit printed in an assembler. Indexed status/pointer operands use the nonsequential `tmap` encoding discussed below.

| Base hex | Family | Encoding | Behavior and caveats |
|---|---|---|---|
| `010` | `lc n` | `010 + 040·n` | Load four-bit constant n at active pointer, then step pointer according to CPU semantics. |
| `024` | `selpf n` | `024 + 040·n` | Select smart peripheral and switch subsequent decoder state. |
| `028` | `reg=c n` | `028 + 040·n` | Replace low nibble of selected RAM address with n, then write C. |
| `038` | `c=reg n` | `038 + 040·n` | For n>0 replace low address nibble, then read; n=0 means `c=data` without replacement. |
| `004` | `s=0 k` | `004 + 040·n` | Clear decoded status bit k; final n=15 encodes special `clr s`. |
| `008` | `s=1 k` | `008 + 040·n` | Set decoded status bit k; final n=15 encodes `clr kb`. |
| `00C` | `? s=1 k` | `00C + 040·n` | Test decoded S[k] through carry; final n=15 encodes `? kb`. |
| `014` | `? pt=k` | `014 + 040·n` | Test active pointer; final n=15 encodes `dec pt`. |
| `01C` | `pt=k` | `01C + 040·n` | Set active pointer; final n=15 encodes `inc pt`. |
| `02C` | `? ext k` | `02C + 040·n` | Query external hardware flag bit via carry (wiring is model-dependent). |
| `03C` | `rcr k` | `03C + 040·n` | Rotate C right by decoded nibble count; final n=15 encodes special `disp compensation`. |

The decoder maps the opcode's top four bits through:
```text
encoded n :  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
decoded k:  3  4  5 10  8  6 11  -  2  9  7 13  1 12  0  -
```
A dash means no ordinary pointer/status index. Certain encodings are special instructions as noted above. **Do not** use a simple arithmetic mapping `k=n` for pointer/status/rotate instructions.

## Relative and absolute branches

### Relative branch: low bits `11`, one word

A signed **7-bit displacement** is encoded across the first word. The destination is computed from the **address of the branch instruction**, modulo the 16-bit program counter. Branch selection tests **previous carry**, not a high-level zero flag.

| First-word bits | Interpretation |
|---|---|
| `[9]` + `[8:3]` | Signed seven-bit offset, range −64 to +63 |
| `[2]` | Required previous-carry condition (`?c` vs `?nc`) |
| `[1:0] = 11` | Relative branch category |

The disassembler may omit the explicit condition for a branch when its data-flow analysis already knows the carry state; this is an annotation optimization, **not a new opcode type**.

### Absolute branch or call: low bits `01`, two words

Both the instruction and the destination operand occupy **10-bit ROM words**. The destination is:

```text
target = (word1 >> 2) | ((word2 & 0x3FC) << 6)
```

The second word's low two bits control **call versus jump** and carry qualification. A subroutine call saves a return address on the hardware four-entry stack. If conditional execution does not take the transfer, the PC continues after the second word. Unmapped ROM behavior and special HP-41 branch forms should be checked against the CPU implementation rather than inferred from the printed mnemonic alone.

## Carry and conditional execution

Unlike a 6502 status register, the NUT model has separate `carry` and `prev_carry` state. At the beginning of each CPU execution cycle, the implementation preserves the preceding condition and clears the new carry; branch/conditional-return handlers evaluate the preserved previous value.

This makes **instruction ordering important**: a comparison/test may precede a `?c` or `?nc` branch, and inserting an instruction can change what condition the branch sees.

Decimal/hexadecimal mode changes the numeric radix of relevant arithmetic instructions, but does not change the 10-bit ROM encoding.

## Examples from the HP-15C ROM

| ROM address | Raw opcode | Interpretation |
|---|---|---|
| `0002` | `270` | `sel ram` selects the RAM address formed by C[2:0]. |
| `001A` | `220` | `c=keys` reads hardware keycode. |
| `0351` | `130` | `ldi` consumes the next ROM word as an immediate literal. |
| `174E` | `330` | `cxisa` reads a ROM word via a computed C address; part of the investigated checksum loop. |
| `2AF1` | `2F0` | `data=c` writes the selected data register; part of the initialization sweep. |

See the [ROM Map]({{ '/hp15c/rom-map.html' | relative_url }}) for instruction context and the [RAM Register Uses]({{ '/hp15c/ram-register-map.html' | relative_url }}) for caller-dependent physical RAM addresses.

## Scope and limitations

This is a **family-level instruction reference**, not a measured cycle-by-cycle silicon specification. Nonpareil emulates several members of the NUT family; its generic disassembler recognizes opcodes and peripheral extensions beyond the original Voyager configuration. Model-specific peripherals can install their own handlers and affect instruction meaning.

The arithmetic family and standard miscellaneous families are tabulated, but this revision does not claim a machine-generated list of the semantics of all 1,024 first words in every decoder state. Unknown, reserved and peripheral-dependent encodings remain explicitly outside that claim.

## Sources and attribution

- [Eric Smith's Nonpareil — NUT disassembler, pinned revision](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c) (instruction names, families and operand field map).
- [Nonpareil — NUT CPU execution engine](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) (execution semantics).
- [NUT CPU state definitions](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h).
- [Nonpareil HP-15C ROM dump](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom) (example opcodes).
- [Project standalone disassembler](https://github.com/kerouanton/hp-voyager-internals/tree/main/tools/hp15c-disassembler).

**Technical foundation:** Eric Smith / Nonpareil. **Documentation research, synthesis, tables and GitHub editing:** OpenAI ChatGPT in interactive manual (*agentless*) mode, under project-maintainer direction. This page does not reproduce upstream implementation source code.

[Back to top](#nut-cpu-instruction-set-reference)
