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

## The RAM-chip instruction word `2B0`

**Source:** [“Nut Instruction Set List Review & Comment”, March 2026, Eric Smith's 21 March reply](https://www.hpmuseum.org/forum/thread-24718.html), among a discussion of an evolving HP-41-oriented spreadsheet.

**Reported (first-hand, Eric Smith):** `2B0` (octal `1260`) is **not executed by the NUT CPU core**; it is a RAM-chip instruction that the CPU ignores. Whether it clears memory depends on the RAM device. Smith had **not tested** it on the Voyager `1LE2` or `1LH1` R2D2 devices.

**Confirmed (emulation):** Nonpareil's [NUT decoder](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c) names it `clr regs`, while the [CPU handler](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) is commented out. **Observed (ROM):** the pinned HP-15C image contains no `2B0` word. See the [instruction reference]({{ '/hp15c/instruction-set.html' | relative_url }}#an-instruction-word-can-target-a-memory-chip-2b0).

The evolving forum spreadsheet covers **multiple NUT calculator/peripheral environments**, especially the HP-41. An HP-41 opcode or peripheral behavior is **not automatically** a verified HP-15C R2D2 operation. We do not reproduce or redistribute the author's spreadsheet.

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
