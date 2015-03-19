/* Linker script for ARM M-profile Simulator
 *
 * Version: Sourcery G++ Lite 2010q1-188
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(__cy_reset)
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)


MEMORY
{
  rom (rx) : ORIGIN = 0x0, LENGTH = 262144
  ram (rwx) : ORIGIN = 0x20000000 - (65536 / 2), LENGTH = 65536
}


CY_APPL_ORIGIN      = 0;
CY_FLASH_ROW_SIZE   = 256;
CY_ECC_ROW_SIZE     = 32;
CY_EE_IN_BTLDR      = 0x0;
CY_APPL_LOADABLE    = 0;
CY_EE_SIZE          = 2048;
CY_APPL_NUM         = 1;
CY_APPL_MAX         = 1;
CY_METADATA_SIZE    = 64;


/* These force the linker to search for particular symbols from
 * the start of the link process and thus ensure the user's
 * overrides are picked up
 */
EXTERN(Reset)

/* Bring in interrupt routines & vector */
EXTERN(main)

/* Bring in meta data */
EXTERN(cy_meta_loader cy_bootloader cy_meta_loadable cy_meta_bootloader)
EXTERN(cy_meta_custnvl cy_meta_wolatch cy_meta_flashprotect cy_metadata)

/* Provide fall-back values */
PROVIDE(__cy_heap_start = _end);
PROVIDE(__cy_region_num = (__cy_regions_end - __cy_regions) / 16);
PROVIDE(__cy_stack = ORIGIN(ram) + LENGTH(ram));
PROVIDE(__cy_heap_end = __cy_stack - 0x0800);


SECTIONS
{
  /* The bootloader location */
  .cybootloader 0x0 : { KEEP(*(.cybootloader)) } >rom

  /* Calculate where the loadables should start */
  appl1_start   = CY_APPL_ORIGIN ? CY_APPL_ORIGIN : ALIGN(CY_FLASH_ROW_SIZE);
  appl2_start   = appl1_start + ALIGN((LENGTH(rom) - appl1_start - 2 * CY_FLASH_ROW_SIZE) / 2, CY_FLASH_ROW_SIZE);
  appl_start    = (CY_APPL_NUM == 1) ? appl1_start : appl2_start;
  ecc_offset    = (appl_start / CY_FLASH_ROW_SIZE) * CY_ECC_ROW_SIZE;
  ee_offset     = (CY_APPL_LOADABLE && !CY_EE_IN_BTLDR) ? ((CY_EE_SIZE / CY_APPL_MAX) * (CY_APPL_NUM - 1)) : 0;
  ee_size       = (CY_APPL_LOADABLE && !CY_EE_IN_BTLDR) ? (CY_EE_SIZE / CY_APPL_MAX) : CY_EE_SIZE;
  PROVIDE(CY_ECC_OFFSET = ecc_offset);

  .text appl_start :
  {
    CREATE_OBJECT_SYMBOLS
    PROVIDE(__cy_interrupt_vector = RomVectors);

    *(.romvectors)

    /* Make sure we pulled in an interrupt vector.  */
    ASSERT (. != __cy_interrupt_vector, "No interrupt vector");

    ASSERT (CY_APPL_ORIGIN ? (SIZEOF(.cybootloader) <= CY_APPL_ORIGIN) : 1, "Wrong image location");

    PROVIDE(__cy_reset = Reset);
    *(.text.Reset)
    /* Make sure we pulled in some reset code.  */
    ASSERT (. != __cy_reset, "No reset code");

	/* Place DMA initialization before text to ensure it gets placed in first 64K of flash */
    *(.dma_init)
    ASSERT(appl_start + . <= 0x10000 || !0, "DMA Init must be within the first 64k of flash");

    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer)

    KEEP(*(.bootloader)) /* necessary for bootloader's, but doesn't impact non-bootloaders */

    *(.ARM.extab* .gnu.linkonce.armextab.*)
    *(.gcc_except_table)
  } >rom
  .eh_frame_hdr : ALIGN (4)
  {
    KEEP (*(.eh_frame_hdr))
  } >rom
  .eh_frame : ALIGN (4)
  {
    KEEP (*(.eh_frame))
  } >rom
  /* .ARM.exidx is sorted, so has to go in its own output section.  */
  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } >rom
  __exidx_end = .;
  .rodata : ALIGN (4)
  {
    *(.rodata .rodata.* .gnu.linkonce.r.*)

    . = ALIGN(4);
    KEEP(*(.init))

    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;

    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;

    . = ALIGN(4);
    KEEP(*(.fini))

    . = ALIGN(4);
    __fini_array_start = .;
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    __fini_array_end = .;

    . = ALIGN(0x4);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))

    . = ALIGN(0x4);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))

    . = ALIGN(4);
    __cy_regions = .;
    LONG (__cy_region_init_ram)
    LONG (__cy_region_start_data)
    LONG (__cy_region_init_size_ram)
    LONG (__cy_region_zero_size_ram)
    __cy_regions_end = .;

    . = ALIGN (8);
    _etext = .;
  } >rom

  .ramvectors (NOLOAD) : ALIGN(8)
  {
    __cy_region_start_ram = .;
    KEEP(*(.ramvectors))
  }

  .noinit (NOLOAD) : ALIGN(8)
  {
    KEEP(*(.noinit))
  }

  .data : ALIGN(8)
  {
    __cy_region_start_data = .;

    KEEP(*(.jcr))
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.* .gnu.linkonce.d.*)
    . = ALIGN (8);
    *(.ram)
    _edata = .;
  } >ram AT>rom
  .bss : ALIGN(8)
  {
    PROVIDE(__bss_start__ = .);
    *(.shbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    *(.ram.b)
    _end = .;
    __end = .;
  } >ram AT>rom
  PROVIDE(end = .);
  PROVIDE(__bss_end__ = .);

  __cy_region_init_ram = LOADADDR (.data);
  __cy_region_init_size_ram = _edata - ADDR (.data);
  __cy_region_zero_size_ram = _end - _edata;

  /* The .stack and .heap sections don't contain any symbols.
   * They are only used for linker to calculate RAM utilization.
   */
  .heap (NOLOAD) :
  {
    . = _end;
    . += 0x80;
    __cy_heap_limit = .;
  } >ram

  .stack (__cy_stack - 0x0800) (NOLOAD) :
  {
    __cy_stack_limit = .;
    . += 0x0800;
  } >ram

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__cy_stack_limit >= __cy_heap_limit, "region RAM overflowed with stack")

  .cyloadermeta ((appl_start == 0) ? (LENGTH(rom) - CY_METADATA_SIZE) : 0xF0000000) :
  {
    KEEP(*(.cyloadermeta))
  } :NONE

  .cyloadablemeta (LENGTH(rom) - CY_FLASH_ROW_SIZE * (CY_APPL_NUM - 1) - CY_METADATA_SIZE) :
  {
    KEEP(*(.cyloadablemeta))
  } >rom

  .cyconfigecc (0x80000000 + ecc_offset) :
  {
    KEEP(*(.cyconfigecc))
  } :NONE

  .cycustnvl      0x90000000 : { KEEP(*(.cycustnvl)) } :NONE
  .cywolatch      0x90100000 : { KEEP(*(.cywolatch)) } :NONE

  .cyeeprom (0x90200000 + ee_offset) :
  {
    KEEP(*(.cyeeprom))
    ASSERT(. <= (0x90200000 + ee_offset + ee_size), ".cyeeprom data will not fit in EEPROM");
  } :NONE

  .cyflashprotect 0x90400000 : { KEEP(*(.cyflashprotect)) } :NONE
  .cymeta         0x90500000 : { KEEP(*(.cymeta)) } :NONE

  .stab 0 (NOLOAD) : { *(.stab) }
  .stabstr 0 (NOLOAD) : { *(.stabstr) }
  /* DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the beginning
   * of the section so we begin them at 0.
   */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* DWARF 2.1 */
  .debug_ranges   0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
}

