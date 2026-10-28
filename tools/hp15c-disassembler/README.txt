HP-15C ROM disassembly / standalone Cygwin tool
==============================================

DELIVERABLES
hp15c_disasm.c          Standalone C11 disassembler (one source file).
15c.rom                Exact original Nonpareil ROM dump.
15c_disassembled.rom    Annotated linear listing, 10,284 rows / 12,288 words.
15c_every_address.lst  Independent decode at all 12,288 stored addresses.
test_decoder.c         Exhaustive branch and decoder-state regression tests.
verify.cjs             Optional Node.js integrity and command-line tests.
COPYING                GPL version 3 text for the adapted C source.
NONPAREIL-COPYING.txt   Original repository licensing notice, unmodified.

The C file is the DISASSEMBLER, not a decompilation of the calculator firmware
into C and not a calculator emulator. It uses only the standard C library;
GTK, GLib, SDL, SCons and Nonpareil itself are not required to build it.

CYGWIN BUILD AND RUN
Install the Cygwin gcc-core package using the Cygwin installer. In the Cygwin
terminal, change to the extracted hp15c directory, then run:

  gcc -std=c11 -O2 -Wall -Wextra -Wpedantic hp15c_disasm.c -o hp15c_disasm.exe
  ./hp15c_disasm.exe 15c.rom > regenerated.lst
  ./hp15c_disasm.exe --every-address 15c.rom > every_address.lst

For an unoptimized debug build:

  gcc -std=c11 -O0 -g3 -Wall -Wextra -Wpedantic hp15c_disasm.c -o hp15c_disasm.exe

Usage errors return 2; input/output errors return 1; success returns 0.
Input is hexadecimal address:word text, with optional # or ; comments.
Blank lines and CRLF line endings are accepted. Duplicate addresses, addresses
above FFFF, words above 3FF, oversized lines and trailing junk are rejected.
Redirect the listing to a DIFFERENT file from the input. Status goes to stderr.

HOW TO READ THE LISTINGS
Columns: hexadecimal address, raw hexadecimal word(s), Nut mnemonic, comment.
One address denotes one 10-bit word, not one byte. General branches and LDI
consume two words. Nonpareil's LDI mnemonic uses OCTAL (e.g. 1777 = hex 3FF).
Register numbers, status-bit indexes and LC constants use decimal notation.
?c and ?nc test the previous carry (1 and 0, respectively); conditions are
retained even when a preceding linear instruction appears to clear carry.
Branch targets are printed as $hhhh hexadecimal addresses.

The dump contains 0000..17FF and 2000..37FF, with a hole at 1800..1FFF.
The hole is preserved and is not padded with invented zero words.

The default listing is a LINEAR SWEEP, not a proven control-flow reconstruction.
It preserves every stored word exactly once, including operand words in the
raw-word column. Embedded tables can decode as plausible instructions and can
shift the apparent instruction boundaries. For example, a transfer outside
supplied ROM may indicate data, an alternate boundary, or unresolved context;
it does not by itself prove an actual firmware error.

The every-address listing is intentionally overlapping: it shows the normal-
state interpretation starting at each stored address, including addresses that
the linear listing treats as operands. It is useful for inspecting computed
jump targets and alternate entry points. It does not prove executability.
Default mode follows SELPF protocol state until its return bit; every-address
mode resets to normal state at each address. No HP-41-specific three-word jump
pseudo-instructions are enabled for this Voyager ROM.

Comments describe instruction semantics and register fields, checked against
proc_nut.c. They do NOT identify mathematical algorithms, user functions,
constants, or original HP symbol names. No unsupported routine names have been
invented. Mnemonics include generic Nut extensions inherited from Nonpareil;
seeing one in a data region does not imply that HP-15C hardware implements it.
The .rom-suffixed disassembly is a human-readable listing, NOT a ROM dump that
Nonpareil can load, and NOT assembler-ready source. Use 15c.rom for emulation.

PROVENANCE AND CHANGES
Repository: https://github.com/brouhaha/nonpareil
Pinned commit: c347bc1ab20170c253512042f7aac0d952f304ea
Retrieved: 2026-10-27
Decoder: src/dis_nut.c (Eric Smith, GPL-3.0-only)
CPU semantics checked: src/proc_nut.c
Architecture / memory map checked: ncd/15c/15c.ncd.tmpl
ROM: ncd/15c/15c.rom (header says extracted 2004-01-25)
ROM Git blob SHA-1: c276b93de235448e65b411412c579c1070cd8955
ROM SHA-256: dea4e861ad2aaa2bba49b5ea4e313a04f4fac9a4abf2516284ecc0f6f394568a

The original decoder is incorporated into the single C file, with its original
copyright and license notice retained. Adaptations: standard-library host,
strict ROM parser, annotations, conservative carry handling, initialize the
otherwise uninitialized short-branch 'uncond' variable, fix the malformed
selected-peripheral fallback format, replace fallthrough attributes with C11
comments, remove unused-argument warnings, and preserve unassigned register
encodings as raw constants instead of displaying register -1. Incomplete or
address-space-wrapping instructions are preserved as raw words.

The C derivative is GPL-3.0-only; see COPYING. The ROM and its derived listings
retain their separate upstream provenance. The ROM itself has no explicit
per-file license header. NONPAREIL-COPYING.txt contains historical mixed-license
information; do not assume that the C source's GPL notice licenses HP firmware.

VALIDATION
Built and executed with GCC on Linux using C11 and -Wall -Wextra -Wpedantic
-Werror. Direct execution under Cygwin was not available in this environment.
Cygwin compatibility is based on exclusive use of standard C11 interfaces.
No Linux executable is supplied as a Windows/Cygwin binary.

Run the C regression suite under Cygwin:

  gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror test_decoder.c -o test_decoder.exe
  ./test_decoder.exe

Result: PASS, 265216 decoder cases: all 65536 absolute branch destinations
with all 4 carry/call modes; all relative branch encodings at address-space
boundaries with both carry-knowledge inputs; all 1024 first-word encodings in
normal and selected-peripheral state. These verify decoding, not the execution
of the calculator firmware or a complete identification of code versus data.

Optional Node.js verification (not a build/runtime dependency):

  node verify.cjs ./hp15c_disasm.exe

Checks: exact upstream Git blob identity; full lossless reconstruction of all
12288 words from the supplied linear listing; malformed-input rejection and
CRLF/incomplete-instruction handling (9 fixtures).
