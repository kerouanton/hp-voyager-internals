---
title: HP-15C RAM 015 — Dynamic Allocation Research
---

# HP-15C RAM `015`: Dynamic Memory Allocation

[Home]({{ '/index.html' | relative_url }}) · [RAM register map]({{ '/hp15c/ram-register-map.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [NUT status bits]({{ '/hp15c/nut-status-bits.html' | relative_url }})

**Status: partial; 19-scenario rerun on 28 October 2026, static allocator review on 7 November 2026.** This page records what can be demonstrated about physical RAM register `015` from the **original** HP-15C microcode and a locally rerun, instrumented Nonpareil V1 research harness. It is **not** a complete allocation specification and does not describe expanded-memory, LE, or CE ROM variants.

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
| **B5** | 3:2 | `D2` | **Observed (independent ROM execution, 7 Nov 2026):** D 1×1 changes B5–B6; the exact physical ownership of this region remains inferred |
| **B6** | 1:0 | `D2` | **Observed (independent ROM execution, 7 Nov 2026):** E 1×1 changes B6; physical relationship to program memory remains unresolved |

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

At **`2AA8` and `2AB9`**, the reported trace shows descriptor reads with RAM page `010` selected. Static decoding now establishes a two-nibble-at-a-time update loop; the caller-supplied state and functional interpretation of all boundaries remain conditional as detailed below.

Shared accesses to `015` also occur through generic address selection (`2958`) and at `2FCC`. A literal opcode search for `c=reg 5` alone misses indirect reads when `C=DATA` accesses the previously selected address.

### Admission check: `2A9C–2AA7` and physical RAM `016`

**Verified (ROM and pinned NUT instruction semantics), conditional on the selected RAM page being `010`:** `2A9C` (`c=reg 5`) reads `015`, `2A9F` copies it into A, and `2AA0` (`c=reg 6`) reads `016`. After the second read, `2AA3` adds B.x to A.x, `2AA4` exchanges A.x and C.x, and `2AA5–2AA6` conditionally branches to `2A79` if A.x is less than C.x. These operations provide an admission test **before** the `015` write at `2ABE`.

The conditional helper [`0BF2–0BF6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L3061-L3065) executes `c=0 xs`, `? c#0 x`, `?c rtn`, `c=c+1 xs`, `rtn`. It first clears C[2] and tests C.x. If the low byte is zero, `0BF5` increments **nibble 2 alone** (the `xs` field), and therefore **normalizes a zero low byte `00` to `100`**, not `001`. The helper is called after both the `015` and `016` reads. A previous informal correction claiming `001` was wrong; the original `100` interpretation has been verified against the opcode field bits and an actual ROM keyboard-path run.

For the investigated hex-mode path, define `normalize(v) = ((v & 0x0FF) == 0 ? 0x100 : (v & 0x0FF))`. With B.x as a three-nibble signed/modular correction, the admission comparison is:

```text
candidate = (normalize(B6) + B.x) mod 0x1000
limit = normalize(RAM[016])
if limit < candidate: branch to 2A79
otherwise: continue toward descriptor update
```

The low-byte normalization is exact for this helper; the expression assumes the selected RAM page is `010`, B6 is the low byte of the descriptor, the checked operands have the stated provenance, and arithmetic is hexadecimal. The alternate branch's full user-interface meaning is not yet known. **This is not a universal 8-bit capacity rule.**

**Synthetic boundary illustration (not a measured HP-15C default):** with B6=`D2`, RAM `016` low byte=`FF`, and B.x=`02D`, the candidate is `0FF` and the comparison permits the update. For B.x=`02E`, the candidate is `100` and it branches to `2A79`. If RAM `016` instead contains zero, normalization yields `100`: a correction of `02E` is admitted and `02F` is rejected in this synthetic boundary calculation. The `DIM D` keyboard-path experiment entered with RAM `016` equal to zero and successfully allocated one element. The meaning of the other fields of RAM `016`, particularly across program-memory changes, remains unknown.

### The two-nibble cumulative update loop: `2AA8–2ABE`

**Verified (ROM and pinned NUT semantics) for the described entry states:**

| ROM location | Decoded operation | Effect |
|---|---|---|
| `2AA8–2AAD` | `c=reg 5`; full and partial exchanges; clear C and C[2] | Preserve the descriptor and prepare a two-nibble correction |
| `2AAE–2AAF` | `pt=1`; conditional skip to `2AB6` | Set P=1; skip the first addition to initialize the counter |
| `2AB0–2AB1` | `ab ex w`; `a=a+c wp` | Add the correction through the active pointer |
| `2AB2–2AB4` | `rcr 12`; `inc pt` twice | Rotate the correction by two nibbles and advance P by two |
| `2AB5–2AB7` | `ab ex w`; `a=a+1 s`; conditional branch to `2AB0` | Increment the high-nibble loop counter; exit on overflow |
| `2AB8–2ABE` | Exchanges, `c=reg 5`, conditional `gosub 0BF2`, `reg=c 5` | Finalize and write the descriptor |

The branch at `2AAF` targets **`2AB6`**; the branch at `2AB7` targets **`2AB0`**. The words `2ABA–2ABB` form a **two-word conditional subroutine call** to `0BF2`, not two independent instructions. Pointer values 1, 3, 5, 7, 9, 11, 13 select successive bytes **B6, B5, B4, B3, B2, B1, B0** in least-significant-first order.

The counter is initialized by an increment at `2AB6` *before* the first field addition. If the high nibble A[13] initially equals `t` in `8` through `E`, hexadecimal carry termination produces **`N = 15 - t`** field updates. The `2A5C–2A5E` entry initializes M[0] to `8`; the other entry at `2A67` stores caller-supplied C in M. On the common exit path, `2A99` (`c=m`), `2A9A` (`rcr 1`), `2A9B` (`bc ex s`), and `2AA7` (`ab ex w`) transfer M[0] into A[13]. This establishes parameter transport, not the semantic meaning of every caller or matrix identifier.

| Synthetic selector `t` | Update count | Bytes affected |
|---|---:|---|
| `8` | 7 | B6–B0 (all fields) |
| `A` | 5 | B6–B2 (candidate for matrix A) |
| `B` | 4 | B6–B3 (candidate for matrix B) |
| `C` | 3 | B6–B4 (candidate for matrix C) |
| `D` | 2 | B6–B5 (confirmed in independent ROM execution for D 1×1 and 2×2) |
| `E` | 1 | B6 (confirmed in independent ROM execution for E 1×1 and 2×2) |

An independent arithmetic cross-check using **synthetic** inputs recovers the published matrix A/B/C and complex-mode descriptor transitions. A separate, newly built NUT interpreter has now executed real HP-15C keyboard sequences against the pinned original ROM; see the instrumented results below. These are **independent ROM-level executions**, not a replay of the full Nonpareil research harness, a native Nonpareil application session or a physical-hardware test.

These results establish the loop's conditional arithmetic mechanism. They do **not** exhaustively establish every ROM caller, actual RAM `016` values, error handling, the allocator's overall memory capacity, or program-storage behavior.

### Independent real-key executions: matrices D/E and program editing (7 November 2026)

**Observed (independent ROM interpretation).** A newly constructed CPU/keyboard/RAM interpreter ran the pinned unmodified original ROM from zero storage and CPU reset, pressed ON and backspace, then sent the actual HP-15C keyboard codes for the selected user operations. Each key was held for at most 2,000 CPU cycles and then released; the emulator ran to sleep after release. Every reported key path reached sleep without an unhandled opcode in the independent interpreter. The key layout was taken from [the pinned 15C model](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl); its NUT behavior was derived from the pinned processor source. This was **not** the previously built, locally rerun Nonpareil V1 harness; the independent interpreter and raw traces are not distributed with this documentation.

Matrix commands were entered as dimension, ENTER, dimension, `f DIM`, and the matrix selector. Initial RAM `015` was `C0D2D2D2D2D2D2` and RAM `016` was `00000000000000` before each scenario.

| Real key sequence | M[0] on `2A9C` entry | B.x on entry | RAM selector | RAM `015` after command | RAM `016` after |
|---|---|---|---|---|---|
| `1 ENTER 1 f DIM D` | `D` | `001` | `015` | `C0D2D2D2D2D3D3` | `00000000000000` |
| `1 ENTER 1 f DIM E` | `E` | `001` | `015` | `C0D2D2D2D2D2D3` | `00000000000000` |
| `2 ENTER 2 f DIM D` | `D` | `004` | `015` | `C0D2D2D2D2D6D6` | `00000000000000` |
| `2 ENTER 2 f DIM E` | `E` | `004` | `015` | `C0D2D2D2D2D2D6` | `00000000000000` |

On all four entry snapshots, the NUT arithmetic mode was **hexadecimal** and previous carry was **clear**. For D 1×1, the instrumentation recorded a write of the new descriptor at ROM `2ABE` after entering `2A9C` with the expected state; the other matrix final values were read at the sleep checkpoint. These experiments independently confirm the previously predicted D/E field-update pattern **for the stated dimension inputs**.

The same interpreter entered program mode via `g P/R`, entered one or two numeric key instructions, and left program mode via `g P/R`. RAM `015` stayed at its initial value throughout these cases:

| Program entry | RAM `016` after entry | ROM writer sites recorded |
|---|---|---|
| No user program instructions | `00000000000000` | Initialization sweep at `2AF1` |
| Enter key `1` in program mode | `000000000001FF` | `27B5`, `336B` |
| Enter keys `1`, then `2` in program mode | `000000000002FF` | `27B5`, `336B` |

**Combined program/matrix tests (same independent interpreter, separately initialized scenarios):**

| Key sequence after ON/backspace | RAM `015` after | RAM `016` after | Allocator entry observed |
|---|---|---|---|
| Program mode (`g P/R`), key `1`, exit; `1 ENTER 1 f DIM D` | `C0D2D2D2D2D3D3` | `000000000001FF` | M[0]=`D`, B.x=`001`, RAM `016`=`000000000001FF` |
| Program mode, keys `1`,`2`, exit; `2 ENTER 2 f DIM E` | `C0D2D2D2D2D2D6` | `000000000002FF` | M[0]=`E`, B.x=`004`, RAM `016`=`000000000002FF` |
| `1 ENTER 1 f DIM D`; program mode, key `1`, exit | `C0D2D2D2D2D3D3` | `000000000001FF` | D allocation before the program edit |

**Capacity-path probes (separately initialized):**

| Dimension command | B.x at `2A9C` | RAM `015` at sleep | Observed result |
|---|---|---|---|
| `6 ENTER 7 f DIM E` (42 elements) | `02A` | `C0D2D2D2D2D2FC` | Descriptor written at `2ABE` |
| `7 ENTER 7 f DIM E` (49 elements) | `031` | `C0D2D2D2D2D2D2` | No descriptor write at `2ABE` |
| One program instruction, then `6 ENTER 7 f DIM E` | `02A` | `C0D2D2D2D2D2FC` | Descriptor written; RAM `016`=`000000000001FF` |

These results support the ROM capacity comparison, and show that program edits can change RAM `016` while a matrix allocation still succeeds. They **do not determine the exact maximum admissible allocation**: 42 accepted and 49 rejected provide only a coarse bracket. The denial's exact user-visible display response was not independently validated.

**Interpretation and limits:** this is direct evidence that physical RAM `016` is updated by program editing, and that the entry-state value read by the allocator is not a fixed constant. The low-byte `FF` in the two program-entry cases is *consistent with* a changed allocation limit, and the preceding nibble changes with the number of entered instructions. **Neither a universal limit formula nor the encoding of program steps can be concluded from just two examples.** Multi-word program packing, program step insertion/deletion, exact memory-full boundary, user-visible errors, and comparison with the original Nonpareil harness remain open; basic mixed matrix/program allocation and one 42-element acceptance versus 49-element rejection have now been exercised.

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

- A complete **functional** decoding of each byte, especially B5 and B6; their update positions are better established than the ownership of the corresponding regions.
- The physical placement of nonzero matrix elements and all matrix metadata.
- The ordering of complex imaginary X/Y/Z/T/LAST X components.
- Validated program-byte encoding, program-memory allocation or deletion effects.
- Reliable SOLVE/INTEGRATE program execution, buffer lifetime and interactions with other allocations.
- Every possible writer to physical RAM `015`, and entry-state coverage sufficient to prove the allocator update rule for all call paths.
- Real-chip electrical measurement or complete emulator state fidelity.

The local Work experiments observed descriptor changes during attempted SOLVE/INTEGRATE cases but **did not verify functional mathematical success**. These scenarios must not be used to name descriptor fields definitively.

## Research next steps

**Highest priority:** independently reproduce these D/E and program-editing observations using the earlier, separately delivered Nonpareil V1 harness, then extend the matrix/program experiments to exact `0FF`/`100` admission boundaries with and without program steps, multi-word program packing, and program step insertion/deletion. Record RAM `015`, RAM `016`, M[0], B.x, selected RAM page, arithmetic mode and carry before `2A9C`; check descriptor writes and memory-full branches. The current independent NUT interpreter is a second implementation but has not been audited for full Nonpareil/physical-chip fidelity. Keep program packing and SOLVE/INTEGRATE as separate, independently validated studies.

**Reproduction and licensing note:** the original 19-scenario Work archive includes adapted Nonpareil instruction code, tests and compressed traces, but is **not checked into this GitHub repository**. Its source files carry a mixture of GPL-2.0-only and GPL-3.0-only notices which require redistribution review. This page presents limited observations and independently checked ROM-address references, not a new combined license for that research harness.

[Complete RAM map]({{ '/hp15c/ram-register-map.html' | relative_url }}) · [Original ROM and rights]({{ '/microcode/rom-provenance.html' | relative_url }})
