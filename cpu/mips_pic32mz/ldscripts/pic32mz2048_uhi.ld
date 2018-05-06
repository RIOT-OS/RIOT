/*
 * A platform and target independent link script to produce UHI
 * compliant binaries with varying levels of system initialization
 * support.
 */

__entry = DEFINED(__reset_vector) ? 0xbfc00000 : _start;
ENTRY(__entry)
OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
GROUP(-lc -luhi -lgcc -lhal)
SEARCH_DIR(.)
__DYNAMIC  =  0;
STARTUP(crt0.o)
/* Force the exception handler to be registered */
EXTERN(__register_excpt_handler)
/* Force the exception handler to be included in the link */
EXTERN(__exception_entry)
/*
 * Require verbose exceptions. This can be changed to pull in
 * __exception_handle_quiet to reduce code size but be less
 * informative
 */
EXTERN(__exception_handle_verbose)
/* Force the interrupt handlers to tbe included in the link */
EXTERN(__isr_vec)
/* Require the UHI getargs support */
EXTERN(__getargs)

/*
 * Set the location of the top of the stack.  A value of 0 means
 * that it will be automatically placed at the highest address
 * available as described by the __memory_* setttings
 */
PROVIDE (__stack = 0);

/* Size of the memory returned by _get_ram_range */
PROVIDE (__memory_size = 512K);

/* Base of the memory returned by _get_ram_range */
PROVIDE (__memory_base = 0x80000000);

/* Stride length for tlb software invalidate for tlbinvf
 * (mipsXXr3+). Some MIPS implementations may layout the sets/ways
 * differently in the index register. Either sets LSB or ways LSB.
 *
 * By setting this to 1 we presume that sets come first. The default boot
 * code will decrement this value from the Number of TLB entries.
 */
PROVIDE (__tlb_stride_length = 1);

/* By default, XPA is not used even if available. To enable XPA,
 * __enable_xpa should be 1.
 */
PROVIDE (__enable_xpa = 0);

/*
 * 0 = Do not use exception handler present in boot for UHI
 * 1 = Use exception handler present in boot for UHI if BEV is 0 at
 *     startup
 * 2 = Always use exception handler present in boot for UHI
 */
PROVIDE (__use_excpt_boot = 0);
/*
 * Include the code to be able to return to boot context.  This is
 * necessary if __use_excpt_boot != 0.
 */
EXTERN (__register_excpt_boot);

ASSERT (DEFINED(__register_excpt_boot) || __use_excpt_boot == 0,
        "Registration for boot context is required for UHI chaining")

/* Control if subnormal floating-point values are flushed to zero in
   hardware.  This applies to both FPU and MSA operations.  */
PROVIDE (__flush_to_zero = 1);

/* Set up the public symbols depending on whether the user has chosen
   quiet or verbose exception handling above */
EXTERN (__exception_handle);
PROVIDE(__exception_handle = (DEFINED(__exception_handle_quiet)
                                      ? __exception_handle_quiet
                                      : __exception_handle_verbose));
PROVIDE(_mips_handle_exception = __exception_handle);

/*
 * Initalize some symbols to be zero so we can reference them in the
 * crt0 without core dumping. These functions are all optional, but
 * we do this so we can have our crt0 always use them if they exist.
 * This is so BSPs work better when using the crt0 installed with gcc.
 * We have to initalize them twice, so we multiple object file
 * formats, as some prepend an underscore.
 */
PROVIDE (hardware_exit_hook = 0);
PROVIDE (hardware_hazard_hook = 0);
PROVIDE (hardware_init_hook = 0);
PROVIDE (software_init_hook = 0);

/* The default base address for application flash code is 0x9D001000 */
PROVIDE (__app_start = 0x9D001000) ;
/* Set default vector spacing to 32 bytes. */
PROVIDE (__isr_vec_space = 32);
/* Leave space for 9 vector entries by default. 8 entry points and one
   fallback handler. */
PROVIDE (__isr_vec_count = 9);
/*
 * The start of boot flash must be set if including boot code.  By default
 * the use of boot code will mean that application code is copied
 * from flash to RAM at runtime before being executed.
 */
PROVIDE (__lower_boot_flash_start = DEFINED(__reset_vector) ? 0xbfc00000 : __app_start);

PROVIDE (__boot_flash1_start = 0xbfc40000);

PROVIDE (__boot_flash2_start = 0xbfc60000);

PROVIDE (__bev_override = 0x9fc00000);

PROVIDE (__flash_vector_start = 0x9D000000);

PROVIDE (__flash_app_start = 0x9D001000);

SECTIONS
{
  /* Start of bootrom */
  .lowerbootflashalias __bev_override : /* Runs uncached (from 0xBfc00000) until I$ is
                               initialized. */
  AT (__lower_boot_flash_start)
  {
    __base = .;

    *(.reset)           /* Reset entry point. */
    *(.boot)            /* Boot code. */
    . = ALIGN(8);

    . = __base + 0xff40; /*Alternate Config bits (lower Alias)*/
    KEEP(*(.adevcfg3_la))
    KEEP(*(.adevcfg2_la))
    KEEP(*(.adevcfg1_la))
    KEEP(*(.adevcfg0_la))
    . = __base + 0xff5c;
    KEEP(*(.adevcp0_la))
    . = __base + 0xff6c;
    KEEP(*(.adevsign_la))

    . = __base + 0xffc0; /*Config bits (lower Alias)*/
    KEEP(*(.devcfg3_la))
    KEEP(*(.devcfg2_la))
    KEEP(*(.devcfg1_la))
    KEEP(*(.devcfg0_la))
    . = __base + 0xffdc;
    KEEP(*(.devcp0_la))
    . = __base + 0xffec;
    KEEP(*(.devsign_la))

    . = __base + 0xfff0;
    KEEP(*(.seq_la))
  } = 0xFFFFFFFF

  /*
   * We only add this block to keep the MPLAB programmer happy
   * It seems to want the config regs values in the non aliased locations
   */
  . = __base + 0x40000 + 0xff40;
  .bootflash1 :
  AT(__boot_flash1_start + 0xff40)
  {
    __altbase = .;

    . = __altbase; /* Alternate Config Bits (boot flash 1) */
    KEEP(*(.adevcfg3_b1))
    KEEP(*(.adevcfg2_b1))
    KEEP(*(.adevcfg1_b1))
    KEEP(*(.adevcfg0_b1))
    . = __altbase + 0x1c;
    KEEP(*(.adevcp0_b1))
    . = __altbase + 0x2c;
    KEEP(*(.adevsign_b1))

    . = __altbase + 0x80;
    KEEP(*(.devcfg3_b1))
    KEEP(*(.devcfg2_b1))
    KEEP(*(.devcfg1_b1))
    KEEP(*(.devcfg0_b1))
    . = __altbase + 0x9c;
    KEEP(*(.devcp0_b1))
    . = __altbase + 0xAc;
    KEEP(*(.devsign_b1))
    . = __altbase + 0xB0;
    KEEP(*(.seq_b1))
   }  = 0xFFFFFFFF

  /*
   * We only add this block to keep the MPLAB programmer happy
   * It seems to want the config regs values in the non aliased locations
   */
  . = __base + 0x60000 + 0xff40;
  .bootflash2 :
  AT(__boot_flash2_start + 0xff40)
  {
    __altbase = .;

    . = __altbase; /* Alternate Config Bits (boot flash 1) */
    KEEP(*(.adevcfg3_b2))
    KEEP(*(.adevcfg2_b2))
    KEEP(*(.adevcfg1_b2))
    KEEP(*(.adevcfg0_b2))
    . = __altbase + 0x1c;
    KEEP(*(.adevcp0_b2))
     . = __altbase + 0x2c;
    KEEP(*(.adevsign_b2))

    . = __altbase + 0x80;
    KEEP(*(.devcfg3_b2))
    KEEP(*(.devcfg2_b2))
    KEEP(*(.devcfg1_b2))
    KEEP(*(.devcfg0_b2))
    . = __altbase + 0x9c;
    KEEP(*(.devcp0_b2))
    . = __altbase + 0xAc;
    KEEP(*(.devsign_b2))
    . = __altbase + 0xB0;
    KEEP(*(.seq_b2))
  } = 0xFFFFFFFF

  /* Start of the application */
  .exception_vector ALIGN(__flash_vector_start, 0x1000) :
  AT (__flash_vector_start)
  {
    PROVIDE (__excpt_ebase = ABSOLUTE(.));
    __base = .;
    KEEP(* (.text.__exception_entry))

    . = __base + 0x200;
    KEEP(* (SORT(.text.__isr_vec*)))
    /* Leave space for all the vector entries */
    . = __base + 0x200 + (__isr_vec_space * __isr_vec_count);
    ASSERT(__isr_vec_space == (DEFINED(__isr_vec_sw0)
                               ? __isr_vec_sw1 - __isr_vec_sw0
                               : __isr_vec_space),
           "Actual ISR vector spacing does not match __isr_vec_space");
    ASSERT(__base + 0x200 == (DEFINED(__isr_vec_sw0)
                              ? __isr_vec_sw0 & 0xfffffffe : __base + 0x200),
           "__isr_vec_sw0 is not placed at EBASE + 0x200");
    . = ALIGN(8);
  } = 0

  . = __flash_app_start;

  .text : {
     _ftext = . ;
    PROVIDE (eprol  =  .);
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    *(.mips16.fn.*)
    *(.mips16.call.*)
  }
  .init : {
    KEEP (*(.init))
  }
  .fini : {
    KEEP (*(.fini))
  }
  .rel.sdata : {
    PROVIDE (__runtime_reloc_start = .);
    *(.rel.sdata)
    PROVIDE (__runtime_reloc_stop = .);
  }
  PROVIDE (etext  =  .);
  _etext  =  .;

  .eh_frame_hdr : { *(.eh_frame_hdr) }
  .eh_frame : { KEEP (*(.eh_frame)) }
  .gcc_except_table : { *(.gcc_except_table*) }
  .jcr : { KEEP (*(.jcr)) }
  .ctors    :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */

    KEEP (*crtbegin.o(.ctors))

    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */

    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  }

  .dtors    :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  }

  . = .;
  .MIPS.abiflags : {
    __MIPS_abiflags_start = .;
    *(.MIPS.abiflags)
    __MIPS_abiflags_end = .;
  }
  .rodata : {
    *(.rdata)
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
  }
  _rom_data_copy = .;

  .data ALIGN(__memory_base + 0x1000, 16) :
  AT (_rom_data_copy)
  {
     _fdata = .;

      *(.data)
      *(.data.*)
      *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    _gp = . + 0x8000;
    __global = _gp;

      *(.lit8)
      *(.lit4)
      *(.sdata)
      *(.sdata.*)
      *(.gnu.linkonce.s.*)
  }
  . = ALIGN(4);
  PROVIDE (edata  =  .);
  _edata  =  .;
  _fbss = .;
  .sbss : {
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
  }
  .bss : {
    _bss_start = . ;
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
  }

  . = ALIGN(4);
  PROVIDE (end = .);
  _end = .;
  /* Now place the data that is only needed within start.S and can be
     overwritten by the heap.  */
  .startdata : {
    *(.startdata)
  }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to
     the beginning of the section so we begin them at 0.  */

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
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .debug_ranges   0 : { *(.debug_ranges) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  /* Special sections generated by gcc */
  /* Newer GNU linkers strip by default */
  .mdebug.abi32            0 : { KEEP(*(.mdebug.abi32)) }
  .mdebug.abiN32           0 : { KEEP(*(.mdebug.abiN32)) }
  .mdebug.abi64            0 : { KEEP(*(.mdebug.abi64)) }
  .mdebug.abiO64           0 : { KEEP(*(.mdebug.abiO64)) }
  .mdebug.eabi32           0 : { KEEP(*(.mdebug.eabi32)) }
  .mdebug.eabi64           0 : { KEEP(*(.mdebug.eabi64)) }
  .gcc_compiled_long32     0 : { KEEP(*(.gcc_compiled_long32)) }
  .gcc_compiled_long64     0 : { KEEP(*(.gcc_compiled_long64)) }
}
