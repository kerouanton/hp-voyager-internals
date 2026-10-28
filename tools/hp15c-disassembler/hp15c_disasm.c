/*
Copyright 2004-2023 Eric Smith <spacewar@gmail.com>
SPDX-License-Identifier: GPL-3.0-only

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 as
published by the Free Software Foundation.

Note that permission is NOT granted to redistribute and/or modify
this porogram under the terms of any other version, earlier or
later, of the GNU General Public License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License version 3 for more details.

You should have received a copy of the GNU General Public License
version 3 along with this program (in the file "gpl-3.0.txt"); if not,
see <https://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


/* Standalone C11 adaptation, 2026.
 * Source: brouhaha/nonpareil, commit c347bc1ab20170c253512042f7aac0d952f304ea, src/dis_nut.c.
 * Changes: standard-library host; initialized short-branch flag; corrected
 * peripheral fallback format; C11 fallthrough comments; strict ROM loader;
 * conservative carry handling; annotated linear and every-address listings.
 * The decoder copyright and GPL-3.0-only notice above are preserved.
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#define UNUSED
#define DIS_FLAG_NUT_41_JUMPS 1u
#define DIS_FLAG_LISTING 2u
#define DIS_FLAG_LABEL 4u
typedef uint8_t bank_t;
typedef uint32_t addr_t;
typedef uint16_t rom_word_t;
typedef enum { inst_normal, inst_nut_selpf } inst_state_t;
typedef enum { flow_no_branch, flow_cond_branch, flow_uncond_branch,
 flow_subroutine_call, flow_cond_subroutine_return,
 flow_subroutine_return } flow_type_t;
typedef struct { rom_word_t words[65536]; bool present[65536]; } sim_t;
static sim_t rom;
static bool sim_read_rom(sim_t *s, bank_t bank, addr_t addr, rom_word_t *word)
{
  if (bank != 0 || addr > 65535 || !s->present[addr]) return false;
  *word = s->words[addr]; return true;
}
static void buf_printf(char **buf, int *len, const char *fmt, ...)
{
  int n;
  va_list ap;
  if (*len <= 0) return;
  va_start(ap, fmt); n = vsnprintf(*buf, (size_t)*len, fmt, ap); va_end(ap);
  if (n < 0) return;
  if (n >= *len) n = *len - 1;
  *buf += n; *len -= n;
}



static int nut_disassemble_selpf (int op1,
				  inst_state_t *inst_state,
				  bool *new_carry_known_clear,
				  char *buf,
				  int len)
{
  (void)new_carry_known_clear;
  if (op1 & 0x001)
  {
    *inst_state = inst_normal;
  }

  if ((op1 & 0x003) == 0x000)
    buf_printf(&buf, &len, "pfchnr 0x%02x", op1 >> 2);
  else if ((op1 & 0x003) == 0x001)
    buf_printf(&buf, &len, "pfchr  0x%02x", op1 >> 2);
  else if ((op1 & 0x03f) == 0x03a)
    buf_printf(&buf, &len, "c=pfnr %d", op1 >> 6);
  else if ((op1 & 0x03f) == 0x03b)
    buf_printf(&buf, &len, "c=pfr  %d", op1 >> 6);
  else if ((op1 & 0x03f) == 0x003)
    buf_printf(&buf, &len, "?pfsft %d", op1 >> 6);
  else
    buf_printf(&buf, &len, "??? 0x%03x", op1);

  return true;
}


static int nut_disassemble_short_branch (rom_word_t op1,
					 bank_t *bank,
					 addr_t *addr, 
					 bool *carry_known_clear,
					 flow_type_t *flow_type,
					 bank_t *target_bank,
					 addr_t *target_addr,
					 char *buf,
					 int len)
{
  int offset;
  bool cond_c, uncond = false;

  offset = (op1 >> 3) & 0x3f;
  if (op1 & 0x200)
    offset -= 64;
  *target_addr = ((*addr) + offset - 1) & 0xffff;
  *target_bank = *bank;

  cond_c = (op1 >> 2) & 1;

  if ((! cond_c) && * carry_known_clear)
    uncond = true;

  *flow_type = uncond ? flow_uncond_branch : flow_cond_branch;

  if (! uncond)
    buf_printf (& buf, & len, "?%s ", cond_c ? "c " : "nc");
  buf_printf (& buf, & len, "goto %%s");

  return true;
}


static bool nut_disassemble_long_branch (uint32_t flags,
					 rom_word_t op1,
					 rom_word_t op2,
					 rom_word_t op3,
					 addr_t *addr, 
					 bank_t *bank,
					 bool *carry_known_clear,
					 flow_type_t *flow_type,
					 bank_t *target_bank,
					 addr_t *target_addr,
					 char *buf,
					 int len)
{
  bool cond_c, call, uncond;
  bool special_41 = false;
  bool set_hex = false;
  int special_page = -1;

  *target_addr = (op1 >> 2) | ((op2 & 0x3fc) << 6);
  *target_bank = *bank;

  cond_c = op2 & 0x001;
  call = ! (op2 & 0x002);

  uncond = (! cond_c) && (* carry_known_clear);

  if (call && (flags & DIS_FLAG_NUT_41_JUMPS))
    {
      special_41 = true;
      switch (*target_addr)
	{
	case 0x0fd9:
	  set_hex = true;
	  /* fall through */
	case 0x0fda:
	  *target_addr = (*addr & 0xfc00) + op3;
	  call = false;
	  break;
	case 0x0fdd:
	  set_hex = true;
	  /* fall through */
	case 0x0fde:
	  *target_addr = (*addr & 0xfc00) + op3;
	  call = true;
	  break;
	case 0x23d0:
	  *target_addr = (*addr & 0xf000) + 0x0000 + op3;
	  special_page = 0;
	  call = false;
	  break;
	case 0x23d2:
	  *target_addr = (*addr & 0xf000) + 0x0000 + op3;
	  special_page = 0;
	  call = true;
	  break;
	case 0x23d9:
	  *target_addr = (*addr & 0xf000) + 0x0400 + op3;
	  special_page = 1;
	  call = false;
	  break;
	case 0x23db:
	  *target_addr = (*addr & 0xf000) + 0x0400 + op3;
	  special_page = 1;
	  call = true;
	  break;
	case 0x23e2:
	  *target_addr = (*addr & 0xf000) + 0x0800 + op3;
	  special_page = 2;
	  call = false;
	  break;
	case 0x23e4:
	  *target_addr = (*addr & 0xf000) + 0x0800 + op3;
	  special_page = 2;
	  call = true;
	  break;
	case 0x23eb:
	  *target_addr = (*addr & 0xf000) + 0x0c00 + op3;
	  special_page = 3;
	  call = false;
	  break;
	case 0x23ed:
	  *target_addr = (*addr & 0xf000) + 0x0c00 + op3;
	  special_page = 3;
	  call = true;
	  break;
	default:
	  special_41 = false;
	}
    }

  if (call)
    *flow_type = flow_subroutine_call;
  else
    *flow_type = uncond ? flow_uncond_branch : flow_cond_branch;

  if (special_41)
    {
      if (special_page >= 0)
	buf_printf (& buf, & len, "x%s%d %%s", call ? "gosub" : "golong", special_page);
      else if (set_hex)
        buf_printf (& buf, & len, "x%s %%s", call ? "gosubh" : "golongh");
      else
        buf_printf (& buf, & len, "x%s %%s", call ? "gosub" : "golong");
    }
  else
    {
      if (! uncond)
	buf_printf (& buf, & len, "?%s ", cond_c ? "c " : "nc");
      buf_printf (& buf, & len, "%s %%s", call ? "gosub" : "golong");
    }

  return true;
}


typedef struct
{
  char *mnem;
  bool can_set_carry;
  flow_type_t flow_type;
} inst_info_t;


static char *nut_op00 [16] =
  { 
    /* 0x000 */ "nop",
    /* 0x040 */ "wrrom", 
    /* 0x080 */ NULL,
    /* 0x0c0 */ NULL,
    /* 0x100 */ "enbank 1",
    /* 0x140 */ "enbank 3",
    /* 0x180 */ "enbank 2",
    /* 0x1c0 */ "enbank 4",
    /* 0x200 */ "wr pil0",
    /* 0x240 */ "wr pil1",
    /* 0x280 */ "wr pil2",
    /* 0x2c0 */ "wr pil3",
    /* 0x300 */ "wr pil4",
    /* 0x340 */ "wr pil5",
    /* 0x380 */ "wr pil6",
    /* 0x3c0 */ "wr pil7",
  };


static char *nut_op18 [16] =
  { 
    /* 0x018 */ NULL,
    /* 0x058 */ "g=c",
    /* 0x098 */ "c=g", 
    /* 0x0d8 */ "c<>g",
    /* 0x118 */ NULL,
    /* 0x158 */ "m=c",
    /* 0x198 */ "c=m",
    /* 0x1d8 */ "c<>m",
    /* 0x218 */ NULL,
    /* 0x258 */ "f=sb",
    /* 0x298 */ "sb=f",
    /* 0x2d8 */ "f<>sb",
    /* 0x318 */ NULL,
    /* 0x358 */ "st=c",
    /* 0x398 */ "c=st",
    /* 0x3d8 */ "c<>st"
  };


static inst_info_t nut_op20 [16] =
  { 
    { /* 0x020 */ "pop stk",     false, flow_no_branch },
    { /* 0x060 */ "powoff",      false, flow_no_branch },
    { /* 0x0a0 */ "sel p",       false, flow_no_branch },
    { /* 0x0e0 */ "sel q",       false, flow_no_branch },
    { /* 0x120 */ "? p=q",       true,  flow_no_branch  },
    { /* 0x160 */ "lld",         true,  flow_no_branch  },
    { /* 0x1a0 */ "clr abc",     false, flow_no_branch },
    { /* 0x1e0 */ "goto c",      false, flow_no_branch },
    { /* 0x220 */ "c=keys",      false, flow_no_branch },
    { /* 0x260 */ "set hex",     false, flow_no_branch },
    { /* 0x2a0 */ "set dec",     false, flow_no_branch },
    { /* 0x2e0 */ "disp off",    false, flow_no_branch },
    { /* 0x320 */ "disp toggle", false, flow_no_branch },
    { /* 0x360 */ "?c rtn",      false, flow_cond_subroutine_return },
    { /* 0x3a0 */ "?nc rtn",     false, flow_cond_subroutine_return },
    { /* 0x3e0 */ "rtn",         false, flow_subroutine_return }
  };


static char *nut_op30 [16] =
  { 
    /* 0x030 */ "disp blink",  // voyager only, HEPAX uses it to relocate ROM
    /* 0x070 */ "n=c",
    /* 0x0b0 */ "c=n", 
    /* 0x0f0 */ "c<>n",
    /* 0x130 */ "ldi",  /* handled elsewhere */
    /* 0x170 */ "stk=c",
    /* 0x1b0 */ "c=stk",
    /* 0x1f0 */ "wptog",  // HEPAX write protect toggle
    /* 0x230 */ "goto keys",
    /* 0x270 */ "sel ram",
    /* 0x2b0 */ "clr regs",
    /* 0x2f0 */ "data=c",
    /* 0x330 */ "cxisa",
    /* 0x370 */ "c=c|a",
    /* 0x3b0 */ "c=c&a",
    /* 0x3f0 */ "sel pfad"
  };


/* map from high opcode bits to register index */
static int tmap [16] =
{ 3, 4, 5, 10, 8, 6, 11, -1, 2, 9, 7, 13, 1, 12, 0, -1 };


static int nut_disassemble_misc (int op1,
				 int op2,
				 inst_state_t *inst_state,
				 bool *new_carry_known_clear,
				 flow_type_t *flow_type,
				 char *buf,
				 int len)
{
  int arg = op1 >> 6;

  switch (op1 & 0x03c)
    {
    case 0x000:
      if (nut_op00 [op1 >> 6])
	buf_printf (& buf, & len, "%s", nut_op00 [op1 >> 6]);
      else
	buf_printf (& buf, & len, "con $%03x", op1);
      break;
    case 0x004:
      if (op1 == 0x3c4)
	buf_printf (& buf, & len, "clr s");
      else
	buf_printf (& buf, & len, "s=0 %d", tmap [arg]);
      break;
    case 0x008:
      if (op1 == 0x3c8)
	buf_printf (& buf, & len, "clr kb");
      else
	buf_printf (& buf, & len, "s=1 %d", tmap [arg]);
      break;
    case 0x00c:
      if (op1 == 0x3cc)
	buf_printf (& buf, & len, "? kb");
      else
	buf_printf (& buf, & len, "? s=1 %d", tmap [arg]);
      *new_carry_known_clear = false;
      break;
    case 0x010:
      buf_printf (& buf, & len, "lc %d", arg);
      break;
    case 0x014:
      if (op1 == 0x3d4)
	buf_printf (& buf, & len, "dec pt");
      else
	{
	  buf_printf (& buf, & len, "? pt= %d", tmap [arg]);
	  *new_carry_known_clear = false;
	}
      break;
    case 0x018:
      if (nut_op18 [op1 >> 6])
	buf_printf (& buf, & len, "%s", nut_op18 [op1 >> 6]);
      else
	buf_printf (& buf, & len, "con $%03x", op1);
      break;
    case 0x01c:
      if (op1 == 0x3dc)
	buf_printf (& buf, & len, "inc pt");
      else
	{
	  buf_printf (& buf, & len, "pt= %d", tmap [arg]);
	  *new_carry_known_clear = true;
	}
      break;
    case 0x020:
      buf_printf (& buf, & len, "%s", nut_op20 [op1 >> 6].mnem);
      if (nut_op20 [op1 >> 6].can_set_carry)
	*new_carry_known_clear = false;
      *flow_type = nut_op20 [op1 >> 6].flow_type;
      break;
    case 0x024:
      buf_printf (& buf, & len, "selpf %d", arg);
      *inst_state = inst_nut_selpf;
      break;
    case 0x028:
      buf_printf (& buf, & len, "reg=c %d", arg);
      break;
    case 0x02c:
      buf_printf (& buf, & len, "? ext %d", tmap [arg]);
      *new_carry_known_clear = false;
      break;
    case 0x030:
      if (op1 == 0x130)
	buf_printf (& buf, & len, "ldi %04o", op2);
      else
	buf_printf (& buf, & len, "%s", nut_op30 [op1 >> 6]);
      break;
    case 0x034:
      buf_printf (& buf, & len, "con $%03x", op1);
      break;
    case 0x038:
      if (arg == 0)
        buf_printf (& buf, & len, "c=data");
      else
        buf_printf (& buf, & len, "c=reg %d", arg);
      break;
    case 0x03c:
      if (op1 == 0x3fc)
	buf_printf (& buf, & len, "disp compensation");
      else
	buf_printf (& buf, & len, "rcr %d", tmap [arg]);
      break;
    }

  return true;
}
 

static inst_info_t nut_arith_info [32] =
  {
    { "a=0",     false, flow_no_branch },
    { "b=0",     false, flow_no_branch },
    { "c=0",     false, flow_no_branch },
    { "ab ex",   false, flow_no_branch },
    { "b=a",     false, flow_no_branch },
    { "ac ex",   false, flow_no_branch },
    { "c=b",     false, flow_no_branch },
    { "bc ex",   false, flow_no_branch },
    { "a=c",     false, flow_no_branch },
    { "a=a+b",   true,  flow_no_branch },
    { "a=a+c",   true,  flow_no_branch },
    { "a=a+1",   true,  flow_no_branch },
    { "a=a-b",   true,  flow_no_branch },
    { "a=a-1",   true,  flow_no_branch },
    { "a=a-c",   true,  flow_no_branch },
    { "c=c+c",   true,  flow_no_branch },
    { "c=a+c",   true,  flow_no_branch },
    { "c=c+1",   true,  flow_no_branch },
    { "c=a-c",   true,  flow_no_branch },
    { "c=c-1",   true,  flow_no_branch },
    { "c=-c",    true,  flow_no_branch },
    { "c=-c-1",  true,  flow_no_branch },
    { "? b<>0",  true,  flow_no_branch },
    { "? c#0",   true,  flow_no_branch },
    { "? a<c",   true,  flow_no_branch },
    { "? a<b",   true,  flow_no_branch },
    { "? a#0",   true,  flow_no_branch },
    { "? a#c",   true,  flow_no_branch },
    { "a sr",    false, flow_no_branch },
    { "b sr",    false, flow_no_branch },
    { "c sr",    false, flow_no_branch },
    { "a sl",    false, flow_no_branch }
  };

static char *nut_field_mnem [8] =
  { "p", "x", "wp", "w", "pq", "xs", "m", "s" };


static int nut_disassemble_arith (int op1,
				  bool *new_carry_known_clear,
				  char *buf,
				  int len)
{
  int op = op1 >> 5;
  int field = (op1 >> 2) & 7;

  buf_printf (& buf, & len, "%-8s%s",
	     nut_arith_info [op].mnem,
	     nut_field_mnem [field]);
  if (nut_arith_info [op].can_set_carry)
    *new_carry_known_clear = false;
  return true;
}


static bool nut_two_word_instruction (rom_word_t op1)
{
  bool two_word = false;

  switch (op1 & 3)
    {
    case 0:  // misc
      two_word = (op1 == 0x130);       // ldi
      break;
    case 1:  two_word = true; break;   // long branch
    case 2:  two_word = false; break;  // arith
    case 3:  two_word = false; break;  // short branch
    }

  return two_word;
}


static bool nut_three_word_instruction (rom_word_t op1, rom_word_t op2)
{
  addr_t target;

  if ((op1 & 3) != 1)
    return false;
  if ((op2 & 2) != 0)
    return false;
  target = (op1 >> 2) | ((op2 & 0x3fc) << 6);
  switch (target)
    {
    case 0x0fd9:
    case 0x0fda:
    case 0x0fdd:
    case 0x0fde:
    case 0x23d0:
    case 0x23d2:
    case 0x23d9:
    case 0x23db:
    case 0x23e2:
    case 0x23e4:
    case 0x23eb:
    case 0x23ed:
      return true;
    }
  return false;
}


bool nut_disassemble (sim_t        *sim,
		      uint32_t     flags,
		      // input and output:
		      bank_t       *bank,
		      addr_t       *addr,
		      inst_state_t *inst_state,
		      bool         *carry_known_clear,
		      addr_t       *delayed_select_mask UNUSED,
		      addr_t       *delayed_select_addr UNUSED,
		      // output:
		      flow_type_t  *flow_type,
		      bank_t       *target_bank,
		      addr_t       *target_addr,
		      char         *buf,
		      int          len)
{
  (void)delayed_select_mask;
  (void)delayed_select_addr;
  bool new_carry_known_clear = true;
  bool status;
  bool two_word = false;
  bool three_word = false;
  rom_word_t op1;
  rom_word_t op2 = 0;
  rom_word_t op3 = 0;
  addr_t base_addr = *addr;

  if (((*inst_state) != inst_normal) &&
      ((*inst_state) != inst_nut_selpf))
    return false;

  *flow_type = flow_no_branch;

  if (! sim_read_rom (sim, *bank, *addr, & op1))
    return false;
  (*addr) = ((*addr) + 1) & 0xffff;

  if (*inst_state == inst_normal)
  {
    two_word = nut_two_word_instruction (op1);
    if (two_word)
    {
      if (! sim_read_rom (sim, *bank, *addr, & op2))
	return false;
      (*addr) = ((*addr) + 1) & 0xffff;
    }

    three_word = (two_word && (flags & DIS_FLAG_NUT_41_JUMPS) && nut_three_word_instruction (op1, op2));
    if (three_word)
    {
      if (! sim_read_rom (sim, *bank, *addr, & op3))
	return false;
      (*addr) = ((*addr) + 1) & 0xffff;
    }
  }

  if (flags & DIS_FLAG_LISTING)
    {
      buf_printf (& buf, & len, "%04x: ", base_addr);
      if (three_word)
	buf_printf (& buf, & len, "%03x %03x %03x  ", op1, op2, op3);
      else if (two_word)
	buf_printf (& buf, & len, "%03x %03x      ", op1, op2);
      else
	buf_printf (& buf, & len, "%03x          ", op1);
    }

  if (flags & DIS_FLAG_LABEL)
    {
      buf_printf (& buf, & len, "<label>  ");
    }

  if (*inst_state == inst_nut_selpf)
  {
    status = nut_disassemble_selpf(op1,
				   inst_state,
				   & new_carry_known_clear,
				   buf,
				   len);
  }
  else
  {
    switch (op1 & 3)
    {
    case 0:
      status = nut_disassemble_misc (op1,
				     op2,
				     inst_state,
				     & new_carry_known_clear,
				     flow_type,
				     buf,
				     len);
      break;
    case 1:
      status = nut_disassemble_long_branch (flags,
					    op1,
					    op2,
					    op3,
					    addr,
					    bank,
					    carry_known_clear,
					    flow_type,
					    target_bank,
					    target_addr,
					    buf,
					    len);
      break;
    case 2:
      status = nut_disassemble_arith (op1,
				      & new_carry_known_clear,
				      buf,
				      len);
      break;
    case 3:
      status = nut_disassemble_short_branch (op1,
					     bank,
					     addr,
					     carry_known_clear,
					     flow_type,
					     target_bank,
					     target_addr,
					     buf,
					     len);
      break;
    }
  }

  *carry_known_clear = new_carry_known_clear;

  return status;
}


/* The host deliberately does not infer carry from the preceding listing line:
 * a branch destination can have several predecessors with different carry. */
static int load_rom(const char *path)
{
  FILE *f = fopen(path, "r");
  char line[512], *p, *end;
  unsigned long a, w;
  unsigned lineno = 0, count = 0;
  if (!f) { perror(path); return 0; }
  while (fgets(line, sizeof line, f)) {
    ++lineno;
    if (!strchr(line, '\n') && !feof(f)) goto bad;
    p = line;
    while (isspace((unsigned char)*p)) ++p;
    if (!*p || *p == '#' || *p == ';') continue;
    if (!isxdigit((unsigned char)*p)) goto bad;
    errno = 0; a = strtoul(p, &end, 16);
    if (errno || a > 65535 || *end != ':') goto bad;
    p = end + 1;
    if (!isxdigit((unsigned char)*p)) goto bad;
    errno = 0; w = strtoul(p, &end, 16);
    if (errno || w > 1023) goto bad;
    while (isspace((unsigned char)*end)) ++end;
    if (*end && *end != '#' && *end != ';') goto bad;
    if (rom.present[a]) goto bad;
    rom.present[a] = true; rom.words[a] = (rom_word_t)w; ++count;
  }
  if (ferror(f)) { perror(path); fclose(f); return 0; }
  fclose(f);
  if (!count) { fprintf(stderr, "%s: empty ROM\n", path); return 0; }
  fprintf(stderr, "Loaded %u 10-bit words from %s\n", count, path);
  return 1;
bad:
  fprintf(stderr, "%s:%u: invalid, oversized or duplicate ROM record\n", path, lineno);
  fclose(f); return 0;
}

static const char *annotation(unsigned op, inst_state_t state)
{
  if (state == inst_nut_selpf) return "Selected-peripheral protocol word";
  if ((op & 3) == 1) return "Two-word absolute transfer; condition uses previous carry";
  if ((op & 3) == 3) return "Signed relative transfer from this instruction; previous carry";
  if ((op & 3) == 2) {
    static const char *fields[] = {
      "Field p: digit at active pointer P or Q", "Field x: digits 0..2",
      "Field wp: digits 0..active pointer", "Field w: all digits 0..13",
      "Field pq: P..Q (P>Q: P..13)", "Field xs: digit 2",
      "Field m: digits 3..12", "Field s: digit 13"
    };
    return fields[(op >> 2) & 7];
  }
  switch(op) {
    case 0x130: return "Load next 10-bit word into C[2:0]; mnemonic operand is OCTAL";
    case 0x270: return "Select RAM address from C[2:0] (low 10 bits)";
    case 0x2f0: return "Write C to selected RAM/peripheral data register";
    case 0x038: return "Read selected RAM/peripheral data into C";
    case 0x260: return "Select hexadecimal arithmetic";
    case 0x2a0: return "Select decimal arithmetic";
    case 0x3e0: return "Return via hardware return stack";
    case 0x360: case 0x3a0: return "Conditional return using previous carry";
    case 0x1e0: return "Computed destination from C[6:3]; not resolved statically";
    case 0x230: return "Computed keycode jump; not resolved statically";
    case 0x330: return "Read ROM word addressed by C[6:3] into C[2:0]";
    case 0x220: return "Copy keyboard keycode into C";
    case 0x3c8: return "Reset keyboard flag";
    case 0x3cc: return "Test keyboard flag";
    case 0x060: return "Enter power-off/sleep state";
    case 0x030: return "Voyager LCD blink control";
    case 0x2e0: return "LCD display off";
    case 0x320: return "Toggle LCD display enable";
    case 0x0a0: return "Use P as active digit pointer";
    case 0x0e0: return "Use Q as active digit pointer";
    default: break;
  }
  if ((op & 0x3f) == 0x10) return "Load nibble at active pointer, then decrement pointer";
  if ((op & 0x3f) == 0x24) return "Enter selected-peripheral instruction state";
  return "";
}

int main(int argc, char **argv)
{
  bool every = false;
  const char *input = NULL;
  unsigned a, rows = 0, covered = 0;
  inst_state_t state = inst_normal;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--every-address")) every = true;
    else if (!strcmp(argv[i], "--help")) {
      puts("Usage: hp15c_disasm [--every-address] input.rom > output.lst\n"
           "Default: linear sweep, preserving raw words.\n"
           "--every-address: independent normal-state decode at each stored address.\n"
           "Neither mode proves which words are executable code.");
      return 0;
    } else if (argv[i][0] == '-' || input) {
      fprintf(stderr, "Invalid arguments; use --help\n"); return 2;
    } else input = argv[i];
  }
  if (!input) { fprintf(stderr, "Missing input; use --help\n"); return 2; }
  if (!load_rom(input)) return 1;
  puts("; HP-15C / Nut disassembly, adapted from Nonpareil dis_nut.c\n"
       "; Upstream commit: c347bc1ab20170c253512042f7aac0d952f304ea\n"
       "; Addresses and raw words: HEX. ldi mnemonic operand: OCTAL.\n"
       "; Conditions retained: ?c = previous carry 1; ?nc = previous carry 0.\n"
       "; Instruction-level comments only; data can resemble instructions.\n"
       "; This is a listing, not an assembler input or an executable ROM image.");
  printf("; Mode: %s\n", every ? "independent decode at every address (overlapping)" : "linear sweep (instruction boundaries are provisional)");
  for (a = 0; a < 65536;) {
    addr_t pc = a, target = 0, dummy1 = 0, dummy2 = 0;
    bank_t bank = 0, target_bank = 0;
    bool carry = false;
    flow_type_t flow;
    char buf[256] = "", decoded[320], dest[32], raw[16];
    unsigned op, used;
    inst_state_t before;
    if (!rom.present[a]) { state = inst_normal; ++a; continue; }
    if (every) state = inst_normal;
    before = state; op = rom.words[a];
    /* Unassigned high-bit encodings must not print a fictitious register -1. */
    bool reserved = before == inst_normal && !(op & 3) &&
      (op >> 6) == 7 && ((op & 0x3c) == 4 || (op & 0x3c) == 8 ||
      (op & 0x3c) == 12 || (op & 0x3c) == 20 || (op & 0x3c) == 28 ||
      (op & 0x3c) == 44 || (op & 0x3c) == 60);
    reserved = reserved || (before == inst_normal && op == 0x3ec);
    bool boundary = before == inst_normal && a == 65535 && nut_two_word_instruction(op);
    if (reserved || boundary || !nut_disassemble(&rom, 0, &bank, &pc, &state, &carry,
                       &dummy1, &dummy2, &flow, &target_bank, &target, buf, sizeof buf)) {
      printf("%04X: %03X      con $%03x                 ; %s\n", a, op, op,
        reserved ? "Unassigned encoding" : boundary ? "Continuation wraps address space; raw word retained" : "Missing continuation word; raw word retained");
      state = inst_normal; ++a; ++rows; ++covered; continue;
    }
    used = (pc - a) & 0xffff;
    if (strstr(buf, "%s")) {
      char *slot = strstr(buf, "%s");
      snprintf(dest, sizeof dest, "$%04X", (unsigned)target);
      *slot = '\0'; snprintf(decoded, sizeof decoded, "%s%s%s", buf, dest, slot + 2);
    } else snprintf(decoded, sizeof decoded, "%s", buf);
    if (used == 2) snprintf(raw, sizeof raw, "%03X %03X", op, rom.words[(a+1)&0xffff]);
    else snprintf(raw, sizeof raw, "%03X", op);
    printf("%04X: %-8s %-28s ; %s", a, raw, decoded, annotation(op, before));
    if (before == inst_normal && ((op & 3) == 1 || (op & 3) == 3) && !rom.present[target])
      printf("; target outside supplied ROM");
    putchar('\n');
    ++rows; covered += every ? 1 : used;
    /* Do not wrap the outer sweep at FFFF. */
    a += every ? 1 : used;
  }
  fprintf(stderr, "Emitted %u rows, covering %u stored-word positions\n", rows, covered);
  if (fflush(stdout) == EOF || ferror(stdout)) { perror("output"); return 1; }
  return 0;
}
