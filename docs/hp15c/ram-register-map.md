---
title: HP-15C RAM Register Uses
---

# HP-15C RAM Register Uses

[Address and topic index]({{ '/hp15c/reference-index.html' | relative_url }}) · [Home]({{ '/index.html' | relative_url }})

Functional reference for the original HP-15C's **80 ordinary 56-bit RAM registers**. This document combines the actual ROM, Nonpareil instruction semantics, and controlled execution experiments. The address inventory is complete; functional identification remains partial.

[Home]({{ '/index.html' | relative_url }}) · [Complete memory map]({{ '/hp15c/memory-map.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [Method](#method-and-confidence) · [Access semantics](#how-the-rom-addresses-ram) · [Functional findings](#evidence-supported-functional-findings) · [Indirect addressing](#indirect-address-computation) · [All 80 registers](#complete-80-register-inventory) · [Unknowns](#unknown-allocations) · [Sources](#sources)

Addresses, raw register contents and keycodes are hexadecimal. Bit/nibble indexes, counts and user register numbers are decimal. A physical data address names a 14-nibble register, not a byte. CPU registers A/B/C/M/N and the four-entry return stack are separate resources.

## Method and confidence

| Label | Meaning |
|---|---|
| **Confirmed (configuration)** | Explicit ordinary-RAM ranges in the original HP-15C model. |
| **Confirmed (emulation)** | Explicit access behavior in the pinned Nonpareil CPU/device source. |
| **Observed (ROM)** | Raw-ROM operation or an executed access in a stated experiment. Observations are limited to the tested states and paths. |
| **Tentative** | Functional interpretation beyond the established operation or tested state. |

The CPU-only research harness incorporates the instruction implementations from Nonpareil's proc_nut.c and digit_ops.c, with the original register definitions. It supplies the configured RAM/ROM, bitmap-read masks, keyboard scanner events and display-enable/sleep-carry behavior. GUI rendering and sound are omitted; LCD refresh/blink timing is not reproduced. This is an adapted research harness, not an unmodified full Nonpareil session or a physical-calculator test.

**Trace reproducibility note:** The instrumented CPU-only harness and raw event logs were not included with the supplied Markdown deliverable. The executed-access findings below are reported by **ChatGPT Work** for its specified experiments, not independently replayed by the project's documentation editor. Source-code and ROM instructions can be checked separately using the linked Nonpareil revision. These reported traces must not be treated as exhaustive coverage.

At each executed normal-state instruction, the recorder captures instruction address, operation, effective data address, and C before/after execution. It resolves the current RAM selector and the instruction's low-nibble replacement before recording an access. LDI operands, long-branch second words and CXISA data-read cycles are not treated as normal instructions. Writes are recorded even when their value equals the old value. Thus the evidence is an access trace, not merely a comparison of changed RAM cells.

Experiments begin from zero-filled emulator storage and CPU reset. ON is pressed, then backspace is pressed to dismiss the initial post-initialization condition before entering test data. A key is held for up to 2,000 CPU cycles, then released, followed by execution to sleep (limit 2,000,000 cycles). All reported ordinary key experiments reached sleep. Input timing is a harness condition, not a hardware timing specification.

| Experiment | Keys after ON/backspace | Purpose / observed result |
|---|---|---|
| Stack | 1, ENTER, 2, ENTER, 3, ENTER, 4 | Idle N = 4; RAM 000 = 3, 001 = 2, 002 = 1. |
| Arithmetic | 1, ENTER, 2, + | Result 3 in idle N; demonstrates ordinary real-mode execution. Does not establish LAST X's storage address. |
| Ten independent storage tests | 7, STO, digit n, backspace, RCL, digit n; n = 0…9 | Each selected register receives the packed value for 7; recall reads the same value and restores idle N = 7. |

The positive integer k in these experiments has the observed packed representation k × 16^12: for example 7 is `07000000000000`. This example is not a complete specification of floating-point, sign, exponent or complex formats.

The published standalone decoder is used to inspect candidate sequences, with instruction semantics checked against CPU source. A printed direct-looking RAM operand does not by itself identify an absolute address. No exhaustive control-flow or all-modes coverage is claimed.

## Physical scope

| Ordinary RAM addresses | Registers | R2D2 device |
|---|---:|---|
| 000–007 | 8 | Index 0 |
| 010–017 | 8 | Index 1 |
| 0C0–0DF | 32 | Index 1 |
| 0E0–0FF | 32 | Index 0 |

**Confirmed (configuration).** Total: 80 × 56 = 4,480 bits. Six extra installed addresses are excluded: 008/018 read zero and ignore writes; 009/00A drive LCD bitmaps; 019/01A have bitmap-style low-six-bit read masking without a second connected LCD. These special addresses can participate in microcode operations but are not six additional ordinary RAM registers.

## How the ROM addresses RAM

| Instruction | Effective address / persistent selector effect |
|---|---|
| `sel ram` (270) | Selector ← C[2:0] masked to 10 bits. |
| `c=data` (038) | Read the current selected address without changing it. |
| `c=reg n`, n > 0 | Replace selector's low nibble with n, then read. Upper selector bits persist. |
| `reg=c n`, including n = 0 | Replace selector's low nibble with n, then write C. |
| `data=c` (2F0) | Write C at the unchanged selected address. |

**Confirmed (emulation):** [selection and access handlers](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L901-L1056). RAM and peripheral selectors are separate. The original HP-15C driver does not install the generic HP-41 peripheral set.

For example, `c=reg 5` can access 005, 015 or 0C5 depending on the selector's upper bits. A prior C modification does not change the selector until `sel ram` executes. Conversely, indexed reads/writes can change the selector even without another `sel ram`. Tracking these two kinds of updates is essential across calls.

## Evidence-supported functional findings

### Lower RPN stack and CPU N

In the initialized real-number state tested here, **000 = Y, 001 = Z, 002 = T**, and N holds X at idle checkpoints. Confidence: **Observed (ROM)** for the experiments and explicit shift operations; global invariance across complex, program, matrix and diagnostic modes is not asserted.

The service at [`17E8–17F3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6123-L6134) saves incoming C in M, selects RAM page zero and performs:

| Write site | Effect |
|---|---|
| [`17EC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6127) | RAM[002] ← RAM[001] |
| [`17F0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6131) | RAM[001] ← RAM[000] |
| [`17F2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6133) | RAM[000] ← saved incoming C |

The stack-entry experiment supplies semantic evidence for the names Y/Z/T; the shift sequence alone would only prove a three-register shift. This hardware RAM usage must not be confused with the CPU return-address stack. N is also used temporarily by microcode, including diagnostics.

### User storage registers R0–R9

The following mapping is **Observed (ROM)** for freshly initialized allocation state and ordinary STO/RCL n. Every row was tested independently with a stored value of 7 and a later recall. Do not extrapolate this table to reallocated memory without tracing the descriptor-dependent addressing.

| User register | Physical address | Store site | Read site |
|---|---|---|---|
| R0 | `010` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R1 | `011` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R2 | `0C0` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R3 | `0C1` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R4 | `0C2` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R5 | `0C3` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R6 | `0C4` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R7 | `0C5` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R8 | `0C6` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |
| R9 | `0C7` | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376) | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) |

The storage operation's `data=c` at 0945 uses an already computed selector. Its opcode contains neither the user register number nor the physical address. The read at 2BC9 likewise uses the selected address; tracing the preceding computation is necessary.

### Display/conversion working registers

**004** is written at [`155A–1560`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5469-L5475) after selecting page zero in the display-processing path. In idle input experiments it contains an encoded representation different from numeric N: after entering 4, 004 is `04FFFFFFFFF000`; after recalling 7, it is `070000FFFFF000`. Calling it a **display/input working register** is supported by the writing path and observed values; calling it a permanent numeric X register is not.

**005 and 006** are written by the same broader display-conversion path. In the tested idle states they end as `00000000000008` and `0000000000000C`. The actual write sites include [`160F`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5650) (006) and [`1612`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5653) (005), while [`156E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5489) also writes 006 earlier with intermediate content. Their exact packed field definitions remain **unknown**. “Conversion workspace” is a **Tentative** general name; the individual writes are **Observed (ROM)**.

004 has another proved use during diagnostics: [`1730–1731`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5939-L5940) saves N to it, and [`1775–1778`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6008-L6011) restores N on the success path. Hence register purposes must be qualified by execution context.

### RAM 007: retention signature and other fields

[`0348–0350`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L843-L851) constructs `EAE` by complementing `151` in hexadecimal arithmetic, selects data page zero, reads register 7, and compares the low three nibbles. A match returns; mismatch falls through into initialization. [`13B1–13B6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5044-L5049) writes the signature before sleeping.

**Observed (ROM):** 007[2:0] is a retained-state validity marker for this check. This does not establish the meanings of its other nibbles. Other ROM paths read, rotate and rewrite 007, so it must not be described simply as an otherwise unused constant register; see [`037D–0383`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L896-L902).

### Dynamic allocation before R02

**Reported (first-hand):** in [Eric Smith's February 2024 explanation of the HP-15C R↑/R↓ microcode](https://www.hpmuseum.org/forum/thread-21336.html), the optional **SOLVE/INTEGRATE buffer always begins at physical `0C0`** when allocated. Optional imaginary components of **X, Y, Z, T, and LAST X** occupy RAM between that buffer and user register R02. Hence R02 at `0C0` is **only an initial, unallocated case**, not a permanent address binding.

The roll-down microcode reads the word immediately **before R02** as if it contained imaginary X. With neither optional area allocated, this means **`0BF`**, a nonexistent physical data address; the hardware read returns zero and the non-complex execution path discards the value. When the SOLVE/INTEGRATE buffer exists without a complex stack, the read can touch its last word instead. This is a first-hand microcode/hardware account; we have not reproduced a pin-level or independent dynamic-allocation trace.

**Open:** correlate those allocations with before/after contents of physical **RAM `015`** and verify the descriptor's individual nibbles through original-ROM call sites. The forum explanation establishes a conceptual layout, **not** a field-level decode of `015`. See [community findings]({{ '/hp15c/community-findings.html' | relative_url }}#dynamic-allocation-before-user-register-r02).

### RAM 015: address-descriptor source

Initialization writes `C0D2D2D2D2D2D2` to 015 at [`035C–0365`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L863-L872). Crucially, later code actually consumes this register when forming RAM addresses:

- [`2BB3–2BC0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9142-L9155) selects page 010, reads register 5, rotates C, clears C[2], adjusts the low address through a pointer-controlled loop, selects that address and reads it.
- [`2FC0–2FDB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L10179-L10206) has several pointer-selecting entry stubs, reads 015, rotates and adjusts its contents, then transfers to the generic select/read helper 2957.

Thus **address-descriptor source** is **Observed (ROM)**. A complete interpretation of the repeated D2 fields, matrix boundaries, stack extensions or allocation policy remains **Tentative/unknown**. The initial constant alone is insufficient to name those fields.

## Indirect address computation

| ROM sequence | Established computation / access | Confidence and limits |
|---|---|---|
| [`2957–2959`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8538-L8540) | Select address from C.x, read selected RAM into C, return | Observed (ROM); generic helper, meaning depends on caller. |
| [`295B–295D`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8542-L8544) | Load 010 and branch to 2957 | Observed (ROM); fixed RAM[010] read. |
| [`2BC2–2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9157-L9164) | Calls 1053 and 1037; reads register 4 of the currently selected page; clears C[2]; selects address from C and reads | Observed (ROM); effective read targets 010/011/0C0–0C7 in the reported storage tests. Full caller-dependent selector state is essential. |
| [`2BB3–2BC0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9142-L9155) | Read 015, rotate right 12, clear address high nibble, increment address while advancing pointer to 12, select and read | Observed (ROM); specific descriptor-field names not established. |
| [`2FC0–2FDB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L10179-L10206) | Entry stubs choose pointer 10, 9, 8, 7 or 11; read/transform 015; decrement C.x while advancing pointer; select/read through 2957 | Observed (ROM); pointer argument and descriptor jointly determine location. |
| [`2B8F–2B9E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9106-L9121) | Alternates selected-address reads and writes while incrementing C.x and decrementing a count in A.m | Observed (ROM); memory-moving mechanism. A specific program/matrix allocator role is not proved here. |
| [`2AE9–2AF5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8940-L8952) | Descending two-nibble address loop; select each address, write zero from B/C exchange, stop when C.wp equals A.wp | Observed (ROM); bounds supplied by caller. |

The C.x high nibble can be explicitly cleared before a computed access, constraining that particular path to addresses 000–0FF. This is microcode behavior; the CPU RAM selector itself remains 10 bits wide.

### Initialization sweep

At [`0355–035B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L856-L862), A.x = 0C0 and C.x = 01B before the call to 2AE9. With P = 1, only the low two nibbles are decremented. Attempts occur at 01A down to 000, then wrap to 0FF and continue to 0C0 inclusive: **91 attempts**, covering all 80 ordinary registers plus four masked registers, two ignored registers and five unmapped addresses. Clearing is followed by nonzero setup writes. A trace showing zero writes from 2AF1 therefore proves initialization coverage, not a user-visible role for each target.

## Complete 80-register inventory

All ordinary physical addresses are listed individually. The read/write columns contain **executed instruction addresses from the reported experiments**, consolidated across tests. Initialization writes at 2AF1 are included. A dash means no such access was observed in this workload, not that the microcode never accesses it. These are instruction sites, not complete function boundaries or exhaustive cross-references.

Register purpose is **Observed (ROM)** only where explicitly scoped above. “Unknown” means that initialization or incidental access does not establish a functional allocation.

| RAM | Supported purpose / limitation | Observed read sites | Observed write sites |
|---|---|---|---|
| `000` | Y in tested real-mode stack; other modes not mapped | [`0898`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2203), [`17EF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6130), [`2958`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8539) | [`17F2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6133), [`27EE`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8177), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `001` | Z in tested real-mode stack; other modes not mapped | [`17EB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6126), [`27ED`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8176) | [`17F0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6131), [`27F0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8179), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `002` | T in tested real-mode stack; other modes not mapped | [`27EF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8178) | [`17EC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6127), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `003` | Unknown; no non-initialization access in this workload | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `004` | Display/input working value; diagnostic N save in another path | [`1614`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5655), [`16BF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5826), [`2F7A`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L10109) | [`1560`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5475), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948), [`2F49`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L10060) |
| `005` | Display-path intermediate state; packed fields unknown | [`163F`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5698) | [`1612`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5653), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `006` | Display-path intermediate state; packed fields unknown | [`157B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5502), [`1589`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5516), [`1595`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5528), [`15A1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5540), [`15B2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5557), [`15C0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5571), [`15CC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5583), [`15DA`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5597), [`15E8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5611) | [`156E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5489), [`160F`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5650), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `007` | Low three nibbles: EAE retention signature; other fields unresolved | [`034E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L849), [`13B1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5044), [`1602`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5637), [`1691`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5780), [`16A8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5803) | [`13B5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5048), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `010` | R0 in initial allocation | [`2958`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8539), [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `011` | R1 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `012` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `013` | Unknown allocation; initialization zero-write observed | — | [`222B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6702), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `014` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `015` | Address-descriptor source; initial C0D2D2D2D2D2D2 | [`2958`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8539) | [`0365`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L872), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `016` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `017` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C0` | R2 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C1` | R3 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C2` | R4 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C3` | R5 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C4` | R6 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C5` | R7 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C6` | R8 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C7` | R9 in initial allocation | [`2BC9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L9164) | [`0945`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L2376), [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C8` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0C9` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CA` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CB` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CC` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CD` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CE` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0CF` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D0` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D1` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D2` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D3` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D4` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D5` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D6` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D7` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D8` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0D9` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DA` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DB` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DC` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DD` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DE` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0DF` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E0` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E1` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E2` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E3` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E4` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E5` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E6` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E7` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E8` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0E9` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0EA` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0EB` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0EC` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0ED` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0EE` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0EF` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F0` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F1` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F2` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F3` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F4` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F5` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F6` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F7` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F8` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0F9` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FA` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FB` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FC` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FD` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FE` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |
| `0FF` | Unknown allocation; initialization zero-write observed | — | [`2AF1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8948) |

## Unknown allocations

| Allocation question | Current evidence boundary |
|---|---|
| 003 | No non-initialization access in the tested workload. It is not identified as LAST X merely because it follows the three lower stack registers. |
| 012–014, 016–017 | Physical RAM confirmed; ordinary purpose unresolved. The existence of indirect helpers does not identify every target's semantic role. |
| 0C8–0FF | Initialized ordinary RAM. A complete partition into storage registers, programs, matrices or temporary data is not established by these tests. |
| R10 and higher, indirect I addressing | Not exercised; no mapping inferred from the first ten registers. |
| LAST X | Not located by the arithmetic experiment; CPU-internal storage and mode-dependent representation must be considered. |
| Complex stack and imaginary components | Not traced; real-mode stack names are not promoted to a full complex-memory map. |
| Statistics accumulators | Not traced or assigned. |
| Program steps, return levels and program allocator | Not traced; packed program-byte layout and dynamic boundaries remain unknown. |
| Matrix elements and dimensions | Descriptor-dependent address generation is visible, but element ordering, headers and relocation policy are not established. |
| User flags and display settings | Exact packed RAM bits/nibbles remain unassigned. CPU S bits are a separate namespace. |
| Diagnostic temporary reuse | A few explicit save/restore uses are identified; a complete all-path lifetime analysis is not provided. |

## Sources

- [Original HP-15C model](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl): ordinary RAM and special-device configuration.
- [Original ROM](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom): all address citations above refer to this image.
- [CPU implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c), [CPU state](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h), [digit arithmetic](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/digit_ops.c), [Voyager LCD/R2D2 driver](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c).
- [Upstream decoder](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c).
- [Project disassembler](https://github.com/kerouanton/hp-voyager-internals/blob/789ed0d540b022a97efe6afca369747b750e69b2/tools/hp15c-disassembler/hp15c_disasm.c) and [decoder tests](https://github.com/kerouanton/hp-voyager-internals/blob/789ed0d540b022a97efe6afca369747b750e69b2/tools/hp15c-disassembler/test_decoder.c).

Upstream revision: `c347bc1ab20170c253512042f7aac0d952f304ea`. ROM SHA-256: `dea4e861ad2aaa2bba49b5ea4e313a04f4fac9a4abf2516284ecc0f6f394568a`. Analysis date: 2026-10-28. Original configuration only; no 192-register patch.

Validation checked all ten STO/RCL target/value pairs and the complete 80-address inventory. These experimental results support the stated allocations in their initial real-mode context; they are not an all-ROM proof or a physical-hardware certification.

## Credits and publication notes

Original technical analysis and document draft: **ChatGPT Work** (2026-10-28). The investigation uses the HP-15C ROM and Eric Smith's **Nonpareil** NUT CPU and device implementations. Editorial review, source-link checks, and website publication: **OpenAI ChatGPT in manual ("agentless") mode**, under the direction of the project maintainer. The upstream ROM and emulator remain independently authored and subject to their own rights and licenses.

[Back to top](#hp-15c-ram-register-uses)