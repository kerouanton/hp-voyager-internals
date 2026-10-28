# HP-15C ROM Disassembler (standalone C11)

A command-line **NUT instruction disassembler** adapted from Eric Smith's [Nonpareil](https://github.com/brouhaha/nonpareil) `src/dis_nut.c`. Intended for examining the **original HP-15C ROM** on Cygwin, Linux and other environments supporting C11.

This directory preserves the code and supporting material supplied in the original tool bundle, with an additional Markdown guide for the public project. **It is not a calculator emulator, a firmware decompiler, or a verified reconstruction of executable code boundaries.**

## Contents

| File | Purpose |
|---|---|
| [`hp15c_disasm.c`](hp15c_disasm.c) | Standalone C11 disassembler |
| [`test_decoder.c`](test_decoder.c) | Branch-encoding and decoder-state regression tests |
| [`verify.cjs`](verify.cjs) | Optional Node.js file-integrity and command-line checks |
| [`README.txt`](README.txt) | Original comprehensive tool-bundle documentation |
| [`COPYING`](COPYING) | GNU GPL v3 text |
| [`NONPAREIL-COPYING.txt`](NONPAREIL-COPYING.txt) | Upstream historical licensing information |

## Build and run (Cygwin / GCC)

```sh
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic hp15c_disasm.c -o hp15c_disasm.exe
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror test_decoder.c -o test_decoder.exe

./test_decoder.exe
./hp15c_disasm.exe 15c.rom > 15c_disassembled.lst
./hp15c_disasm.exe --every-address 15c.rom > 15c_every_address.lst

# Optional (requires Node.js):
node verify.cjs ./hp15c_disasm.exe
```

Download **`15c.rom` separately**, from the [Nonpareil upstream file](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom). This repository **does not redistribute HP firmware** or ROM-derived listings.

The default listing uses a linear sweep and preserves raw words. `--every-address` decodes independently at each recorded ROM address, including possible operand or data locations. **Neither mode identifies executable code with certainty.**

For exact parser behavior, instruction notation, validation scope, and provenance, refer to [`README.txt`](README.txt).

## Development and validation

The standalone adaptation, regression tests, verification helper, and initial annotated listings were produced using **ChatGPT Work** from Eric Smith's Nonpareil decoder. Work reported successful compilation and testing on Linux, with **265,216 regression cases passing**. The maintainer subsequently compiled and tested the delivered tool under **Cygwin**, confirming operation there.

The project's reference documentation and GitHub organization are maintained with assistance from **OpenAI ChatGPT in manual ("agentless") mode**, with the maintainer directing and reviewing the work.

See the [development history](../../docs/firmware/disassembler-history.md).

## Attribution and licensing

The core NUT decoder is derived from Nonpareil by **Eric Smith**, retaining its original copyright and `GPL-3.0-only` notice. The standalone modifications and support files are distributed with the included [GPL version 3](COPYING) notice.

The upstream ROM and ROM-derived listings have **separate** copyright/provenance concerns: the GPL applicable to the C source **does not license HP firmware**.

See the [firmware section](../../docs/firmware/index.md) and [project source policy](../../docs/sources.md).
