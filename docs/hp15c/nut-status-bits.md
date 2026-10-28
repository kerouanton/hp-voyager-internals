---
title: HP-15C NUT CPU Status Bits (ST)
---

# NUT CPU Status Bits: HP-15C

[Home]({{ '/index.html' | relative_url }}) · [NUT instruction set]({{ '/hp15c/instruction-set.html' | relative_url }}) · [Microcode map]({{ '/hp15c/rom-map.html' | relative_url }}) · [RAM 015 allocation]({{ '/hp15c/ram015-allocation.html' | relative_url }})

The **original NUT CPU** exposes **14 internal status bits, S0–S13**, in a register conventionally called **ST**. Their physical existence and instruction semantics must be distinguished from **the meaning assigned to them by the particular calculator's microcode**.

**Scope:** original HP-15C ROM and NUT semantics in the [pinned Nonpareil V1 implementation](https://github.com/brouhaha/nonpareil/tree/c347bc1ab20170c253512042f7aac0d952f304ea). HP-10C, HP-16C and HP-41 flag assignments are **not portable** to the HP-15C. This page also distinguishes CPU ST from the calculator's **user flags** (e.g. user `g SF 8`).

## CPU representation and instruction effects

Nonpareil models the status register as [`bool s[SSIZE]`, where `SSIZE=14`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h#L30), with the [array declared in `nut_reg_t`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h#L124).

| Instruction | Effect as implemented in the pinned NUT emulator |
|---|---|
| `SF n` / `s=1 n` | Set selected internal status bit n |
| `CF n` / `s=0 n` | Clear selected internal status bit n |
| `?FS n` / `? s=1 n` | Test the selected bit and set the instruction's carry accordingly |
| `ST=C` / `st=c` | Copy **C[0] and C[1]** into **S0–S7**, four bits per C digit |
| `C=ST` / `c=st` | Copy **S0–S7** into C[0] and C[1] |
| `C<>ST` / `c<>st` | Exchange **S0–S7** with C[0] and C[1] |
| `CLR ST` / `s=0` | Clear **S0–S7 only** |

The pinned [implementation of flag operations](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1093-L1182) and [operation decoder](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1741-L1761) are the primary sources. Status-bit indexes in the 10-bit instruction encoding use a **nonlinear mapping** (`tmap` / `itmap`); do not infer the status number directly from the raw high opcode bits.

**Key subtlety:** ST has **14** status bits, but the bulk C↔ST operations above operate on **only eight**. A subroutine can therefore save S0–S7 into C or another storage location, use some of them as scratch bits, and restore them before returning. That does **not** imply that S8–S13 are unused or unaffected by their individual flag instructions.

For historical context, [Ken Emery's *HP-41 MCODE For Beginners* (1985)](https://literature.hpcalc.org/items/1605) covers MCode for the HP-41 family, which also uses the NUT. Its **HP-41 operating-system conventions and status-bit interpretations must not be treated as HP-15C facts**. The precise S0–S13 and C↔ST statements above are checked against pinned Nonpareil V1, not assumed from the book.

## Identified HP-15C microcode contexts

These are **local observations in the pinned original ROM**, not declarations that a flag has the same meaning during every function.

| Status bit | Established context | Scope |
|---|---|---|
| **S0** | Cleared during part of the reset/wake and retention-signature path | Startup path; meaning elsewhere unassigned |
| **S4** | Selects test-pattern/phase handling in diagnostics | Autotest routines around `0375` and `172E` |
| **S5** | Controls repeat behavior on a diagnostic key path | Diagnostic path around `005E–0064` |
| **S10** | Distinguishes lower `0000–17FF` and upper `2000–37FF` ROM checksum passes | Autotest around `1733–1772` |
| **S11** | Participates in RAM/register-test control logic | Diagnostic paths; no universal meaning established |

See [the HP-15C ROM map's diagnostic analysis]({{ '/hp15c/rom-map.html' | relative_url }}#diagnostics-and-self-test) for evidence and address-level context. The statuses of **S1–S3, S6–S9, S12 and S13** are **not comprehensively identified** here, and **no bit is declared free for permanent reuse**.

## Why a global list of `SF` / `CF` instructions is insufficient

Eric Smith explained in the [Museum of HP Calculators discussion “MCode: Nut CPU Status Bits”, 12 February 2026](https://www.hpmuseum.org/forum/thread-24677.html) that some bits represent global state, some are reused across digit entry and unrelated numerical routines, and others are short-lived function selectors. Routines can preserve and restore S0–S7. His statement is **first-hand general NUT/Voyager research guidance**, not a fully decoded HP-15C status map.

Consequently, merely counting `SF n`, `CF n` and `?FS n` in a linear ROM listing can mislead because:

- Different routines give a bit different meanings and may save and restore it.
- Instruction words can also appear as table data, not executable code.
- Callers may enter with distinct status values.
- Bulk C↔ST operations can modify S0–S7 without an explicit `SF` or `CF`.
- The NUT carry is **not** one of the 14 stored S bits; it follows its own instruction-to-instruction semantics.
- CPU status **S8** is **not** calculator user flag **8** (the latter is handled by calculator microcode and memory state).

## Proposed method for a reliable HP-15C ST map

Start with a **specific routine and entry state**, rather than a flag number:

1. Determine entry conditions and caller-provided status bits.
2. Trace single-bit operations and C↔ST exchanges.
3. Identify local save/restore boundaries across calls.
4. Observe whether a value survives the routine's return.
5. Reproduce at least two distinct entry states before claiming a global semantic assignment.
6. Keep a traceable table of PC, raw word, decoded operation, status before/after, and carry.

**Current result:** the **CPU-level operation semantics are established by source**, and a handful of **HP-15C routine-local uses are anchored in the ROM**. A general meaning for each of S0–S13 remains an open project.

[Original ROM provenance]({{ '/microcode/rom-provenance.html' | relative_url }}) · [NUT architecture]({{ '/hp15c/nut-cpu.html' | relative_url }})
