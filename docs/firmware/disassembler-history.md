---
title: HP-15C Disassembler — Development and Validation History
---

# HP-15C Disassembler: Development and Validation

[Home](../index.md) · [Firmware](index.md) · [Tools](tools.md) · [Source code](../../tools/hp15c-disassembler/README.md)

This page records **who contributed what**, where the code originated, what was tested, and the limits of the results.

## Why this tool exists

The initial goal was to turn the original HP-15C ROM dump into a readable NUT instruction listing that could be explored on **Cygwin**, without requiring GTK or a full Nonpareil build.

Rather than invent a new instruction set decoder, the project started from **Eric Smith's Nonpareil** sources, especially [`src/dis_nut.c`](https://github.com/brouhaha/nonpareil/blob/main/src/dis_nut.c) and the [HP-15C ROM dump](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom).

The standalone adaptation cites the pinned upstream Nonpareil commit `c347bc1ab20170c253512042f7aac0d952f304ea`; see its original [`README.txt`](../../tools/hp15c-disassembler/README.txt) for provenance details.

## Who did what

| Contributor / workflow | Role |
|---|---|
| **Eric Smith / Nonpareil** | Original NUT decoder, emulator, model configuration, and source repository for the HP-15C ROM dump. |
| **ChatGPT Work** | Produced the standalone C11 adaptation, test harness, integrity helper, documentation bundle, and initial annotated listings. |
| **Project maintainer** | Defined the goal, compiled and executed the result successfully under **Cygwin**, and reviewed and approved publication. |
| **OpenAI ChatGPT, manual ("agentless") mode** | Reviewed technical materials, helped organize the reference website, checked source-level claims, and prepared documentation and GitHub contributions interactively. |

**Terminology:** *agentless* means the human-directed conversational workflow used for this documentation. This does not imply an absence of tools. The original standalone C tool was created in **ChatGPT Work**, a separate workflow.

## Deliverables

The original bundle includes:

- `hp15c_disasm.c`: standard-C11 command-line disassembler;
- `test_decoder.c`: structural decoder tests;
- `verify.cjs`: optional Node.js integrity and CLI tests;
- `README.txt` and licensing notes;
- a linear annotated ROM listing and an independent every-address listing.

The source files and tests are hosted under [`tools/hp15c-disassembler/`](../../tools/hp15c-disassembler/README.md). **The ROM dump and its ROM-derived listings are not redistributed here.** Visitors can obtain the ROM from its [upstream Nonpareil location](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom) and regenerate the listings locally.

## Validation record

### Work's original tests

**ChatGPT Work reported** successful compilation and execution on Linux. It reported **265,216 passing decoder cases**, distributed as follows:

| Category | Cases |
|---|---:|
| Absolute branches: 65,536 destinations × 4 modes | 262,144 |
| Relative branches: 256 encodings × 2 boundary addresses × 2 carry-knowledge states | 1,024 |
| 1,024 first-word encodings × 2 decoder states | 2,048 |
| **Total** | **265,216** |

The test cases check structure, branch destinations, instruction lengths, and state handling. They **do not independently prove the semantic correctness of every mnemonic**, establish executable code/data boundaries, or validate numerical algorithms in the HP-15C firmware.

### Cygwin confirmation

The **project maintainer subsequently compiled and tested** the delivered disassembler and test program successfully under **Cygwin**, confirming the practical portability that Work could not verify directly in its Linux environment.

This statement records the maintainer's reported local verification; no specific Cygwin version, compiler version, or archived terminal transcript is asserted here.

### Reproducing the tests

Run from `tools/hp15c-disassembler/`:

```sh
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic hp15c_disasm.c -o hp15c_disasm.exe
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror test_decoder.c -o test_decoder.exe
./test_decoder.exe

# Requires a separately obtained 15c.rom
./hp15c_disasm.exe 15c.rom > 15c_disassembled.lst
./hp15c_disasm.exe --every-address 15c.rom > 15c_every_address.lst

# Optional: requires Node.js and the original ROM
node verify.cjs ./hp15c_disasm.exe
```

For full usage notes and exact validation fixtures, consult [`README.txt`](../../tools/hp15c-disassembler/README.txt).

## How to interpret the output

The **linear sweep** preserves ROM words and chooses provisional instruction boundaries. The **every-address** mode attempts decoding from each stored ROM address independently, including addresses that might be data or operand words.

Neither output demonstrates that all decoded words are executed, and **a disassembler is not an emulator or a decompiler**. Functional ROM maps require additional control-flow analysis, traces, and scrutiny of the hardware model.

## Rights and acknowledgements

The adapted C decoder preserves Eric Smith's `GPL-3.0-only` copyright/license notice and is distributed with the [GPL text](../../tools/hp15c-disassembler/COPYING). Its license **does not grant rights to the original HP firmware**. More historical licensing context appears in [`NONPAREIL-COPYING.txt`](../../tools/hp15c-disassembler/NONPAREIL-COPYING.txt).

[Back to Firmware](index.md)
