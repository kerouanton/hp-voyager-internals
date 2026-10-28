---
title: HP-15C ROM Map
---

# HP-15C ROM Map

An address-oriented map of the **original 12,288-word HP-15C ROM**, with explicit evidence and limits. Coverage of the image is complete; identification of firmware functions is not. Unknown regions remain unknown rather than being assigned speculative routine names.

[Memory map]({{ '/hp15c/memory-map.html' | relative_url }}) · [Coverage index](#complete-rom-coverage-index) · [Entry-point map](#address-oriented-entry-point-map) · [Startup](#reset-startup-and-wake) · [Keyboard dispatch](#keycode-indexed-dispatch-table) · [Memory clear](#initialization-and-memory-clearing) · [Diagnostics](#diagnostics-and-self-test) · [Checksums](#rom-checksum-rule-and-results) · [Unknowns](#unresolved-subsystems) · [Sources](#sources-and-validation)

## Scope, notation and evidence

All addresses, raw words, register values, keycodes and constants are hexadecimal unless identified as decimal. ROM addresses count **10-bit words**, not bytes. CPU register fields and data-address notation follow the [memory map]({{ '/hp15c/memory-map.html' | relative_url }}#scope-and-evidence).

| Evidence class | Meaning |
|---|---|
| **Confirmed (configuration)** | Explicit model declarations: populated ROM regions, data memory and hardware keycodes. |
| **Confirmed (emulation)** | Explicit Nonpareil CPU or device behavior. This is source-level emulator evidence, not a claim of physical measurement or a completed end-to-end test run. |
| **Observed (ROM)** | Exact words and consequences derived from them under the cited execution semantics, including independently evaluated checksum arithmetic. |
| **Tentative** | Functional interpretation whose stronger meaning is not established by the available evidence. |

Descriptions are editorial, not recovered HP symbols. A range denotes the cited sequence being discussed, **not an asserted exhaustive subroutine boundary**. Caller/callee lists contain specific established edges, not a complete call graph. The same word may be used as data in one path and as an instruction in another.

**Do not equate a printed mnemonic with executed code.** The project disassembler's linear output is an inspection aid. Its every-address mode deliberately produces overlapping interpretations. A long branch or LDI consumes its next word; CXISA instead performs a separate ROM read cycle at an address formed from C[6:3]. SELPF has its own decoding state. Calls and returns depend on previous carry and the hardware return stack.

## Complete ROM coverage index

The 1,024-word bins below are navigation units, not function boundaries. All populated words fall in exactly one bin. **Confirmed (configuration)** for population; **Observed (ROM)** for image contents and listed anchors.

| Address interval | Populated words | Established anchors / remaining scope |
|---|---:|---|
| `0000–03FF` | 1,024 | Reset/wake gate; keycode-indexed words; startup key comparisons; diagnostic key sequence; signature check and initialization. Other code/data boundaries unresolved. |
| `0400–07FF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `0800–0BFF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `0C00–0FFF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `1000–13FF` | 1,024 | Signature write/sleep sequence at `13AC–13B6`; keyboard release/debounce helper at `13CF–13D8`. Remaining contents unresolved here. |
| `1400–17FF` | 1,024 | Keyboard/display diagnostic sequence, self-test, checksum loop, RAM/register tests and three-register shift service. Remaining contents unresolved here. |
| `1800–1FFF` | 0 | Unmapped gap; not part of either checksum span. |
| `2000–23FF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `2400–27FF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `2800–2BFF` | 1,024 | Wake gate `2810`, blink bitmap service `2845`, RAM read helpers `2957`/`295B`, clearing loop `2AE9`. Other contents unresolved here. |
| `2C00–2FFF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `3000–33FF` | 1,024 | Populated. No comprehensive functional segmentation established. |
| `3400–37FF` | 1,024 | Populated, including explicit zero words near the end. Zeros within the supplied image are not the same as unmapped addresses. |
| `3800–FFFF` | 0 | No ROM supplied or declared. |

Physical devices: `0000–17FF` belongs to R2D2 index 0, `1LE2-0321`; `2000–37FF` to index 1, `1LH1-0302`. Both are in bank 0. See [physical ROM map]({{ '/hp15c/memory-map.html' | relative_url }}#instruction-rom).

## Address-oriented entry-point map

Unless stated otherwise, confidence is **Observed (ROM)**: the operation described is supported, while any broader subsystem name remains unassigned. Links open the exact raw ROM records, including inline operand words.

| Address / sequence | Established description | Specific callers / successors | Register or memory effects | Evidence / confidence |
|---|---|---|---|---|
| [`0000`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L3) | Reset/wake carry gate: raw `1CF`, short branch to `0039` when previous carry = 1; otherwise continue at `0001` | `nut_reset()` and `powoff` both set PC to zero; wake carry depends on state | Tests previous carry, not a reset-vector pointer fetched from RAM | Confirmed (emulation) + Observed (ROM) |
| [`0001–000E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L4-L17) | Light-wake path begins by selecting data address zero, reading register 8, and testing it | Calls `2810` at `0006`, `0348` at `0008`; long transfer at `000D` to `0069` | Reads `008`, which the emulator forces to zero | Observed (ROM); detailed subsequent state branches remain conditional |
| [`0019–001F`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L28-L34) | Read and normalize keycode; prepare ON comparison | Calls at `003B`, `0048`, `0020` | C cleared, keycode read into C[4:3], rotated down; A.x ← keycode; C.x ← `018` | Observed (ROM) |
| [`0020–0028`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L35-L43) | Key-read service and ON comparison followed by display-off/transfer path | Calls `0019`, conditionally returns at `0023`; calls `2B1C` at `0024`; transfers to `13AA` at `0027` | Reads keycode; opcode `2E0` at `0026` disables display | Observed (ROM); not a complete boundary for all power handling |
| [`0039–0043`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L60-L70) | Deep-wake/ON qualification and retained-memory check | Calls `2810`, `0019`, `0348`, `13CF`; mismatch at `003E` branches to `006E` | Clears S0 before signature check | Observed (ROM) |
| [`0044–0068`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L71-L107) | Startup secondary-key comparisons and diagnostic selection | `14 → 0085 → 0351`; `17 → 005F → 0061 → 172E`; `12 → 16F3`; `15` sets S5 then enters `005F` | Clears S1, S2, S9, S5; plus-key path enables diagnostic repetition using S5 | Observed (ROM); key names confirmed by configuration |
| [`006E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L113) | Sleep instruction | Reached from `003E` on failed ON comparison | `powoff`: PC ← 0, bank selectors ← 0, request sleep | Confirmed (emulation) + Observed (ROM) |
| [`0085–0086`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L136-L137) | Direct transfer into initialization | Reached by equality branch at `005A`; destination `0351` | Bypasses the signature-match early return at `0350` | Observed (ROM) |
| [`0089–0090`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L140-L147) | Keycode-indexed ROM-word dispatch | Entry selected by `0069–006A`; destination obtained from ROM[keycode] | ROM read via CXISA; C[6:3] formed from word; push with `stk=c`, then `rtn`; display off | Observed (ROM); target is an entry stub, not necessarily the final user function |
| [`02DA–0301`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L733-L772) | 40-word expected-key sequence for diagnostics | Read by CXISA at `170D` after forming `02DA + A.x` | Literal hardware keycodes, including ENTER twice | Observed (ROM) |
| [`0348–0350`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L843-L851) | Retained-memory signature check | Called at `0008`, `0040`; match returns, mismatch falls through to `0351` | Compare RAM[007].x against `EAE`; selects RAM page zero | Observed (ROM) |
| [`0351–0374`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L852-L887) | Initialization sequence | Entry from `0348` fall-through or `0085`; calls `2AE9`, `295B`; ends with transfer to `1399` | Clears RAM through helper, sets G to `04`, writes setup values, clears selected status bits and N | Observed (ROM); full meaning of initialization fields unresolved |
| [`0375–037C`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L888-L895) | Diagnostic repeating-nibble pattern generator | Calls at `1736`, `179D`, `1769` | Fills C with `55555555555555`; if S4 is set, doubles to `AAAAAAAAAAAAAA` | Observed (ROM), under hexadecimal arithmetic |
| [`13AC–13B6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5039-L5049) | Signature write and sleep sequence | Reached through surrounding display/service paths; exhaustive caller list not established | Selects data page zero; writes ignored `008`; writes RAM[007].x = `EAE`; sleeps | Observed (ROM) |
| [`13CF–13D8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5074-L5083) | Wait for keyboard latch release and execute debounce delay | Called at `0042`, `005F`, `16FF`; `2812` calls inner entry `13D2` | `clr kb`/`? kb` loop; C.x countdown from `004` | Observed (ROM) + Confirmed (emulation) for keyboard FSM |
| [`16F3–172D`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5878-L5936) | Display-pattern and ordered-key diagnostic | Entered from `0055` for keycode `12`; calls `13CF`; reads table `02DA–0301`; reports through `178C` on mismatch/timeout | Writes bitmap registers 9/10 in selected page; compares latched codes; counts 40 positions | Observed (ROM); complete human-visible diagnostic procedure not independently validated |
| [`172E–173C`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5937-L5951) | Main diagnostic entry and first register/RAM-test checks | Called at `0061`; calls `2845`, `1790`, `0375`; failure to `1773` | Saves N in RAM[004]; clears S10 and S4 | Observed (ROM); “self-test entry” supported by startup key selection and subsequent tests |
| [`173D–174D`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5952-L5968) | Initialize ROM checksum bounds and accumulator | Follows successful diagnostic checks; continues at `174E` | A.x=`C00`, B.x=`C01`; select region by S10 | Observed (ROM) |
| [`174E–1757`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5969-L5978) | ROM checksum loop and comparison | Repeated CXISA at `174E`; failure branch `1757 → 1782` | Reads 6,144 words per selected region; compares final A.x to `C01` | Observed (ROM), independently evaluated on both regions |
| [`1758–1772`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5979-L6005) | Second RAM-test phase, aggregate comparison, and second-chip selection | Calls `1790` and `0375`; S10=0 sets S10 and branches to `1733`; S10=1 goes to `1775` | Sets S4; compares derived full-register values; tests M against pattern | Observed (ROM); full aggregate-test derivation remains incomplete |
| [`1773–178F`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6006-L6034) | Diagnostic failure/result handling and success branch | Failure from `173C`, `1768`, `176E`; checksum failure enters `1782`; report transfer at `178E → 1385` | Distinct result values described below; success restores N, writes full LCD bitmaps | Observed (ROM); no blanket assertion that all paths have passed on hardware |
| [`1790–17E7`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6035-L6122) | Register exchange and RAM test subroutine | Called at `1734`, `1759`; returns at `17E7` or reports through `178B` | Swaps/test-pattern operations; reads RAM, writes pattern, reads back, restores previous value | Observed (ROM); path depends on S4/S10/S11 |
| [`17E8–17F3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6123-L6134) | Three-register shift service | Direct call at `2B06` | RAM[002] ← RAM[001], RAM[001] ← RAM[000], RAM[000] ← incoming C; M temporary | Observed (ROM); do not infer all RPN roles from this alone |
| [`2810–2816`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8211-L8217) | Hex/P setup, keyboard-present gate, or sleep | Calls at `0006`, `0039`; calls `13D2`; returns on key flag | Sets hexadecimal mode, selects P; `? kb`, conditional return, otherwise sleep | Observed (ROM) |
| [`2845–285E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8264-L8289) | Fixed LCD bitmap and blink service | Called at `172E` | Selects data page zero; writes `009`, `00A`; enables blinking; preserves incoming C through B exchange | Observed (ROM); fixed bitmap meaning need not be guessed |
| [`2957–2959`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8538-L8540) | Select RAM address from C.x and read | Branch target of `295D` and other small read stubs | `sel ram`, `c=data`, return | Observed (ROM) |
| [`295B–295D`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8542-L8544) | Read fixed data address `010` | Called at `035C`; branches to `2957` | C.x ← `010`; selected address becomes `010`, C becomes its contents | Observed (ROM) |
| [`2AE9–2AF5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8940-L8952) | Descending low-byte zero-write loop | Called at `035A`; also follows surrounding code at `2AE7` | P ← 1, hex mode; clears B; decrements C.wp, selects address, writes zero; stops at A.wp | Observed (ROM); parameterized helper, not intrinsically “clear all RAM” |

## Reset, startup and wake

[Nonpareil reset](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L2100-L2136) sets PC = `0000`, carry = 1, normal instruction state, awake = true, P = Q = 0, active pointer P, and bank 0. It clears A/B/C/M/N and S bits. It does **not** perform the firmware's full RAM initialization; `nut_clear_memory()` is a separate emulator operation.

At `0000`, raw word `1CF` encodes a carry-set short branch with displacement `+39`, so the reset path begins `0000 → 0039`. A wake from display-enabled light sleep can enter with carry clear and continue through `0001`; `powoff` has already placed PC at zero. The LCD handler explicitly forces carry to 1 on display-disabled sleep. Sources: [CPU sleep instruction](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1698-L1719), [LCD sleep handler](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L243-L265).

The path at `0039` first calls `2810`. That helper can sleep again if there is no latched key. If it returns, `0019` obtains the keycode and `003D–003E` requires ON (`18`) before proceeding. A reset with no key is consequently not equivalent to an ON event.

The check at `0348` tests whether RAM[007].x equals `EAE`. The value is formed by hexadecimal nibblewise complement of `151` (`c=-c-1 x`), not by a ROM checksum calculation. Matching memory returns at `0350`; otherwise initialization begins at `0351`.

After the retained-memory check and the keyboard-release helper, the sequence at `0048` samples the latched code again. The following table describes the **ROM predicates**. It does not specify analog key-contact timing or prove a particular simultaneous-press sequence in a single-key emulator.

| Secondary latched code | Physical key | Established selection | Confidence |
|---|---|---|---|
| `075` | Decimal point | Equality branch at `004D → 0079`; bit-field adjustment path | Observed (ROM); user-visible setting interpretation not established here |
| `017` | Multiply | `0051 → 005F`; release helper; call `172E` at `0061` | Observed (ROM); supports ON + multiply self-test identification |
| `012` | Divide | Long transfer at `0055 → 16F3` | Observed (ROM); display/ordered-key diagnostic body supports identification |
| `014` | Minus | `005A → 0085 → 0351` | Observed (ROM); supports ON + minus memory-initialization identification |
| `015` | Plus | Equality after increment at `005B`; S5 set at `005E`, then diagnostic call | Observed (ROM); S5-controlled repeat loop at `0063–0064` |
| Other code | Other key / retained latch | Mismatch path at `005D → 0076` | Observed (ROM); subsequent function not assigned here |

## Initialization and memory clearing

The startup minus path transfers to `0351`, **after** the signature-match return. It therefore forces initialization instead of merely checking the signature.

### Clear-loop proof

At `0355–035B`, A.x receives `0C0`, C.x receives `01B`, and the code calls `2AE9`. The helper clears C[2], selects hexadecimal arithmetic, clears B, and sets P = 1. It compares A.wp and C.wp, decrementing the **two low nibbles** of C before each write.

| Property | Established result |
|---|---|
| Initial low-byte value | `1B` |
| First attempted write | `01A` |
| Descending low range | `01A, 019, …, 001, 000` |
| Wrap | `00 → FF`, because only C[1:0] is decremented; C[2] remains zero |
| Descending high range | `0FF, 0FE, …, 0C0` |
| Last attempted write | `0C0`, inclusive |
| Stop condition | C.wp equals A.wp = `C0` |
| Written value | Zero, supplied through B/C exchange around `data=c` |
| Total attempts | 91 decimal |
| Actual ordinary RAM covered | All 80 ordinary RAM registers |
| Other attempted addresses | Four bitmap-style registers, two ignored special registers, five unmapped positions |

This conclusion follows directly from [`2AE9–2AF5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8940-L8952), the [pointer/arithmetic semantics](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L418-L440), and the [physical data map]({{ '/hp15c/memory-map.html' | relative_url }}#complete-data-bus-map). It is not inferred from the printed label “clear RAM.”

Following the zeroing loop, `035C` calls `295B` to select/read `010`. `035E–0365` forms `C0D2D2D2D2D2D2` and writes `015`; `0368–036B` writes `00000000A00000` to `01A`. N is cleared at `036C–036D`. The sequence constructs further state and transfers at `0373` to `1399`. The detailed descriptor layout and every subsequent startup write remain unassigned.

**Important distinction:** the clear helper is parameterized. Its use elsewhere need not cover the same addresses. Only the demonstrated call at `035A`, with the stated bounds, establishes the full initialization sweep.

## Keycode-indexed dispatch table

The sequence [`0089–0090`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L140-L147) clears C, reads the key into C[4:3], then executes CXISA. Because C[6:5] are zero, the ROM read address is the keycode value. It then clears the m field, rotates C so that the fetched 10-bit word becomes a return address in C[6:3], pushes it, and returns to it.

Thus, **for this dispatch mechanism**, the word stored at ROM address `00kk` is itself the destination. It is not decoded as an instruction at the table address. Some of these same addresses are executable from other paths. Shift and mode processing can continue at the destination; the table does not identify the complete mathematical implementation of a key.

All rows are **Observed (ROM)**; key legends are **Confirmed (configuration)**. Destinations are entry stubs only. The table includes all 39 configured hardware codes, sorted numerically.

| Table address (= hardware code) | Physical key | Raw 10-bit word | Destination of this dispatch |
|---|---|---|---|
| [`0010`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L19) | SST | `1BC` | [`01BC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L447) |
| [`0011`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L20) | R/S | `1FB` | [`01FB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L510) |
| [`0012`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L21) | divide | `132` | [`0132`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L309) |
| [`0013`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L22) | sqrt | `179` | [`0179`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L380) |
| [`0014`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L23) | subtract | `140` | [`0140`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L323) |
| [`0015`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L24) | add | `113` | [`0113`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L278) |
| [`0017`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L26) | multiply | `135` | [`0135`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L312) |
| [`0018`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L27) | ON | `024` | [`0024`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L39) |
| [`0030`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L51) | GTO | `1B4` | [`01B4`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L439) |
| [`0031`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L52) | GSB | `1EF` | [`01EF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L498) |
| [`0032`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L53) | 9 | `0DF` | [`00DF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L226) |
| [`0033`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L54) | e^x | `178` | [`0178`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L379) |
| [`0034`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L55) | 3 | `123` | [`0123`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L294) |
| [`0035`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L56) | Sigma+ | `107` | [`0107`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L266) |
| [`0037`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L58) | 6 | `0EB` | [`00EB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L238) |
| [`0038`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L59) | f | `2C2` | [`02C2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L709) |
| [`0070`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L115) | SIN | `23B` | [`023B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L574) |
| [`0071`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L116) | Rdn | `202` | [`0202`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L517) |
| [`0072`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L117) | 8 | `0E0` | [`00E0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L227) |
| [`0073`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L118) | 10^x | `177` | [`0177`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L378) |
| [`0074`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L119) | 2 | `124` | [`0124`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L295) |
| [`0075`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L120) | decimal | `0D0` | [`00D0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L211) |
| [`0077`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L122) | 5 | `0EC` | [`00EC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L239) |
| [`0078`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L123) | g | `2C9` | [`02C9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L716) |
| [`0080`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L131) | TAN | `257` | [`0257`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L602) |
| [`0081`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L132) | backspace | `1CA` | [`01CA`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L461) |
| [`0082`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L133) | CHS | `206` | [`0206`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L521) |
| [`0083`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L134) | 1/x | `175` | [`0175`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L376) |
| [`0084`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L135) | ENTER | `21B` | [`021B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L542) |
| [`0087`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L138) | EEX | `212` | [`0212`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L533) |
| [`0088`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L139) | RCL | `2B8` | [`02B8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L699) |
| [`00C0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L195) | COS | `244` | [`0244`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L583) |
| [`00C1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L196) | x<>y | `229` | [`0229`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L556) |
| [`00C2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L197) | 7 | `0E1` | [`00E1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L228) |
| [`00C3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L198) | y^x | `176` | [`0176`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L377) |
| [`00C4`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L199) | 1 | `0FC` | [`00FC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L255) |
| [`00C5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L200) | 0 | `0FD` | [`00FD`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L256) |
| [`00C7`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L202) | 4 | `0ED` | [`00ED`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L240) |
| [`00C8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L203) | STO | `2AD` | [`02AD`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L688) |

Example: ROM[`0013`] contains `179`. Used as data by `008B`, this dispatches the square-root key to `0179`. A linear decoder starting at `0013` may instead combine `179` with the following word and print an unrelated long call. That interpretation does not describe this table read.

## Diagnostics and self-test

### Main diagnostic path

The multiply-code selection reaches `172E`. The body calls a fixed display/blink service (`2845`), saves N in RAM[004], and initializes status bits S10 and S4. It invokes `1790` and uses the pattern generator `0375`, then enters the checksum setup at `173D` if the checks pass.

S10 distinguishes the two chip/ROM passes: clear selects the lower ROM block; set selects the upper block. After the first pass's later tests, `1771` sets S10 and `1772` returns to `1733`. The second pass need not repeat the initial save/display sequence at `172E`.

The code at `1790–17E7` contains register exchanges, pattern generation and RAM operations that save a value, write a pattern, read it back and restore the original. The exact read/write loop is visible at [`17AC–17BC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6063-L6079). S4 selects pattern/test phase; S10 and S11 alter address/control handling. This is strong evidence for RAM/register diagnostics, but a complete derivation of every accumulated check and its hardware assumptions is **not** claimed here.

The ordinary and special data addresses must be modeled correctly: the bitmap registers mask six readback bits, while `008`/`018` ignore writes. The routine contains special handling for data address `019` and other transition values. Treating all installed addresses as conventional 56-bit RAM changes diagnostic results.

### Result construction and display paths

| ROM path | Established behavior | Limit |
|---|---|---|
| `1757 → 1782` | ROM checksum mismatch: clear C, increment at `1783` and `1784`; add two further increments if S10 is set | Produces C.x = `002` for lower pass or `004` for upper pass before rotation; this is a code-derived result value, not an independently photographed display. |
| `173C` / `1768` / `176E → 1773` | Other comparison failure: clear C, branch to `1784`; add two if S10 is set | Produces C.x = `001` or `003` before rotation. |
| `17A1 → 178B` | Early pattern mismatch | Clears C and joins `178C`; a separate failure entry. |
| `1789–178E` | Rotate result, select pointer 9, exchange with N, transfer to `1385` | Downstream formatting is not fully mapped here. |
| `1775–177E` | Success path restores N from RAM[004], writes all-one bitmaps to `009`/`00A`, checks keyboard, conditionally returns | Establishes a success path; does not certify that a complete diagnostic run has passed on physical hardware. |

**Validation boundary:** the ROM checksum arithmetic below was independently evaluated on the entire image. That is not the same as passing all register tests, RAM tests, display tests and keyboard tests. No full end-to-end hardware self-test certification is made by this map.

## ROM checksum rule and results

### Bounds and recurrence

Setup at [`173D–174D`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5952-L5968) establishes hexadecimal arithmetic inputs:

| CPU field | Lower pass (S10 = 0) | Upper pass (S10 = 1) |
|---|---|---|
| C[6:3], first ROM read | `0000` | `2000` |
| A.m, exclusive end address encoded in address field | `1800` | `3800` |
| A.x, accumulator seed | `C00` | `C00` |
| B.x, correction and expected result | `C01` | `C01` |
| Number of reads | 6,144 decimal | 6,144 decimal |

At [`174E–1754`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5969-L5975), CXISA reads a 10-bit word w into C.x. The loop adds w to the 12-bit A.x accumulator. If that addition overflows, it adds `C01` to the wrapped result. C.m is then incremented, and the loop continues until its address field equals A.m.

Precisely, for accumulator s and word w, let t = s + w. The next accumulator is `(t & FFF)` if t ≤ `FFF`, otherwise `((t & FFF) + C01) & FFF`. Here `&` is bitwise AND and all constants are hexadecimal. The correction is equivalent to subtracting `3FF` from the unwrapped sum at overflow; retain the actual 12-bit recurrence when reproducing the firmware.

At `1755`, the code subtracts B.x = `C01`; `1756–1757` require zero and branch to the failure path otherwise.

### Results on the pinned original image

| Region read, inclusive | Word count (decimal) | Final A.x before subtraction | After subtracting `C01` | Verdict for this check |
|---|---:|---|---|---|
| `0000–17FF` | 6,144 | `C01` | `000` | Pass |
| `2000–37FF` | 6,144 | `C01` | `000` | Pass |

Both results were calculated directly from the actual ROM records, independently of the printed disassembly. All words in each range participate, including data and explicit zero words. The unmapped gap `1800–1FFF` does not participate. The test does not require knowing which words are instructions.

No dedicated “checksum word address” is assigned here. The acceptance criterion applies to each entire region; a word near the end is not automatically a stored checksum merely because it influences the total.

## Diagnostic key sequence table

The code at [`1708–1719`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5899-L5916) forms a ROM-read address from `02DA + A.x`, obtains the expected code with CXISA, and compares it to the keyboard latch. Its bound is `028`, i.e. 40 decimal positions. The 40 words are at [`02DA–0301`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L733-L772).

This is an evidenced **data table**, despite the plausible mnemonics a linear disassembler can print for its words. It follows keyboard layout order, including ON/f/g, and includes ENTER a second time at sequence position 35. There are 39 configured physical keys, not 40 distinct hardware codes.

Sequence indexes are decimal. All code/data associations below are **Observed (ROM)** plus **Confirmed (configuration)** for key names.

| Sequence index | ROM address | Expected hardware code | Key |
|---:|---|---|---|
| 0 | [`02DA`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L733) | `13` | sqrt |
| 1 | [`02DB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L734) | `33` | e^x |
| 2 | [`02DC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L735) | `73` | 10^x |
| 3 | [`02DD`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L736) | `C3` | y^x |
| 4 | [`02DE`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L737) | `83` | 1/x |
| 5 | [`02DF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L738) | `82` | CHS |
| 6 | [`02E0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L739) | `C2` | 7 |
| 7 | [`02E1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L740) | `72` | 8 |
| 8 | [`02E2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L741) | `32` | 9 |
| 9 | [`02E3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L742) | `12` | divide |
| 10 | [`02E4`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L743) | `10` | SST |
| 11 | [`02E5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L744) | `30` | GTO |
| 12 | [`02E6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L745) | `70` | SIN |
| 13 | [`02E7`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L746) | `C0` | COS |
| 14 | [`02E8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L747) | `80` | TAN |
| 15 | [`02E9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L748) | `87` | EEX |
| 16 | [`02EA`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L749) | `C7` | 4 |
| 17 | [`02EB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L750) | `77` | 5 |
| 18 | [`02EC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L751) | `37` | 6 |
| 19 | [`02ED`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L752) | `17` | multiply |
| 20 | [`02EE`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L753) | `11` | R/S |
| 21 | [`02EF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L754) | `31` | GSB |
| 22 | [`02F0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L755) | `71` | Rdn |
| 23 | [`02F1`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L756) | `C1` | x<>y |
| 24 | [`02F2`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L757) | `81` | backspace |
| 25 | [`02F3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L758) | `84` | ENTER |
| 26 | [`02F4`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L759) | `C4` | 1 |
| 27 | [`02F5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L760) | `74` | 2 |
| 28 | [`02F6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L761) | `34` | 3 |
| 29 | [`02F7`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L762) | `14` | subtract |
| 30 | [`02F8`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L763) | `18` | ON |
| 31 | [`02F9`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L764) | `38` | f |
| 32 | [`02FA`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L765) | `78` | g |
| 33 | [`02FB`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L766) | `C8` | STO |
| 34 | [`02FC`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L767) | `88` | RCL |
| 35 | [`02FD`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L768) | `84` | ENTER (second ENTER position) |
| 36 | [`02FE`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L769) | `C5` | 0 |
| 37 | [`02FF`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L770) | `75` | decimal |
| 38 | [`0300`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L771) | `35` | Sigma+ |
| 39 | [`0301`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L772) | `15` | add |

The diagnostic also shifts/writes LCD patterns and has timeout/failure handling at `1724–172D`. A complete timed operator procedure, including release order, is outside the evidence established here.

## Unresolved subsystems

| Area | What is known here | What is not established |
|---|---|---|
| Mathematical functions | Physical-key entry stubs can be derived from the dispatch words | Complete routine ranges for square root, powers, trig, statistics, integration, SOLVE, matrices or complex arithmetic |
| Normal display conversion | Several entry/call sites and fixed bitmap writers are observed | Exhaustive formatting pipeline, packed digit tables and all annunciator meanings |
| Program execution / editing | ROM is populated and contains extensive code beyond mapped anchors | Verified program-byte encoding, interpreter dispatch, allocator and editing routine boundaries |
| User RAM layout | Physical map, signature, several initialization writes and an explicit shift helper | Complete RPN/user-register/complex-stack/matrix/program-memory assignments |
| Diagnostic aggregate tests | Branches, patterns, RAM read/write/restore sequences and result construction | Independent derivation of every register/RAM aggregate comparison and physical-hardware acceptance criteria |
| General call graph | Selected explicit edges listed above | Complete reachability, alternate entries, computed jumps, tail calls, data/code overlap and all callers |
| Remaining ROM words | Exact addresses and values preserved by the original dump | A proven executable/data classification for every word |

These are research boundaries, not claims that the functions are absent. Subsequent additions should supply state assumptions, exact raw words, target calculations and memory effects before assigning stronger names or larger ranges.

## Sources and validation

- [Nonpareil HP-15C configuration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl): chip identities, declared memory, LCD wiring, keyboard codes.
- [Original ROM](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom): the 12,288-word dump used here.
- [NUT CPU implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) and [state definitions](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h).
- [Voyager R2D2/LCD implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c).
- [NUT disassembler](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c); [digit arithmetic](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/digit_ops.c).
- [Published standalone disassembler](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/hp15c_disasm.c), [decoder tests](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/test_decoder.c), and [file-integrity tests](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/verify.cjs).

Upstream commit: `c347bc1ab20170c253512042f7aac0d952f304ea`. Project snapshot: `789ed0d540b022a97efe6afca369747b750e69b2`. Prepared 2026-10-28. The ROM file's SHA-256 is `dea4e861ad2aaa2bba49b5ea4e313a04f4fac9a4abf2516284ecc0f6f394568a`; its Git blob SHA-1 is `c276b93de235448e65b411412c579c1070cd8955`. These identify the original image, not the separate 192-register patch or later HP-15C products.

### Checks performed

| Check | Result / significance |
|---|---|
| Original ROM identity | Verified Git blob SHA-1 and SHA-256 shown above. |
| Address coverage | Exactly 12,288 unique 10-bit records in the two declared regions; no invented words in gaps. |
| Published C decoder | Compiled with C11 and strict warnings; the repository's 265,216 decoder cases passed. This does not establish a firmware control-flow graph. |
| Dispatch data | All 39 keycodes matched to actual ROM words; target construction checked against C rotation, CXISA, stack push and return semantics. |
| Diagnostic key data | All 40 records at `02DA–0301` matched against configured hardware codes; duplicate ENTER retained. |
| Clear-loop bounds | Derived from actual call operands, two-nibble decrement and inclusive stop test: 91 attempted addresses. |
| Checksum loops | Independent whole-image evaluation produced `C01` for both 6,144-word spans. |
| Full self-test | Not certified. Checksum agreement alone does not validate all diagnostic paths or physical hardware. |

The confidence labels intentionally separate source-defined hardware behavior from firmware interpretation. No routine name, region boundary, or caller relationship was promoted solely because a linear disassembly printed a plausible instruction.

[Back to top](#hp-15c-rom-map) · [Complete memory map]({{ '/hp15c/memory-map.html' | relative_url }})

## Document credits

Technical investigation and original draft: **ChatGPT Work** (2026-10-28), using Eric Smith's Nonpareil and the HP-15C ROM. Publication and additional editorial integration: **OpenAI ChatGPT in manual (agentless) mode**, under the project maintainer's direction. The original NUT emulation, disassembler and machine configuration are credited to **Eric Smith**. Analysis labels describe the evidence available, not independent measurement of physical hardware.
