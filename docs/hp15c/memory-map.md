---
title: HP-15C Complete Memory Map
---

# HP-15C Complete Memory Map

[Address and topic index]({{ '/hp15c/reference-index.html' | relative_url }}) · [Home]({{ '/index.html' | relative_url }})

An address-oriented reference for the **original HP-15C**, based on its ROM and Eric Smith's Nonpareil implementation. Hardware resources, CPU state, and firmware allocations are documented separately.

[ROM map]({{ '/hp15c/rom-map.html' | relative_url }}) · [Address spaces](#address-spaces) · [ROM](#instruction-rom) · [RAM and I/O](#data-ram-and-r2d2-io) · [LCD](#lcd-segment-map) · [Keyboard](#keyboard-interface) · [CPU registers](#cpu-internal-registers) · [Firmware RAM uses](#firmware-assigned-ram-uses) · [Sources](#sources-and-reproducibility)

For a conceptual introduction to these address spaces and their physical owners, see [HP-15C Hardware Architecture: NUT and R2D2]({{ '/hp15c/hardware-architecture.html' | relative_url }}).

## Scope and evidence

| Evidence class | Meaning in this document |
|---|---|
| **Confirmed (configuration)** | Explicitly declared in the pinned HP-15C model description. This is a model declaration, not an independent chip measurement. |
| **Confirmed (emulation)** | Explicit behavior of the pinned Nonpareil implementation. It may include simplifications of hardware. It does not imply that a physical calculator was tested. |
| **Observed (ROM)** | Words, transfers, or data operations established from the actual image using the CPU semantics. Conditions and preconditions still matter. |
| **Tentative** | A functional interpretation beyond what the cited evidence establishes. |

Addresses and ROM words are hexadecimal unless stated otherwise. Register widths, counts, nibble indexes, bit numbers, and keyboard layout IDs are decimal. `C[i]` is nibble `i`, with nibble 0 least significant. A data address denotes a **56-bit register**, not a byte; a ROM address denotes a **10-bit word**. The mnemonic field `x` means nibbles 0–2 of a CPU register; it is not the user-visible RPN X register.

## Address spaces

| Namespace | Address width / extent | Addressed unit | HP-15C interpretation |
|---|---|---|---|
| Instruction memory | 16-bit PC; `0000–FFFF` | 10-bit word | Two populated ROM regions; no RAM is declared in this space. |
| Data-register bus | 10-bit selector; `000–3FF` | 14 nibbles, normally 56 bits | 80 ordinary RAM registers plus six specially handled R2D2 addresses. |
| Peripheral-address selector | 8 bits; `00–FF` | Device-specific operations | Separate `pf_addr` namespace in the generic CPU emulator; no HP-41 peripheral register map is implied. |
| Smart-peripheral selector | 4 bits; `0–F` | Subsequent bus protocol words | Generic `SELPF` instruction state, not a RAM bank selector. |
| CPU-internal state | Named registers and latches | Varies | Not assigned data-bus addresses. |
| Keyboard code | 8 bits | Latched hardware key identifier | Not a RAM address or a ROM entry address. Firmware can use it as a ROM table index. |

Source: [CPU state definitions](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h#L24-L197), [RAM/PF selection](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L901-L1056), and [configuration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl). **Confirmed (configuration)** and **Confirmed (emulation)** as distinguished below.

## Instruction ROM

| Address range | Words | Device / interpretation | Evidence |
|---|---:|---|---|
| `0000–17FF` | 6,144 | R2D2 index 0, HP `1LE2-0321`, bank 0 | Confirmed (configuration) |
| `1800–1FFF` | 2,048 unpopulated positions | No ROM supplied or declared | Confirmed (configuration) |
| `2000–37FF` | 6,144 | R2D2 index 1, HP `1LH1-0302`, bank 0 | Confirmed (configuration) |
| `3800–FFFF` | 51,200 unpopulated positions | No ROM supplied or declared | Confirmed (configuration) |

Total populated ROM: **12,288 × 10 bits = 122,880 bits**, equivalent to 15,360 octets if tightly packed. The text dump's file size is unrelated to packed ROM capacity. The CPU part is declared as HP `1LF5-0301`.

The original ROM records agree exactly with these populated ranges. Nonpareil's generic CPU supports 4,096-word pages and multiple banks, but this configuration declares only bank 0. Generic bank-switch instructions do not establish additional HP-15C ROM banks.

**Unmapped fetches:** `nut_get_ucode()` returns zero if the selected page does not exist. Allocated page storage can also contain zero-filled positions beyond the populated image. Zero therefore describes an emulator fetch result, not proof of physical ROM at that address. A long subroutine call whose destination word reads as zero immediately pops the return address in `op_long_branch_word_2()`; a missing-ROM call cannot be analyzed as an ordinary unconditional transfer.

Sources: [configuration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl), [ROM fetch](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L162-L173), [long transfers](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L725-L755), [page allocation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L265-L285). See the [ROM coverage map]({{ '/hp15c/rom-map.html' | relative_url }}#complete-rom-coverage-index).

## Data RAM and R2D2 I/O

### Complete data-bus map

Every address in `000–3FF` is covered below. “Unmapped” means absent from this HP-15C model; no address aliasing is assumed.

| Data addresses | Count | Resource | Evidence |
|---|---:|---|---|
| `000–007` | 8 | Ordinary RAM, R2D2 index 0 | Confirmed (configuration) |
| `008` | 1 | Special zero/ignored register, index 0 | Confirmed (emulation) |
| `009–00A` | 2 | LCD bitmap registers, index 0 | Confirmed (configuration) + Confirmed (emulation) |
| `00B–00F` | 5 | Unmapped | Confirmed (configuration) + Confirmed (emulation) |
| `010–017` | 8 | Ordinary RAM, R2D2 index 1 | Confirmed (configuration) |
| `018` | 1 | Special zero/ignored register, index 1 | Confirmed (emulation) |
| `019–01A` | 2 | Index-1 registers with bitmap-style read masking; not connected to a second LCD in this model | Confirmed (emulation) |
| `01B–0BF` | 165 | Unmapped | Confirmed (configuration) + Confirmed (emulation) |
| `0C0–0DF` | 32 | Ordinary RAM, R2D2 index 1 | Confirmed (configuration) |
| `0E0–0FF` | 32 | Ordinary RAM, R2D2 index 0 | Confirmed (configuration) |
| `100–3FF` | 768 | Unmapped | Confirmed (configuration) + Confirmed (emulation) |

Each R2D2 supplies **40 ordinary registers**: eight low registers and 32 high registers. Total ordinary RAM is **80 × 56 = 4,480 bits**, equivalent to 560 octets. This does **not** mean 80 user storage registers: firmware state and user allocations share the resources.

The driver separately creates three special addresses per R2D2 at `index × 0x10 + 0x08`. Thus 86 data addresses are installed, of which 80 are ordinary RAM. Do not count all six special addresses as six additional physical 56-bit RAM registers.

Sources: [chip 0 declarations](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl#L24-L36), [chip 1 declarations](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl#L174-L185), and [`voyager_r2d2_install()`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L322-L361). The latter is authoritative for the extra I/O addresses; they are not ordinary `<memory>` declarations.

### Special-register access behavior

| Address | CPU read | CPU write | Display connection |
|---|---|---|---|
| `008`, `018` | All zero | Ignored | None |
| `009`, `00A` | Stored value with bits 0–5 forced to zero | Normal RAM backing-store write | All 100 connected LCD segment bits originate here. |
| `019`, `01A` | Same low-six-bit masking | Normal RAM backing-store write | No segment in the HP-15C configuration references these addresses. |

The bitmap read handler clears nibble 0 and masks nibble 1 with `0xC`; bits 6–55 remain readable. Nonpareil's backing store can retain low bits written to these addresses, but the CPU's read path masks them. A debugger reading the backing array directly is not equivalent to executing `C=DATA`.

Index 1 does not install another display controller. Its special addresses are still created and participate in memory operations. Their implementation is not evidence of a physically bonded-out second LCD.

For an unmapped RAM read with no responding peripheral, `op_read_reg_n()` leaves C zero and prints a stray-read warning. Unmapped writes have no RAM effect. These are emulator behaviors, not a description of electrical open-bus behavior.

Sources: [bitmap read](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L281-L305), [installation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L322-L361), [RAM read/write](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L934-L1056). **Confirmed (emulation).**

### Address selection and data transfer

| Nut operation | Opcode | Effect |
|---|---|---|
| `sel ram` | `270` | `ram_addr = (256 × C[2] + 16 × C[1] + C[0]) & 0x3FF`. |
| `c=data` | `038` | Read the currently selected RAM address; does **not** replace its low nibble. |
| `c=reg n`, n = 1…15 | `038 + 040 × n` | Replace the selected address's low nibble with n, retain the upper bits, then read. The changed selection persists. |
| `reg=c n`, n = 0…15 | `028 + 040 × n` | Replace the selected address's low nibble with n, then write C. Even n = 0 changes the selection. |
| `data=c` | `2F0` | Write C at the current address without changing the selector. |
| `sel pfad` | `3F0` | `pf_addr = 16 × C[1] + C[0]`; independent of `ram_addr`. |
| `selpf n` | `024 + 040 × n` | Enter smart-peripheral instruction state for selector n. This is distinct from `sel pfad`. |

Opcode formulas use hexadecimal constants; n is the numerical operand. Reads/writes can also query generic peripheral callbacks at the independently selected `pf_addr`. Selecting a PF address does not deselect RAM. The supplied HP-15C configuration/driver does not install the HP-41 peripheral set; in particular, do not import the Coconut LCD peripheral map into this Voyager map.

Sources: [`op_c_to_dadd()` through `op_smart_periph()`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L901-L1091). **Confirmed (emulation).**

## LCD segment map

The model has 11 positions, numbered 0–10, and segment identifiers a–j. Position 0 has only the sign segment g connected. Segments a–g are the seven digit segments; h is the decimal point; i is the comma tail; j is the annunciator channel associated with that position.

Each cell is **hexadecimal data-register address : decimal bit index**. A dash means `reg="-1", bit="-1"` in the configuration. All cells inherit **Confirmed (configuration)**.

| Position | a | b | c | d | e | f | g | h | i | j |
|---:|---|---|---|---|---|---|---|---|---|---|
| 0 | — | — | — | — | — | — | `009:46` | — | — | — |
| 1 | `009:21` | `009:23` | `009:19` | `009:47` | `009:18` | `009:20` | `009:22` | `009:39` | `009:38` | — |
| 2 | `009:27` | `009:29` | `009:25` | `009:17` | `009:24` | `009:26` | `009:28` | `009:15` | `009:14` | `009:16` |
| 3 | `009:51` | `009:53` | `009:49` | `009:13` | `009:48` | `009:50` | `009:52` | `009:55` | `009:54` | `009:12` |
| 4 | `009:33` | `009:35` | `009:31` | `009:9` | `009:30` | `009:32` | `009:34` | `009:37` | `009:36` | `009:8` |
| 5 | `009:43` | `009:45` | `009:41` | `009:7` | `009:40` | `009:42` | `009:44` | `009:11` | `009:10` | `009:6` |
| 6 | `00A:11` | `00A:13` | `00A:9` | `00A:15` | `00A:8` | `00A:10` | `00A:12` | `00A:17` | `00A:16` | `00A:14` |
| 7 | `00A:21` | `00A:23` | `00A:19` | `00A:7` | `00A:18` | `00A:20` | `00A:22` | `00A:25` | `00A:24` | `00A:6` |
| 8 | `00A:29` | `00A:31` | `00A:27` | `00A:39` | `00A:26` | `00A:28` | `00A:30` | `00A:37` | `00A:36` | `00A:38` |
| 9 | `00A:47` | `00A:49` | `00A:45` | `00A:33` | `00A:44` | `00A:46` | `00A:48` | `00A:35` | `00A:34` | `00A:32` |
| 10 | `00A:53` | `00A:55` | `00A:51` | `00A:41` | `00A:50` | `00A:52` | `00A:54` | `00A:43` | `00A:42` | `00A:40` |

This accounts for all **110 declared slots: 100 connected bits and 10 disconnected slots**. Registers `009` and `00A` each supply every bit 6–55 exactly once. Bits 0–5 are not segment storage.

For bit b, the implementation reads nibble `floor(b / 4)` and mask `1 << (b mod 4)`. Example: `009:46` is nibble 11, mask `0x4`, and drives position 0 segment g. Addresses in the XML are decimal (`reg="10"` means data address `00A`, not `010`).

The table uses configuration position/segment names rather than inferring printed annunciator legends from generic Voyager artwork or copied layout comments. A j-bit mapping alone does not prove a firmware flag's meaning.

### LCD control state

| Opcode | Mnemonic | Nonpareil effect | Storage class |
|---|---|---|---|
| `030` | `disp blink` | Enable display and blinking; begin in visible blink phase | Display-controller state |
| `2E0` | `disp off` | Disable display and blinking; delay update to allow an immediately following toggle | Display-controller state |
| `320` | `disp toggle` | Invert display enable; force update | Display-controller state |

The decoder also names opcode `3FC` as display compensation, but the examined Voyager driver does not install a handler for it; the mnemonic alone is not evidence of an implemented memory-mapped control. The generic CPU's low-battery test (`lld`, opcode `160`) simply clears carry (batteries treated as good). These implementation limits should not be mistaken for a complete electrical display/battery model.

These controls are instruction effects, **not additional data-RAM addresses**. The driver's `enable`, `blink`, refresh counter, and blink phase are emulator state. Its blink divisor of 150 and refresh scheduling must not be presented as directly measured physical timing. During sleep with the display disabled, the LCD event handler sets CPU carry to 1; that affects the ROM entry at `0000` on wake.

Sources: [complete LCD wiring](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl#L35-L171), [bitmap interpretation and display state](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c#L93-L278). **Confirmed (configuration)** for wiring; **Confirmed (emulation)** for control behavior.

## Keyboard interface

There is no keyboard matrix mapped into the data-RAM table. The model passes a hardware keycode to the CPU, which keeps a key latch and keyboard state machine.

| Interface | Opcode / state | Effect |
|---|---|---|
| `c=keys` | `220` | Copy keycode high nibble into C[4], low nibble into C[3]. Other nibbles are unchanged. |
| `goto keys` | `230` | Replace the low byte of the **already incremented** PC with the keycode. It does not read the firmware's dispatch table. |
| `? kb` | `3CC` | Carry becomes true in `KB_PRESSED` or `KB_RELEASED`; can initiate the debounce countdown. |
| `clr kb` | `3C8` | Change `KB_RELEASED` to `KB_WAIT_CHK`; does not unconditionally erase a held key. |
| Press / release | CPU events | Press latches the key; release changes key-down/state, not the keycode itself. |
| Deep sleep wake | Display disabled | Only hardware keycode `18` (ON) can wake a sleeping CPU through `nut_press_key()`. |
| Debounce | 32 emulator cycles | `KB_WAIT_CHK → KB_WAIT_CYC → KB_IDLE` or `KB_PRESSED`, according to key-down state. |

Source: [keyboard instructions and scanner](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1443-L1543), [press/release events](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L2056-L2097). **Confirmed (emulation).** The model has one latched keycode and one key-down flag; this does not by itself establish electrical behavior for simultaneous key presses.

### Complete hardware keycode table

Layout ID is the configuration's decimal `user_keycode`, not a scan code. Shifted functions use the same physical key; f/g processing belongs to firmware. All 39 keys are listed in configuration order. **Confirmed (configuration).**

| Layout ID | Unshifted legend | Hardware code |
|---:|---|---|
| 11 | sqrt | `13` |
| 12 | e^x | `33` |
| 13 | 10^x | `73` |
| 14 | y^x | `C3` |
| 15 | 1/x | `83` |
| 16 | CHS | `82` |
| 17 | 7 | `C2` |
| 18 | 8 | `72` |
| 19 | 9 | `32` |
| 10 | divide | `12` |
| 21 | SST | `10` |
| 22 | GTO | `30` |
| 23 | SIN | `70` |
| 24 | COS | `C0` |
| 25 | TAN | `80` |
| 26 | EEX | `87` |
| 27 | 4 | `C7` |
| 28 | 5 | `77` |
| 29 | 6 | `37` |
| 20 | multiply | `17` |
| 31 | R/S | `11` |
| 32 | GSB | `31` |
| 33 | Rdn | `71` |
| 34 | x<>y | `C1` |
| 35 | backspace | `81` |
| 36 | ENTER | `84` |
| 37 | 1 | `C4` |
| 38 | 2 | `74` |
| 39 | 3 | `34` |
| 30 | subtract | `14` |
| 41 | ON | `18` |
| 42 | f | `38` |
| 43 | g | `78` |
| 44 | STO | `C8` |
| 45 | RCL | `88` |
| 47 | 0 | `C5` |
| 48 | decimal | `75` |
| 49 | Sigma+ | `35` |
| 40 | add | `15` |

Source: [keyboard declaration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl#L187-L231). See the ROM map for the [keycode-indexed dispatch words]({{ '/hp15c/rom-map.html' | relative_url }}#keycode-indexed-dispatch-table) and the [diagnostic key sequence]({{ '/hp15c/rom-map.html' | relative_url }}#diagnostic-key-sequence-table).

## CPU-internal registers

These are CPU state, **not addresses in `000–3FF`**. Register purpose here describes the machine interface, not a fixed firmware allocation.

| Register / state | Width | Meaning / access |
|---|---:|---|
| A, B, C | 56 bits each | Arithmetic/data registers, operated on in selected nibble fields. C is also the data-bus and address-formation register. |
| M, N | 56 bits each | Auxiliary registers; transfer/exchange with C. Names do not establish a permanent RPN role. |
| G | 8 bits | Two nibbles transferred/exchanged with C around the active pointer. Pointer-at-13 handling has special cases and an explicit implementation caveat. |
| P, Q | 4-bit storage each | Digit pointers, normally 0–13. Increment/decrement wrap over 14 digit positions. |
| Q-select | 1 bit | Selects P or Q as the active pointer used by pointer-relative instructions. |
| S[0…13] | 14 bits | Status bits; explicitly indexed instructions can access individual bits. `clr s`, C/status transfers, and F/status transfers operate only on S[0…7]. |
| F / FO | 8 bits | Flag-output latch, transferred to/from low eight status bits. Generic sound support does not imply an HP-15C speaker. |
| PC | 16 bits | Instruction-word address. |
| Return stack | 4 × 16 bits | Hardware subroutine return addresses; separate from RAM and from the user's RPN stack. Push shifts/discards the oldest entry; pop shifts and zero-fills. |
| Carry / previous carry | 1 bit each in model | Carry is cleared for each cycle; branch/conditional-return instructions test saved previous carry. Arithmetic subtraction uses carry as borrow. |
| Decimal mode | 1 bit | Selects base-10 or base-16 arithmetic. It does not change the 4-bit nibble storage format. |
| RAM selector | 10 bits | Selected data-register address. |
| PF selector | 8 bits | Selected peripheral address. |
| SELPF selector | 4 bits | Smart-peripheral selection; subsequent words use a different decoding state until a word with bit 0 set returns control. |
| External flags | 14 boolean inputs | Generic Nut peripheral inputs; do not confuse these with RAM-resident user flags or S bits. HP-41-specific names in the header do not prove HP-15C wiring. |
| Keyboard latch/state | 8-bit code plus state | Keycode, key-down and debounce state. |
| Awake/display state | Boolean state | CPU wake/sleep and the CPU's copy of display enable. |
| Instruction phase / pending operand | Emulator bookkeeping | Tracks second words of long branches/LDI, CXISA's extra read cycle and SELPF state. Not general-purpose firmware RAM. |
| Active-bank/page arrays | Emulator bookkeeping | Generic Nut mapping machinery; original HP-15C configuration only supplies bank 0. |

Sources: [state layout](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h#L24-L197), [CPU register metadata](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L93-L133), [return stack](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L626-L706), [status transfers](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1096-L1181), [G-register caveats](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1355-L1436), [cycle execution](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1902-L1966). **Confirmed (emulation).**

### Arithmetic fields

| Mnemonic field | Nibble interval |
|---|---|
| `p` | Active pointer only |
| `x` | 0–2 |
| `wp` | 0 through active pointer |
| `w` | 0–13 |
| `pq` | P through Q; if P > Q, P through 13 |
| `xs` | 2 |
| `m` | 3–12 |
| `s` | 13 |

Source: [`op_arith()` field selection](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L418-L440). **Confirmed (emulation).** The field names reflect conventional numeric formatting, but the firmware can use the same nibbles for addresses, counters or packed flags.

### ROM-address formation

| Operation | Address formation / effect |
|---|---|
| `goto c` (`1E0`) | PC ← `4096 × C[6] + 256 × C[5] + 16 × C[4] + C[3]`. |
| `cxisa` (`330`) | Read the ROM word addressed by C[6:3] into C[2:0] on an extra cycle; the next program word is not an inline operand. |
| `stk=c` / `c=stk` | Push/pop a return address using C[6:3]. |
| `ldi` (`130`) | Consume the next ROM word as a 10-bit literal into C[2:0]. |
| Short branch | Destination = instruction address + signed 7-bit displacement, modulo 65,536; conditional on previous carry. |
| Long branch/call | Two words: destination = `(word1 >> 2) \| ((word2 & 0x3FC) << 6)`; word2 bits 0/1 select carry condition and call/jump. |

Sources: [branches and computed jump](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L711-L767), [ROM reads and literals](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c#L1572-L1636). **Confirmed (emulation).**

## Firmware-assigned RAM uses

A dedicated [80-register RAM-use reference]({{ '/hp15c/ram-register-map.html' | relative_url }}) records all ordinary addresses, Work-reported execution-trace read/write sites, the initial real-mode Y/Z/T and R0–R9 mappings, and explicit unknowns. Its assignments apply only to the tested states and are not a complete firmware allocation model.

The following entries describe specific evidenced accesses. They are deliberately narrower than a complete user-memory allocation scheme. **Observed (ROM)** unless explicitly marked otherwise.

| Data address / field | Established use | ROM evidence and limits |
|---|---|---|
| `007`, nibbles 0–2 | Retention/initialization signature tested against `EAE` | [`0348–0350`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L843-L851) constructs `~151 & FFF = EAE`, selects page zero and compares `RAM[007].x`. [`13B1–13B6`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5044-L5049) writes this signature before sleep. Meaning of other nibbles is not assigned here. |
| `000–002` | Three-register shift service | [`17E8–17F3`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6123-L6134) writes `RAM[002] ← RAM[001]`, `RAM[001] ← RAM[000]`, `RAM[000] ← incoming C`, using M as temporary storage. A complete, mode-independent assignment to RPN Y/Z/T is not established here. |
| `004` | Temporary N-register save during diagnostics | [`1730–1731`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5939-L5940) saves N after selecting data page zero; [`1775–1778`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6008-L6011) restores it on the diagnostic success path. This is a use during this routine, not an exclusive purpose of RAM[004]. |
| `015` | Initialization value written by the clear/setup path | [`035C–0365`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L863-L872) selects `010` through helper `295B`, then fills and writes `C0D2D2D2D2D2D2` to register 5 of that page. Descriptor/allocator interpretation remains **Tentative**. |
| `01A` | Initialization write `00000000A00000` | [`0368–036B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L875-L878), while data page `010` remains selected. This is one of the masked special registers, not ordinary RAM. Permanent field meanings remain unknown. |
| `009`, `00A` | Display bitmap writes | Many sites; [`2845–285E`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8264-L8289) constructs fixed bitmaps and enables blinking; [`1779–177C`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L6012-L6015) writes all-one bitmaps. Wiring is independently confirmed above. |
| All ordinary RAM, plus special/unmapped positions encountered | Initialization clearing | [`0355–035B`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L856-L862) calls [`2AE9–2AF5`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L8940-L8952) with low-byte bounds `A=C0`, `C=1B`. The loop attempts zero writes to `01A…000`, then `0FF…0C0`; the selector is not a byte address. See [clear-loop proof]({{ '/hp15c/rom-map.html' | relative_url }}#initialization-and-memory-clearing). |
| `008`, `018` | Accesses may occur, but values do not persist through CPU writes | For example [`13AF–13B0`](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom#L5042-L5043) writes all ones to `008`; the emulator ignores it. Do not assign stored firmware flags to these addresses. |
| Other locations / fields | Firmware-assigned meanings not established | No complete verified mapping of user R0–R9, I, LAST X, real/complex stacks, statistics, matrices, program storage, allocation metadata or packed user flags is supplied by the cited configuration/CPU files. |

The RAM-clearing routine covers 91 **attempted addresses**: 80 ordinary registers, four bitmap-style registers, two ignored special registers and five unmapped positions (`00B–00F`). The post-clear setup writes nonzero values; “memory clear” therefore does not mean that every mapped register is zero when the calculator returns to use.

A full firmware allocation map needs independently verified state transitions and mode-sensitive analysis. Physical capacity and initialization constants alone do not establish user-register numbering or packed program layout.

## Sources and reproducibility

- [Nonpareil HP-15C configuration](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.ncd.tmpl): chip identities, declared memory, LCD wiring, keyboard codes.
- [Original ROM](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/ncd/15c/15c.rom): the 12,288-word dump used here.
- [NUT CPU implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.c) and [state definitions](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/proc_nut.h).
- [Voyager R2D2/LCD implementation](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/voyager_lcd.c).
- [NUT disassembler](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/dis_nut.c); [digit arithmetic](https://github.com/brouhaha/nonpareil/blob/c347bc1ab20170c253512042f7aac0d952f304ea/src/digit_ops.c).
- [Published standalone disassembler](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/hp15c_disasm.c), [decoder tests](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/test_decoder.c), and [file-integrity tests](https://github.com/kerouanton/hp-voyager-internals/blob/main/tools/hp15c-disassembler/verify.cjs).

Upstream commit: `c347bc1ab20170c253512042f7aac0d952f304ea`. Project snapshot: `789ed0d540b022a97efe6afca369747b750e69b2`. Prepared 2026-10-28. The ROM file's SHA-256 is `dea4e861ad2aaa2bba49b5ea4e313a04f4fac9a4abf2516284ecc0f6f394568a`; its Git blob SHA-1 is `c276b93de235448e65b411412c579c1070cd8955`. These identify the original image, not the separate 192-register patch or later HP-15C products.

The LCD table was transcribed programmatically from all 110 configuration slots and checked for 100 distinct connected register/bit pairs. All 39 key entries were checked against the model. The data map partitions all 1,024 selector values without overlap. The ROM address records were checked against both declared regions. The published C decoder and its 265,216-case test suite were compiled and run; decoder tests validate decoding, not hardware fidelity or firmware names.

The address-oriented presentation is inspired by the [Commodore 64 memory map](https://sta.c64.org/cbm64mem.html). Its byte-addressed architecture is not used as a technical model for the Nut.

[Back to top](#hp-15c-complete-memory-map) · [ROM map]({{ '/hp15c/rom-map.html' | relative_url }})

## Document credits

Technical investigation and original draft: **ChatGPT Work** (2026-10-28), using Eric Smith's Nonpareil and the HP-15C ROM. Publication and additional editorial integration: **OpenAI ChatGPT in manual (agentless) mode**, under the project maintainer's direction. The original NUT emulation, disassembler and machine configuration are credited to **Eric Smith**. Analysis labels describe the evidence available, not independent measurement of physical hardware.
