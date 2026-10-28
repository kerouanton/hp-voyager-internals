---
title: HP-15C Community Research Notes
---

# HP-15C Community Research Notes

[Home]({{ '/index.html' | relative_url }}) · [Reference index]({{ '/hp15c/reference-index.html' | relative_url }}) · [Hardware architecture]({{ '/hp15c/hardware-architecture.html' | relative_url }}) · [Microcode provenance]({{ '/firmware/rom-provenance.html' | relative_url }})

**Purpose:** a short, topic-oriented index to technical discoveries scattered across the Museum of HP Calculators forum. It is **not** a forum mirror or an independent hardware test. Findings are paraphrased, attributed, dated, and restricted to the models discussed. Follow the original messages for context and later corrections.

**Evidence labels:** *Reported (first-hand)* means the named participant describes their own work or measurement, but we have not reproduced it; *Confirmed (configuration)* and *Confirmed (emulation)* refer to the [pinned Nonpareil source]({{ '/sources.html' | relative_url }}); *Observed (ROM)* means visible original HP-15C microcode words; *Tentative* means an interpretation still to test. A forum report alone is not an independent ROM trace or silicon measurement by this project.

## Hardware revisions: three-chip and two-chip HP-15C

**Source:** [“Nut 1LQ9 pinout?”, 13–15 May 2024](https://www.hpmuseum.org/forum/thread-21739.html), particularly Eric Smith's replies of **14 and 15 May** and the opening physical-board report by **hjalfi**.

- **Confirmed (configuration):** the [Nonpareil HP-15C definition](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl) models three separate devices: NUT `1LF5-0301` and R2D2 `1LE2-0321` and `1LH1-0302`.
- **Reported (first-hand, Eric Smith):** a later **two-chip HP-15C** uses `1LQ9-0325`, incorporating half the HP-15C ROM, alongside `1LH1-0302` for the other half. The `1LQ9-0325` is **not** an HP-12C part.
- **Reported (first-hand):** a 1994 HP-12C board carried `1LQ9-0322`. Smith explained that its external bus interface is disabled, possibly through mask programming or pin-strapping; he did not establish which. A board with a vacant 1LH1 footprint is not proof of a working expansion bus.
- **Reported (first-hand, Eric Smith):** he had only a **partial 1LF5 pinout**, based on earlier probing. In an apparent 44-position QFP with missing leads, only 27 pins were soldered. His numbering notes identify **1 = phase 1**, **2 = phase 2**, **37 = SYNC**, and **44 = ISA**, with the package oriented as described in his post. These are **not** verified `1LQ9` or `1LH1` pin assignments.
- **Unverified:** Smith had **not** verified whether the `1LH1` pinout matches `1LE2`; do not publish a combined Voyager pinout by assumption.

**Consequence:** the three-chip drawing is a **Nonpareil configuration diagram**, not a universal physical schematic for every original HP-15C revision. The logical ROM address map does not by itself identify the physical chip packaging of a particular specimen. See [hardware architecture]({{ '/hp15c/hardware-architecture.html' | relative_url }}).

## Reads from missing RAM during roll-up and roll-down

**Source:** [“Observation about the HP-15C roll up and roll down microcode”, Eric Smith, 21 February 2024](https://www.hpmuseum.org/forum/thread-21336.html); response by **J-F Garnier**, 22 February 2024.

**Reported (first-hand, Eric Smith):**

- On original HP-15C hardware, writes to nonexistent RAM addresses are ignored, and reads yield a **56-bit zero word**. Smith attributes the read behavior to NUT data-line precharging.
- The `R↓` microcode always reads the hypothetical **imaginary X** location, even when no complex stack is allocated. `R↑` calls the roll-down routine **three times**.
- The calculation uses **one address below the physical address of R02**. In initial real mode with no SOLVE/INTEGRATE buffer, R02 begins at `0C0`, so the read targets **`0BF`**, which is physically absent from the ordinary HP-15C data map.
- With a SOLVE/INTEGRATE buffer present but no complex stack, the same read can instead access the final buffer register. In non-complex mode, the value read is not used, so it does not affect the result.
- Nonpareil II initially flagged this as invalid memory access. To reproduce the real machine without suppressing diagnostics globally, Smith modeled **`0B8–0BF`** as read-zero/write-ignore (an eight-register unit of his emulator's memory mapping). **These are not eight installed RAM registers.**

**Implication for our map:** “unmapped” does **not** mean a hardware bus exception. This report describes electrically benign reads and ignored writes, not extra user memory. Our present map and any emulator check should keep physical RAM existence separate from behavior on unpopulated addresses.

**Related, not identical:** Smith also reports that fetching an all-zero instruction word at a nonexistent ROM subroutine target can cause a return when it is the first word after a call; this is contextual NUT behavior, not a universal statement that instruction `000` always returns. Independent trace and pin-level verification are outstanding.

## Dynamic allocation before user register R02

**Source:** the same [21 February 2024 microcode discussion](https://www.hpmuseum.org/forum/thread-21336.html).

**Reported (first-hand, Eric Smith):**

1. The user-data area starts at **`0C0`** in a normal HP-15C.
2. If SOLVE or INTEGRATE needs its temporary buffer, that buffer begins at **fixed address `0C0`**.
3. If the complex stack is allocated, the imaginary parts of **X, Y, Z, T, and LAST X** occupy registers above the optional SOLVE/INTEGRATE buffer and immediately before R02.
4. As a result, **R02 is at `0C0` only in the basic allocation state**. Its physical address moves upward when earlier regions are allocated.

This explains why our [RAM register map]({{ '/hp15c/ram-register-map.html' | relative_url }}) explicitly restricts its R02–R09 address table to an initialized real-mode test. The [ROM reads and writes of RAM `015`]({{ '/hp15c/ram-register-map.html' | relative_url }}#ram-015-address-descriptor-source) strongly suggest allocation-address metadata, **but this thread does not decode the individual fields of `015`**.

**Open verification:** reproduce `015` before/after complex-mode and SOLVE/INTEGRATE transitions, track the physical R02 address, and identify corresponding ROM instructions. These remain research questions, not completed tests.

## NUT instruction and Voyager LCD edge cases from Nonpareil I

**Source:** [“Nonpareil status”, Eric Smith's posts of 1–2 November 2022](https://www.hpmuseum.org/forum/thread-18940.html).

- **Reported (first-hand, emulator debugging), HP-12C:** a self-test problem led Smith to change Nonpareil's `C<>G` implementation for **digit pointer = 13**, under a condition involving whether the pointer had been set to 13 in the immediately preceding cycle. He cited the **David Assembler ROM description**. This warns that instruction edge cases can depend on prior CPU state; **it is not a separately verified HP-15C silicon erratum**.
- **Reported (first-hand emulator debugging), HP-12C LCD:** Smith questioned an earlier assumption that the low six unused bits of Voyager LCD bitmap registers read back as a complement of another bit. He observed spurious segments during the ON + divide self-test; returning **zero** for those low bits produced the expected display in his tests. He still wanted more physical hardware experiments, including register 8 behavior. **This is not proof of universal electrical readback for every Voyager revision.**

These discussions help interpret our [NUT instruction reference]({{ '/hp15c/instruction-set.html' | relative_url }}) and [LCD memory map]({{ '/hp15c/memory-map.html' | relative_url }}#lcd-segment-map) but should not be silently promoted from **HP-12C emulator debugging** to confirmed HP-15C hardware facts.

## The RAM-chip instruction word `2B0`

**Source:** [“Nut Instruction Set List Review & Comment”, March 2026, Eric Smith's 21 March reply](https://www.hpmuseum.org/forum/thread-24718.html), among a discussion of an evolving HP-41-oriented spreadsheet.

**Reported (first-hand, Eric Smith):** `2B0` (octal `1260`) is **not executed by the NUT CPU core**; it is a RAM-chip instruction that the CPU ignores. Whether it clears memory depends on the RAM device. Smith had **not tested** it on the Voyager `1LE2` or `1LH1` R2D2 devices.

**Confirmed (emulation):** Nonpareil's [NUT decoder](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c) names it `clr regs`, while the [CPU handler](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) is commented out. **Observed (ROM):** the pinned HP-15C image contains no `2B0` word. See the [instruction reference]({{ '/hp15c/instruction-set.html' | relative_url }}#an-instruction-word-can-target-a-memory-chip-2b0).

The evolving forum spreadsheet covers **multiple NUT calculator/peripheral environments**, especially the HP-41. An HP-41 opcode or peripheral behavior is **not automatically** a verified HP-15C R2D2 operation. We do not reproduce or redistribute the author's spreadsheet.

## Expanded-memory HP-15C: LU matrix limit and patch caveats

**Source:** [“HP-15C expanded memory firmware: problems with matrix larger than 8x8”](https://www.hpmuseum.org/forum/thread-19406.html), Eric Smith and J-F Garnier, **11–16 January 2023**; Giorgio and Smith, **3–8 July 2024**. The thread calls this *firmware*; this site generally uses *microcode* for native NUT ROM words.

**Reported (first-hand / attributed historical analysis), not a claim about added RAM in the stock machine:**

- Expanded-memory HP-15C microcode patches began around **2005**. Smith credits **Hrast Programmer** and **J-F Garnier** for additional corrections; Garnier shared a version offering **192 user-memory registers plus R0, R1, and I**, used in simulators and some later hardware platforms. **The stock HP-15C does not gain memory through a ROM edit alone**.
- Large matrices can be **stored and manipulated** in the expanded version; exceeding 8×8 is **not** a general matrix allocation limit. The problem concerns operations dependent on **LU decomposition**, notably **matrix division, inverse, or determinant** of square matrices with order **greater than 8**, which may yield wrong results or hang.
- Smith attributes the problem to **J. Fossy Weinzinger's analysis** (as quoted in the Swissmicros Voyager User Manual): the microcode encodes row interchanges using **three spare bits in matrix diagonal elements**. Three bits cannot represent the necessary larger permutation offsets. Smith cautioned that other order-9-and-above LU defects might also exist; he had **not** implemented a general LU fix.
- Smith proposed a **guard patch** to generate **Error 11** for affected square matrices larger than order 8, instead of running an unsafe operation. He announced a patch on **16 January 2023**. This **rejects** unsupported operations, **not** a correction that makes large-matrix LU valid. Its former `brouhaha.com` page is unavailable, so **no working patch download is claimed**.
- Garnier separately reported limitations in a 192-register version: at most **99 storage registers** allocated at a time and at most **99 matrix elements per row or column** (both Error 10); display problems with program steps past **999** and with the `MEM` readout. These are **version-specific reports**, not properties established for the stock 15C.

**Practical research consequence:** do not extrapolate the stock [physical RAM map]({{ '/hp15c/memory-map.html' | relative_url }}) or [RAM `015` allocator hypotheses]({{ '/hp15c/ram-register-map.html' | relative_url }}#ram-015-address-descriptor-source) to extended versions without tracking their differing memory configuration and ROM changes. The original NUT instruction stream, modern calculator variants, and third-party patches must be treated as distinct artifacts.

**Open work:** identify the LU pivot/row-interchange routine in the **original** image and confirm the three-bit storage mechanism with a trace. No such original-ROM address or independent LU trace is established by this forum account.

## Stock versus modified ROM provenance: a five-word fingerprint

**Source:** same [forum thread, Giorgio (3 and 6 July 2024) and Eric Smith (5 and 8 July 2024)](https://www.hpmuseum.org/forum/thread-19406.html).

Smith warned that earlier Nonpareil distributions (until approximately **2022**) contained **his modified HP-15C ROM**, **not** the unmodified original image now in the [Nonpareil GitHub repository](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom). He explicitly described his modified ROM as copyrighted, non-open-source, and not authorized for redistribution outside Nonpareil. **The fact that a modified calculator ROM is publicly obtainable does not grant redistribution rights.**

A useful identifying discrepancy was reported by Giorgio, who found the following words in HP-15C **Limited Edition** and **Collector Edition** ROMs. The original column below was **independently checked against the pinned Nonpareil original ROM**; the LE/CE column is **Giorgio's reported comparison**, not independently dumped by this project.

| Hex ROM address | Original HP-15C (Observed ROM) | LE/CE (reported by Giorgio) |
|---|---|---|
| `13F9` | `000` | `08B` |
| `13FA` | `000` | `0E5` |
| `13FB` | `000` | `2D3` |
| `13FC` | `000` | `2C6` |
| `13FD` | `000` | `0F5` |

Smith said his old changes included a shifted display `r` in the running annunciation, identification changes, and checksum compensation; in July 2024 he reported having since identified HP's own original checksum-adjustment words with **high confidence**. He did **not** identify those addresses in this thread. **Do not confuse this five-word comparison with proof that these are HP's checksum-adjustment locations**, nor assume that a zero ROM word is necessarily an unused location safe to overwrite.

**Research policy:** all absolute ROM addresses in this project refer to the **pinned original HP-15C image**, unless a variant is named explicitly. The [microcode ROM provenance page]({{ '/firmware/rom-provenance.html' | relative_url }}#original-and-modified-hp-15c-rom-images) records this caveat alongside the 2004 extraction story.

## Nonpareil I, Nonpareil II, and preservation

**Sources:** [“Nonpareil status”, Eric Smith, 2022–2025](https://www.hpmuseum.org/forum/thread-18940.html); [“Nonpareil II status and plans”, 2024–2025](https://www.hpmuseum.org/forum/thread-21213.html); [“16C microcode patch for expanded memory”, September 2026 follow-up](https://www.hpmuseum.org/forum/thread-23279.html).

- **Nonpareil I:** the [public, historical C/GTK2 repository](https://github.com/brouhaha/nonpareil) is our pinned and reproducible source for the HP-15C machine definition, instruction implementation, and microcode ROM. Smith announced the GitHub revival in **October 2022**.
- **Nonpareil II:** a separate C++/Qt6 rewrite, described by Smith as initially **closed source** with plans for preview and later paid releases. He reported a running **HP-25 in January 2024**, **HP-16C in February 2024**, and **HP-15C and HP-16C on Linux in May 2025**. A running development build is **not proof of a publicly released binary or source repository**.
- **Former personal website:** Smith reported on **27 September 2026** that his Web server was down after serious network disruption and that he suspected, but had not proved, a compromise. He intended to seek alternative hosting for calculator material; no replacement host or restoration date was confirmed in that post.
- **Historical HP-16C expanded-memory patch:** Smith described a patch using additional simulated RAM `020–0DF` and ROM `2000–2049`. This is **not installed memory on an original HP-15C or HP-16C**. The original standalone patch page was on the now-unavailable personal domain and is **not offered here as a working download link**.

**Project scope:** we are documenting the **real HP-15C and its microcode**, not rebuilding Nonpareil. Nonpareil II anecdotes can identify historical discoveries; V1 remains the code base against which readers can reproduce our source-oriented analyses.

## How to cite a new forum discovery

Record the **thread URL, direct post permalink when available, author, post date, model/chip revision, precise technical claim, evidence class, and remaining uncertainty**. Recheck later posts for corrections. Prefer short paraphrases over copied posts; do not treat attachments, private hardware notes, vanished pages, or emulator patches as freely redistributable.

[Sources and evidence policy]({{ '/sources.html' | relative_url }}) · [Microcode extraction and provenance]({{ '/firmware/rom-provenance.html' | relative_url }})
