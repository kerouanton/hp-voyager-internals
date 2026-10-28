---
title: Firmware Tools
---

# Firmware Tools and Reproduction

[Home](../index.md) · [Firmware](index.md)

For authorship, provenance, test coverage, and the maintainer's Cygwin validation, see [Disassembler development and validation history](disassembler-history.md).

## Standalone NUT disassembler

[**Source, tests and original documentation**](../../tools/hp15c-disassembler/README.md)

The NUT decoder is adapted from Eric Smith's Nonpareil and runs without GTK or the Nonpareil GUI. Compile with a C11 compiler; the commands below work with GCC on Cygwin.

```sh
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic hp15c_disasm.c -o hp15c_disasm.exe
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror test_decoder.c -o test_decoder.exe
./test_decoder.exe
./hp15c_disasm.exe 15c.rom > 15c_disassembled.lst
./hp15c_disasm.exe --every-address 15c.rom > 15c_every_address.lst
```

Run these commands in the `tools/hp15c-disassembler/` directory, after obtaining `15c.rom` separately from [Nonpareil](https://github.com/brouhaha/nonpareil/blob/main/ncd/15c/15c.rom).

### Interpretation limits

The **linear listing** assumes instruction boundaries sequentially, while **every-address** assumes a fresh normal decoding state at each ROM location. Both outputs are research aids; neither reconstructs runtime control flow or proves which ROM regions are code.

### Tests

The C regression suite covers 265,216 input cases involving absolute transfers, relative transfers, and decoder states, as documented in the original tool bundle. These test structural decoding, **not the full behavior of the HP-15C firmware**.

The optional Node.js verifier is provided as `verify.cjs`; it checks ROM provenance and generated listing integrity using a locally obtained copy of the original ROM.

### Rights

The C implementation derives from GPL-3.0-only Nonpareil source; the associated [license text](../../tools/hp15c-disassembler/COPYING) and original [licensing notice](../../tools/hp15c-disassembler/NONPAREIL-COPYING.txt) are included. The ROM is not redistributed. See [Sources](../sources.md).
