/* Regression tests for the standalone Nonpareil Nut decoder. GPL-3.0-only. */
#define main disassembler_main
#include "hp15c_disasm.c"
#undef main
#include <assert.h>

int main(void)
{
  unsigned checks = 0;
  for (unsigned base = 0; base <= 65535; base += 65535) {
    for (unsigned op = 3; op < 1024; op += 4) {
      for (unsigned known = 0; known < 2; ++known) {
        addr_t pc = base, target = 0, d = 0;
        bank_t b = 0, tb = 0;
        inst_state_t state = inst_normal;
        bool carry = known != 0;
        flow_type_t flow;
        char text[256] = "";
        rom.present[base] = true; rom.words[base] = (rom_word_t)op;
        assert(nut_disassemble(&rom, 0, &b, &pc, &state, &carry, &d, &d,
                              &flow, &tb, &target, text, sizeof text));
        int signed_offset = (int)(op >> 3);
        if (signed_offset >= 64) signed_offset -= 128;
        assert(target == ((base + signed_offset) & 65535));
        assert((flow == flow_uncond_branch) == (known && !(op & 4)));
        ++checks;
      }
    }
  }
  /* All 65536 destinations and all four carry/call selector combinations. */
  rom.present[0x100] = rom.present[0x101] = true;
  for (unsigned target = 0; target < 65536; ++target) {
    for (unsigned mode = 0; mode < 4; ++mode) {
      addr_t pc = 0x100, actual = 0, d = 0;
      bank_t b = 0, tb = 0;
      inst_state_t state = inst_normal;
      bool carry = false;
      flow_type_t flow;
      char text[256] = "";
      rom.words[0x100] = (rom_word_t)(((target & 255) << 2) | 1);
      rom.words[0x101] = (rom_word_t)(((target >> 8) << 2) | mode);
      assert(nut_disassemble(&rom, 0, &b, &pc, &state, &carry, &d, &d,
                            &flow, &tb, &actual, text, sizeof text));
      assert(pc == 0x102 && actual == target);
      assert(strncmp(text, mode & 1 ? "?c " : "?nc", 3) == 0);
      assert((flow == flow_subroutine_call) == !(mode & 2));
      ++checks;
    }
  }
  /* Every first-word encoding, including LDI and selected-peripheral exit. */
  for (unsigned op = 0; op < 1024; ++op) {
    for (unsigned peripheral = 0; peripheral < 2; ++peripheral) {
      addr_t pc = 0x100, target = 0, d = 0;
      bank_t b = 0, tb = 0;
      inst_state_t state = peripheral ? inst_nut_selpf : inst_normal;
      bool carry = false;
      flow_type_t flow;
      char text[256] = "";
      rom.words[0x100] = (rom_word_t)op; rom.words[0x101] = 0x3ff;
      assert(nut_disassemble(&rom, 0, &b, &pc, &state, &carry, &d, &d,
                            &flow, &tb, &target, text, sizeof text));
      assert(pc == 0x101u + (!peripheral && ((op & 3) == 1 || op == 0x130)));
      if (peripheral) assert(state == ((op & 1) ? inst_normal : inst_nut_selpf));
      if (!peripheral && op == 0x130) assert(!strcmp(text, "ldi 1777"));
      ++checks;
    }
  }
  printf("PASS: %u decoder cases\n", checks);
  return 0;
}
