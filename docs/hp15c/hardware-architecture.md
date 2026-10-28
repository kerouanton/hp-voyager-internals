---
title: HP-15C Hardware Architecture — NUT and R2D2
---

# HP-15C Hardware Architecture: NUT and R2D2

[Home]({{ '/index.html' | relative_url }}) · [Reference index]({{ '/hp15c/reference-index.html' | relative_url }}) · [Memory map]({{ '/hp15c/memory-map.html' | relative_url }}) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [NUT instruction set]({{ '/hp15c/instruction-set.html' | relative_url }})

**The essential distinction: NUT executes instructions; R2D2 supplies the firmware and data memory and, in the main device, drives the LCD.**

The original **HP-15C** belongs to HP's Voyager family of compact calculators. Unlike a contemporary desktop microcomputer, its calculator-specific logic, firmware storage, data registers and display interface are combined into a very small chipset.

This page explains **the modeled original HP-15C architecture**, not later ARM-based HP-15C reissues or a measured electrical schematic. Its primary source is Eric Smith's [Nonpareil HP-15C model](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl). In the tables, **Confirmed (configuration)** means declared by that model; **Confirmed (emulation)** describes Nonpareil's implementation; and **Observed (ROM)** means supported by actual firmware words or explicitly scoped execution experiments. The exact chip markings can vary with hardware revision.

## The whole machine in one picture

```text
         HP-15C KEYBOARD
        39 physical keys
                |
                | key events / hardware codes
                v
   +--------------------------------+
   | NUT CPU                        |
   | HP 1LF5-0301 (model)           |
   |                                |
   | Executes 10-bit instructions   |
   | A B C M N / G / P Q / S / PC  |
   | Carry / four-level call stack |
   | Keyboard and power state      |
   +--------------------------------+
               |   ^
        ROM fetch / data accesses
               v   |
      +-----------------------------+
      | R2D2 #0                     |
      | HP 1LE2-0321                |
      | ROM 0000-17FF (6,144 words)|
      | 40 ordinary RAM registers  |
      | LCD control + bitmap state |
      +-----------------------------+
               |
               +--------------------> SEGMENTED LCD
                                      11 positions,
                                      100 connected bits

      +-----------------------------+
      | R2D2 #1                     |
      | HP 1LH1-0302                |
      | ROM 2000-37FF (6,144 words)|
      | 40 ordinary RAM registers  |
      | Additional I/O addresses   |
      | No second connected LCD    |
      +-----------------------------+
                ^
                |
     NUT ROM fetches and data accesses
```

**This is a functional diagram, not a pinout or a claim about separate physical parallel buses.** Both R2D2 devices are accessed by the CPU; the drawing emphasizes their *roles*, not measured signal routing. The precise electrical bus pins, waveform timings, clock distribution and power supply circuitry are outside the evidence established by the emulator model.

## Three chips, three different responsibilities

| Modeled chip | HP part number | Principal role | Declared memory |
|---|---|---|---|
| NUT CPU | `1LF5-0301` | Executes firmware, contains working registers and keyboard/control state | CPU-internal state, **not** the 80 external RAM registers |
| R2D2, index 0 | `1LE2-0321` | Firmware ROM, data RAM and LCD driver | 6,144 ROM words + 40 ordinary RAM registers |
| R2D2, index 1 | `1LH1-0302` | Additional firmware ROM and data RAM | 6,144 ROM words + 40 ordinary RAM registers |

The distinction is **functional, not a software hierarchy**: the R2D2 is not a second CPU that executes HP-15C instructions. It is a combined memory/interface chip. Conversely, the NUT has working registers but does not contain the entire mapped HP-15C ROM and RAM.

The CPU and both R2D2 part numbers appear explicitly in the [machine description](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl). Different Voyager models and historical board revisions may use different part markings, so these are **the declared parts for the documented configuration**, not a universal claim about every surviving HP-15C.

## NUT: the processor

NUT is HP's proprietary calculator CPU architecture. It fetches **10-bit ROM words**, decodes their instruction category, modifies CPU state, and initiates memory or keyboard/display operations when instructed.

### The CPU's own registers

| CPU state | Size in Nonpareil | What it is used for |
|---|---|---|
| `A`, `B`, `C`, `M`, `N` | Five registers, each 14 × 4-bit nibbles = 56 bits | Arithmetic, working values, addresses and temporary state |
| `G` | 2 nibbles = 8 bits | Auxiliary nibble storage |
| `P`, `Q` and pointer selection | Two 4-bit digit pointers | Selects which nibbles of wide registers an instruction affects |
| `S[0..13]` | 14 status flags | Firmware-visible conditions; distinct from user-allocated flags in RAM |
| `FO` / `F` | 8 bits | Flag output / status transfers in the generic CPU model |
| `PC` | 16 bits | Address of the next 10-bit instruction word |
| Hardware return stack | Four 16-bit addresses | Saves subroutine return addresses; **not** the RPN numeric stack |
| `carry` and `prev_carry` | One bit each in the model | Arithmetic/test results and subsequent conditional control flow |
| `ram_addr`, `pf_addr` | 10-bit RAM selector, 8-bit peripheral selector | Select addressable external resources |
| Keyboard latch and sleep state | Model-specific state fields | Key detection, release/debounce and power handling |

The five wide CPU registers are **not five user storage registers**. Their role varies dynamically. In the tested initialized real-number state, the [RAM-register investigation]({{ '/hp15c/ram-register-map.html' | relative_url }}#lower-rpn-stack-and-cpu-n) found `N` holding RPN X at idle, but the firmware can reuse N during diagnostics and other operations. Likewise, `C` is an important communication register for memory accesses; it does not permanently mean the RPN C or X value.

See [NUT CPU Architecture]({{ '/hp15c/nut-cpu.html' | relative_url }}) and [NUT Instruction Set]({{ '/hp15c/instruction-set.html' | relative_url }}) for registers, nibble fields, control flow and instruction encoding.

### Why the CPU operates on nibbles

A 56-bit CPU register is represented as **14 hexadecimal digits (nibbles)**. Arithmetic instructions choose a field: the whole register, its low three nibbles (`x`), the exponent-sign nibble (`xs`), the mantissa-like field (`m`), and so on.

This is well suited to calculator arithmetic and packed decimal data. However, the instruction mnemonic `x` refers to **CPU nibbles 0–2**, not automatically to the user-visible **RPN X** register.

A NUT instruction word occupies **10 bits**, not eight. Most instructions use one ROM word; the absolute branches/calls and `LDI` use two. The ROM's word address is therefore **not a byte offset**.

### The clock

The Nonpareil HP-15C configuration declares an **LC oscillator at 215,000 Hz** (`<inst_clock osc_type="LC" freq="215000"/>`). That is a model timing parameter; it must **not** be read as a demonstrated throughput of 215,000 completed firmware instructions per second, nor as a physical frequency measurement on every board revision.

## R2D2: the memory and display companion

The name **R2D2** stands for the combined **ROM/RAM/Display Driver** function. Its role in the Voyager architecture is to supply resources the NUT accesses.

### What each HP-15C R2D2 contributes

| Feature | R2D2 #0 | R2D2 #1 |
|---|---|---|
| ROM | `0000–17FF` | `2000–37FF` |
| ROM capacity | 6,144 × 10-bit words | 6,144 × 10-bit words |
| Ordinary data RAM | `000–007`, `0E0–0FF` | `010–017`, `0C0–0DF` |
| Ordinary RAM capacity | 8 + 32 = 40 registers | 8 + 32 = 40 registers |
| Extra I/O addresses installed in Nonpareil | `008–00A` | `018–01A` |
| LCD interface | Active: bitmap registers `009`/`00A` drive the display | No second LCD connected in this HP-15C model |

**Total:** 12,288 firmware ROM words and 80 ordinary 56-bit data registers. In bit capacities, that is **122,880 ROM bits** plus **4,480 ordinary RAM bits**; these numbers are storage capacities, not byte-addressed ranges.

The auxiliary R2D2 is needed because the HP-15C's firmware and memory requirements exceed the resources supplied by its main device. The emulator explicitly installs a real display model only for index 0; the second device adds memory.

Sources: [HP-15C hardware configuration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl) and [`voyager_r2d2_install()`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L322-L361).

## ROM: where the HP firmware lives

The HP-15C ROM contains the **permanent firmware**: initialization, keyboard dispatch, mathematical routines, numeric entry, RPN stack operations, program interpretation, diagnostics and other services. Its 10-bit words are fetched and executed by NUT.

```text
NUT ROM addresses (instruction words, hexadecimal)

0000  +------------------------+
      | R2D2 #0 ROM            |  6,144 words
17FF  +------------------------+
1800  | Unmapped gap           |  2,048 addresses
1FFF  +------------------------+
2000  | R2D2 #1 ROM            |  6,144 words
37FF  +------------------------+
3800  | No ROM declared        |
FFFF  +------------------------+
```

The `PC` is 16 bits, but that does **not** mean 65,536 populated instruction words. The HP-15C model defines only the two ROM areas above, both in bank 0. The gaps must not be treated as hidden firmware merely because the generic NUT emulator has addressing machinery for them.

### Fetching instructions versus reading ROM as data

Most of the time, NUT fetches a ROM word at `PC` and decodes it. The instruction **`CXISA`** is different: the firmware constructs a ROM address in `C[6:3]` and reads the ROM word at that address into `C[2:0]`.

That mechanism lets code read **tables and constants stored inside the firmware ROM**, including the key-dispatch table and checksums. As a result, **a word that looks like an opcode in a disassembler may in fact be table data**. ROM and RAM are separate logical spaces; `CXISA` is not a RAM read.

[Browse ROM Map]({{ '/hp15c/rom-map.html' | relative_url }}) · [NUT opcodes]({{ '/hp15c/instruction-set.html' | relative_url }})

## RAM: where working data and user programs live

A NUT **data RAM address** selects an entire **14-nibble (56-bit) register**. It is **not** a location in a conventional byte array.

```text
NUT data register addresses (hexadecimal; 000–3FF)

000–007  R2D2 #0 ordinary RAM    8 registers
008–00A  R2D2 #0 special I/O    3 addresses
00B–00F  unmapped
010–017  R2D2 #1 ordinary RAM    8 registers
018–01A  R2D2 #1 special I/O    3 addresses
01B–0BF  unmapped
0C0–0DF  R2D2 #1 ordinary RAM   32 registers
0E0–0FF  R2D2 #0 ordinary RAM   32 registers
100–3FF  unmapped
```

**Note:** the emulator installs six special I/O addresses in addition to the 80 ordinary RAM registers. That does **not** prove six further full physical RAM registers on the original chips. In particular `008` and `018` read as zero and ignore writes; `009`/`00A` are the active display bitmap registers; `019`/`01A` use similar read masking but are not connected to a second LCD in the model.

### How NUT selects a RAM register

The ROM runs instructions such as:

| Opcode | NUT mnemonic | Meaning |
|---|---|---|
| `270` | `sel ram` | Select the data address encoded by C[2:0] |
| `038` | `c=data` | Read the selected data register into C |
| `2F0` | `data=c` | Write C to the selected data register |
| `028 + 040 × n` | `reg=c n` | Replace the low address nibble with `n`, then write C |
| `038 + 040 × n` | `c=reg n` | For `n > 0`, replace low address nibble then read |

For example, selecting RAM address `010` and then executing `reg=c 5` addresses **`015`**, not `005`. The upper address bits are inherited from the **current selector**. This is why a firmware RAM map requires **execution-state tracking**: seeing an instruction that says `reg=c 5` is not enough to know the physical destination.

The `sel ram` selector has **10 bits** and can represent `000–3FF`; many of those addresses simply have no installed memory in the original HP-15C model. The peripheral-address selector `pf_addr` is separate and should not be mistaken for another physical HP-15C RAM bank.

### How the firmware uses the 80 data registers

The ROM assigns *software meanings* to physical RAM locations. These meanings are **not hard-wired by the R2D2 chip**. For example, the current [trace-based RAM register reference]({{ '/hp15c/ram-register-map.html' | relative_url }}) supports the following **initial real-number state** mapping:

| Physical data address | Observed software use |
|---|---|
| `000` | RPN Y |
| `001` | RPN Z |
| `002` | RPN T |
| CPU register `N` | RPN X **at the tested idle checkpoints**, not a fixed permanent assignment |
| `010`, `011` | User registers R0, R1 |
| `0C0–0C7` | User registers R2–R9 |
| `007` low three nibbles | Retained-memory signature `EAE` |
| `015` | Source of a packed address/allocation descriptor; precise layout still under investigation |

These findings have **limited experimental scope**. They do not establish the complete real/complex RPN stack, LAST X, matrix storage or program-memory relocation. They come from ChatGPT Work's instrumented Nonpareil-derived experiments, whose source-level evidence is cited; its full harness and raw logs were **not** included for independent replay.

### Firmware versus a program typed by the user

These are different kinds of “program”:

- **HP firmware:** permanently stored in R2D2 **ROM**, executed as NUT machine instructions.
- **User keystroke program:** stored in the calculator's writable **RAM**, represented in a format understood by the firmware.
- **Execution of a user program:** the NUT continues executing **firmware in ROM**, whose interpreter reads and acts on the user program stored in RAM.

A keystroke program is therefore **not necessarily a stream of raw 10-bit NUT opcodes** that the NUT fetches directly from program RAM. This distinction is essential when studying the HP-15C's user-program interpreter.

## Keyboard: CPU input, not a RAM-mapped matrix

The original model specifies **39 physical keys** and their hardware codes. Nonpareil delivers a pressed key into the NUT keyboard latch and implements key-down, release and debounce states.

| Key | Hardware code |
|---|---|
| ON | `18` |
| × | `17` |
| ÷ | `12` |
| − | `14` |
| + | `15` |
| ENTER | `84` |
| STO | `C8` |
| RCL | `88` |

Two important instructions are **`c=keys`** (`220`), which reads the latched hardware code, and **`goto keys`** (`230`), which changes the program counter using that code. The firmware also reads a **keycode-indexed dispatch table in ROM** via `CXISA`. This is a software/firmware lookup, not a keyboard table burned into a CPU register.

The physical `f` and `g` keys have their own hardware codes. Shifted functions do **not** require separate electrical codes for all the labels printed on the keyboard; the firmware tracks the shift state and interprets the next key accordingly.

The **keyboard is not mapped into `000–3FF` RAM** in this Nonpareil HP-15C model. Its event/latch model should not be mistaken for a verified electrical keyboard-matrix schematic or a proof that simultaneous key-press timing is modeled exactly.

[Complete 39-key code table]({{ '/hp15c/memory-map.html' | relative_url }}#complete-hardware-keycode-table) · [ROM keyboard dispatch]({{ '/hp15c/rom-map.html' | relative_url }}#keycode-indexed-dispatch-table)

## LCD: display data lives in R2D2 #0

The main R2D2 combines data-memory access with display control. Its two active **bitmap registers** are `009` and `00A`; their stored bits supply the LCD segment states.

The machine description declares **11 display positions × 10 segment slots = 110 slots**. Of these, **100 are connected to bitmap bits** and **10 are absent**. The first position is primarily the sign position. The segment slots include seven numeric segments, decimal point, comma tail and an annunciator channel.

Each active bitmap register supplies the **50 bits numbered 6–55**; bits 0–5 do not represent connected display segments. In the emulator, reading the bitmap registers through the CPU masks those low six bits, so a raw inspection of their backing storage need not match the CPU-visible read result.

A concrete example from the model's LCD map:

```text
Data register 009, bit 46  --->  position 0, segment g (sign)
```

The **display-on/off/blink state** is controlled by NUT operations such as `disp off` (`2E0`), `disp toggle` (`320`) and `disp blink` (`030`), handled by the Voyager display driver. These operations do **not** introduce a separate LCD-address region in the 10-bit data RAM space.

**R2D2 #1 is not a second independent display driver in this model.** Although its installation creates three special addresses, the emulator explicitly installs the LCD controller only for index 0.

[Full LCD segment/bit table]({{ '/hp15c/memory-map.html' | relative_url }}#lcd-segment-map) · [Voyager LCD implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c).

## Power, sleep and continuous memory

The **“C”** in HP-15C refers to the calculator's *continuous memory*: user data and programs can be retained across ordinary OFF/ON use. It is important to distinguish **sleep**, **CPU reset**, **firmware initialization**, and **complete loss of retained state**.

At the instruction level, NUT has a `powoff` operation (`060`). In the emulator, it places the CPU into sleep and arranges for execution to return to the startup/wake path; it does **not** automatically zero every R2D2 RAM register. Keyboard and LCD state participate in the emulator's wake handling.

The firmware uses a value in **RAM register `007`**, low three nibbles `EAE`, as part of a retained-state validity check. On wake, the code may preserve the existing memory or enter an initialization sequence according to its control flow.

This explains why the physical resources must be understood together: **the CPU can stop executing, the ROM remains available, and the RAM may still hold the user's state**. The exact analog supply circuitry and voltage thresholds are not established by the Nonpareil model; its sleep behavior is a functional emulation, not a measurement of battery drain or power rails.

[Reset and startup analysis]({{ '/hp15c/rom-map.html' | relative_url }}#reset-startup-and-wake) · [Retention signature in RAM 007]({{ '/hp15c/ram-register-map.html' | relative_url }}#ram-007-retention-signature-and-other-fields)

## A keystroke, from hardware to result

Consider the sequence **`7 ENTER 2 +`** in normal real-number mode. The following is an **architectural explanation**, not a cycle-accurate execution trace:

1. **Key input:** a key press is delivered to the NUT keyboard interface as a hardware code.
2. **Firmware dispatch:** the NUT executes ROM code that reads and interprets the code, with possible dispatch through a ROM table.
3. **Number entry:** the firmware updates its working state and the RPN stack. Some state resides in NUT registers, some in R2D2 RAM.
4. **Arithmetic:** the NUT executes sequences of low-level 10-bit arithmetic and control instructions. No R2D2 independently performs the calculation.
5. **Result management:** the firmware arranges the numeric result and the RPN stack state.
6. **Display update:** the NUT writes the appropriate bitmap values into the main R2D2's display registers; the LCD controller displays the result.

If you now enter `STO 2` in the **initial allocation state**, Work's instrumented experiment found user register **R2** at physical RAM address **`0C0`**. That address mapping is firmware-defined and can depend on allocation state; it is not a hard-coded physical “R2 pin” on the chip.

### Example of reset and memory clear

Another useful distinction:

- **CPU reset:** the emulator resets the NUT's internal execution state (including the PC); it does not, by itself, mean the 80 R2D2 RAM registers were zeroed.
- **Firmware initialization:** the ROM can inspect a retained-memory marker and run an initialization sequence if needed.
- **ON + minus:** the identified path at ROM `0351` calls a helper at `2AE9`; under the proven bounds this **attempts 91 address writes** and covers **all 80 ordinary RAM registers**, plus six special addresses and five unmapped positions, then performs nonzero setup writes.

The result of ON + minus is thus **an initialized memory state**, not just a blanket assertion that every address remains zero. See [Initialization and memory clearing]({{ '/hp15c/rom-map.html' | relative_url }}#initialization-and-memory-clearing).

## Common misunderstandings

| Question | Correct interpretation |
|---|---|
| Is R2D2 another processor? | No: in this architecture it is combined ROM/RAM/display-interface hardware, accessed by NUT. |
| Does the NUT contain no memory? | It contains *working registers and control state*, but the mapped HP-15C firmware ROM and ordinary RAM are provided by R2D2 devices. |
| Are there 80 freely usable user registers? | No: 80 is the **physical ordinary data-register count**; the firmware also needs workspace, descriptors, stack and program/matrix storage. |
| Are ROM and RAM contiguous like a 6502's 64 KB map? | No: NUT uses separate **instruction-word** and **data-register** address spaces. |
| Is RPN X the same as arithmetic field `x`? | No: one is a user-level stack value; the other specifies CPU nibbles 0–2. |
| Is the four-address CPU return stack the RPN X/Y/Z/T stack? | No: return addresses and numeric data have different purposes and storage. |
| Is `0x009` ordinary user RAM? | No: it is a display bitmap I/O register with special read behavior in the emulated hardware. |
| Does a disassembled ROM word always represent an instruction executed by NUT? | No: ROM tables and literals may occupy addresses that also decode to plausible opcode strings. |
| Does the CPU's 16-bit PC imply a 64K-word HP-15C ROM? | No: only 12,288 addresses are populated in the documented configuration. |

## Physical hardware versus Nonpareil

This reference is deliberately explicit about its evidence boundaries:

- **Machine description:** establishes the declared three-chip configuration, ROM and RAM regions, LCD segment wiring and hardware keycodes.
- **Emulator implementation:** describes instruction behavior, keyboard latch, special RAM register handlers and LCD update logic.
- **ROM analysis:** explains code/data effects inferred from specific firmware words and tested execution states.
- **Physical board:** the exact part markings, electrical buses, pin counts, board routing, power retention and LCD waveforms may differ by revision and require independent hardware documents or measurement.

For additional historical perspective and variant descriptions, see [community HP-15C hardware notes](https://finseth.com/hpdata/hp15c.php) and the [HP Voyager architecture discussion](https://marc.info/?l=classiccmp&m=124622156101308&w=2). These secondary accounts describe real hardware variants but are **not substitutes for a schematic of a particular board**.

## Primary sources and attribution

- **Eric Smith**, [Nonpareil source repository](https://github.com/brouhaha/nonpareil), especially [HP-15C model and chip declarations](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl), [NUT CPU state](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h), [NUT execution implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) and [Voyager R2D2/LCD implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c).
- [Original HP-15C ROM](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom) (source of firmware descriptions; not redistributed here).
- Project references: [Complete Memory Map]({{ '/hp15c/memory-map.html' | relative_url }}), [RAM Register Uses]({{ '/hp15c/ram-register-map.html' | relative_url }}), [ROM Map]({{ '/hp15c/rom-map.html' | relative_url }}) and [NUT Instruction Set]({{ '/hp15c/instruction-set.html' | relative_url }}).

**Technical foundation:** Eric Smith / Nonpareil. **Architecture synthesis, explanatory writing, and GitHub publication:** OpenAI ChatGPT, interactive manual (*agentless*) mode, with the project maintainer directing and approving the work. The experimental RAM register assignments referenced here were developed separately by **ChatGPT Work** and are qualified by their stated trace limitations.

[Back to the reference index]({{ '/hp15c/reference-index.html' | relative_url }})
