---
title: HP-15C RAM 015 — Dynamic Allocation Research
---

# HP-15C RAM `015`: Dynamic Memory Allocation

[Home]({{ '/index.html' | relative_url }}) · [RAM register map]({{ '/hp15c/ram-register-map.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [NUT status bits]({{ '/hp15c/nut-status-bits.html' | relative_url }})

**Status: partial, experimentally reproduced, 28 October 2026.** This page records what can be demonstrated about physical RAM register `015` from the **original** HP-15C microcode and a locally rerun, instrumented Nonpareil V1 research harness. It is **not** a complete allocation specification and does not describe expanded-memory, LE, or CE ROM variants.

## Source identity, methodology and evidence

The original [HP-15C ROM image](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom), pinned to Nonpareil commit `c347bc1ab20170c253512042f7aac0d952f304ea`, has SHA-256 `dea4e861ad2aaa2bba49b5ea4e313a04f4fac9a4abf2516284ecc0f6f394568a`. The experimental harness incorporates Nonpareil V1's NUT instruction behavior rather than substituting high-level mathematical algorithms.

The user-supplied Work investigation contains **19 deterministic scenarios**, structured instruction traces, effective RAM-access logs and complete RAM snapshots. It was subsequently **built and rerun locally**; the three generated analysis outputs (`results.json`, `allocation-transitions.csv`, `ram015-access-sites.csv`) matched those in the delivered archive. The verifier accepted **10,076 RAM-access events** and **22 predicted effective addresses** for the `2FC9` helper. The ROM hash and five original zero words at `13F9–13FD` were checked. The research package and raw traces are **not hosted in this repository**: reproducing these experimental observations requires the separately delivered archive and its pinned upstream source inputs.

**Evidence categories:** *Verified (ROM)* means directly established from the original instruction words; *Verified (source)* means established by pinned Nonpareil code; *Observed (rerun execution)* means a locally reproduced instrumented run (but **not** real-chip measurement); *Inferred* means the data support the interpretation without fully proving it. The report does not establish numerical SOLVE/INTEGRATE correctness, an exhaustive call graph or a full allocator field encoding.

## The seven-byte descriptor

RAM `015` holds **14 hexadecimal nibbles (56 bits)**. In textual dumps, nibble 13 is on the left and nibble 0 on the right. For analysis, split the word into seven **most-significant-first** bytes:

| Byte | Nibbles | Initial value | Interpretation and confidence |
|---|---|---|---|
| **B0** | 13:12 | `C0` | **Observed (rerun execution):** basis of R02-relative effective-address calculations; changes to `C5` in one complex allocation |
| **B1** | 11:10 | `D2` | **Inferred:** boundary after the initial user-storage span; remains `0x12` above B0 in tested states |
| **B2** | 9:8 | `D2` | **Inferred:** cumulative boundary affected by matrix A |
| **B3** | 7:6 | `D2` | **Inferred:** downstream boundary affected by matrices A and B |
| **B4** | 5:4 | `D2` | **Inferred:** downstream boundary affected by A, B and C |
| **B5** | 3:2 | `D2` | **Inferred:** further downstream boundary; D-specific behavior untested |
| **B6** | 1:0 | `D2` | **Inferred:** final observed boundary; exact relationship to E/program memory unproved |

The initialized value `C0D2D2D2D2D2D2` is written at ROM **`0365`**, after construction at [`035C–0365`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L863-L872). **Do not infer that B1–B6 are identically functioning fields merely because they all initially contain `D2`.**

The initial difference `D2 - C0 = 0x12 = 18` registers is consistent with R2–R19 occupying an initial span. This is a useful **hypothesis**, not yet a tested universal register-allocation rule.

## Measured descriptor transitions

Each scenario begins from an independently initialized original HP-15C model; the values are full physical RAM `015` contents.

| Test condition | Descriptor after action | What was actually observed |
|---|---|---|
| Initialisation | `C0D2D2D2D2D2D2` | Original microcode writes descriptor at `0365` |
| Complex allocation triggered by calculator `g SF 8` | `C5D7D7D7D7D7D7` | Each of seven bytes increases by 5; `STO 2` now targets `0C5` |
| Release after calculator `g CF 8` | `C0D2D2D2D2D2D2` | Observed return to initial descriptor |
| `DIM A` 1×1 | `C0D2D3D3D3D3D3` | B2–B6 increment by 1 |
| `DIM A` 2×2 | `C0D2D6D6D6D6D6` | B2–B6 increment by 4 |
| `DIM A` 3×2 | `C0D2D8D8D8D8D8` | B2–B6 increment by 6 |
| `DIM B` 1×1 | `C0D2D2D3D3D3D3` | B3–B6 increment by 1 |
| `DIM C` 1×1 | `C0D2D2D2D3D3D3` | B4–B6 increment by 1 |
| Complex allocation then `DIM A` 2×2 | `C5D7DBDBDBDBDB` | The allocation effects compose in the tested state |
| Delete matrix A after 2×2 | `C0D2D2D2D2D2D2` | Initial descriptor recovered |

**Observed (rerun execution):** updates to the descriptor occur at ROM **`2ABE`**, and are not merely changes inferred from a RAM diff; they are present in instruction-level access traces. The allocation tests support an **ordered cumulative-boundary model**. They do not alone prove the physical positions of every matrix header, element or program word.

The experimental inputs `g SF 8` and `g CF 8` are **HP-15C user-flag operations**. They must **not** be confused with NUT CPU status instructions `SF 8` and `CF 8` that modify S8 inside the processor.

## Microcode that transforms `015`

The initialization routine establishes `015` at `0365`. The **writer at `2ABE`** applies a computed allocation adjustment. The observed writer set also includes `2AF1` as part of the zeroing sweep; these are **workload-observed writer sites**, **not proof of an exhaustive set** for all possible ROM paths.

At **`2AA8` and `2AB9`**, the trace shows descriptor reads with RAM page `010` selected. The intervening **`2AB0–2AB7`** logic exchanges CPU registers, uses field-width additions, performs a 12-digit rotation and advances the digit pointer to propagate a change through part of the descriptor. The exact universal rule for selecting which bytes are updated remains **inferred**, pending a complete instruction-by-instruction proof.

Shared accesses to `015` also occur through generic address selection (`2958`) and at `2FCC`. A literal opcode search for `c=reg 5` alone misses indirect reads when `C=DATA` accesses the previously selected address.

### Proven relative address computation: `2FC9–2FDB`

The entry stubs select pointer P in the tested range **7 through 11**, and execution reaches `2FC9`:

1. `2FC9–2FCC` selects page `010` and reads RAM `015`.
2. `2FCD–2FD0` rearranges C and sets up the arithmetic based on **B0**, the high descriptor byte.
3. `2FD1–2FD5` performs **`12 - P` decrements** under the observed entry-state conditions.
4. `2FD6–2FDA` finishes address formation and transfers to the generic RAM helper.
5. `2958` performs the actual physical-address read.

For the tested entry states, the effective low-byte address is:

```text
n = 12 - P
effective_address_low_byte = (B0 - n) mod 256
```

The harness predicted **22 observed accesses** from the descriptor and P **without using the logged effective address as its input**. This is stronger evidence than a retrospective correlation. The high-nibble arithmetic also affects mode-restoration logic, so the formula alone does **not** specify all routine side effects.

For **P=11**, the formula yields `C0-1=0BF` in the initial allocation and `C5-1=0C4` after complex allocation. In the real-mode roll-down experiment, execution reads **unpopulated `0BF` at ROM `2958`** and receives zero, as reported for the physical HP-15C by [Eric Smith](https://www.hpmuseum.org/forum/thread-21336.html). The original HP-15C does **not** thereby acquire a physical RAM register at `0BF`.

### Forward relative address computation: `2BB3–2BC0`

Another descriptor-based path selects page `010`, reads `015`, rotates C, clears the high selector nibble, then increments an address while advancing pointer P. Under the stated entry conditions, its address expression is:

```text
effective_address = B0 + (12 - P)
```

This is a **ROM-derived expression**, but the Work package's independent 22-prediction test covered the **`2FC9` path**, not every possible `2BB3` entry state. Do not promote it to universal behavior without further tests.

## Outstanding questions

The report explicitly **does not** establish:

- A complete decoding of each byte, especially B5 and B6.
- The physical placement of nonzero matrix elements and all matrix metadata.
- The ordering of complex imaginary X/Y/Z/T/LAST X components.
- Validated program-byte encoding, program-memory allocation or deletion effects.
- Reliable SOLVE/INTEGRATE program execution, buffer lifetime and interactions with other allocations.
- Every possible writer to physical RAM `015`, or a general formal proof of `2AB0–2ABE`.
- Real-chip electrical measurement or complete emulator state fidelity.

The local Work experiments observed descriptor changes during attempted SOLVE/INTEGRATE cases but **did not verify functional mathematical success**. These scenarios must not be used to name descriptor fields definitively.

## Research next steps

**Highest priority:** derive the complete selector/carry/pointer semantics of **`2AA8–2ABE`** from the raw original microcode; then test matrix D/E and R2–R19 boundary cases. Keep program packing and SOLVE/INTEGRATE as separate, independently validated studies.

**Reproduction and licensing note:** the original 19-scenario Work archive includes adapted Nonpareil instruction code, tests and compressed traces, but is **not checked into this GitHub repository**. Its source files carry a mixture of GPL-2.0-only and GPL-3.0-only notices which require redistribution review. This page presents limited observations and independently checked ROM-address references, not a new combined license for that research harness.

[Complete RAM map]({{ '/hp15c/ram-register-map.html' | relative_url }}) · [Original ROM and rights]({{ '/microcode/rom-provenance.html' | relative_url }})
