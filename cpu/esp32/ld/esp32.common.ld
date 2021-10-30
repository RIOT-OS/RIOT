/*  Default entry point:  */
ENTRY(call_start_cpu0);

SECTIONS
{
  /* RTC fast memory holds RTC wake stub code,
     including from any source file named rtc_wake_stub*.c
  */
  .rtc.text :
  {
    . = ALIGN(4);
    *(.rtc.literal .rtc.text)
    *rtc_wake_stub*.o(.literal .text .literal.* .text.*)
  } >rtc_iram_seg

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    /* part that is initialized if not waking up from deep sleep */
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.o(.bss .bss.*)
    *rtc_wake_stub*.o(COMMON)
    _rtc_bss_end = ABSOLUTE(.);
    /* part that saves some data for rtc periph module, this part is
       only initialized at power on reset */
    _rtc_bss_rtc_start = ABSOLUTE(.);
    *(.rtc.bss .rtc.bss.*)
    _rtc_bss_rtc_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* RTC slow memory holds RTC wake stub
     data/rodata, including from any source file
     named rtc_wake_stub*.c
  */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    *(.rtc.data)
    *(.rtc.rodata)
    *rtc_wake_stub*.o(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* This section holds data that should not be initialized at power up
     and will be retained during deep sleep. The section located in
     RTC SLOW Memory area. User data marked with RTC_NOINIT_ATTR will be placed
     into this section. See the file "esp_attr.h" for more information.
  */
  .rtc.noinit (NOLOAD):
  {
    . = ALIGN(4);
    _rtc_noinit_start = ABSOLUTE(.);
    *(.rtc_noinit .rtc_noinit.*)
    . = ALIGN(4) ;
    _rtc_noinit_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(**(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);

    /* This goes here, not at top of linker script, so addr2line finds it last,
       and uses it in preference to the first symbol in IRAM */
    _iram_start = ABSOLUTE(0);
  } > iram0_0_seg

  /* If Bluetooth is not used, this DRAM section can be used as heap */
  . = _data_start_btdm; /* 0x3ffae6e0 */
  . = ALIGN (4);
  _sheap1 = ABSOLUTE(.);
  . = 0x3ffb0000;
  _eheap1 = ABSOLUTE(.);

  .iram0.text :
  {
    /* Code marked as running out of IRAM */
    _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)

    *libhal.a:(.literal .text .literal.* .text.*)
    *libgcc.a:(.literal .text .literal.* .text.*)
    *libgcov.a:(.literal .text .literal.* .text.*)
    *libc.a:*(.literal .text .literal.* .text.*)

    /* Xtensa basic functionality written in assembler should be placed in iram */
    *xtensa/*(.literal .text .literal.* .text.*)
    /* ESP-IDF parts that have to run in IRAM */
    *esp_idf_heap/*(.literal .text .literal.* .text.*)
    *esp_idf_spi_flash/*(.literal .text .literal.* .text.*)
    /* parts of RIOT that should to run in IRAM */
    *core/*(.literal .text .literal.* .text.*)
    *littlefs/*(.literal .text .literal.* .text.*)
    *littlefs2/*(.literal .text .literal.* .text.*)
    *newlib_syscalls_default/*(.literal .text .literal.* .text.*)
    *spiffs_fs/*(.literal .text .literal.* .text.*)
    *spiffs/*(.literal .text .literal.* .text.*)
    *syscalls.o(.literal .text .literal.* .text.*)
    *vfs/*(.literal .text .literal.* .text.*)

    /* part of the RIOT port that should run in IRAM */
    *cpu/*(.literal .text .literal.* .text.*)
    *esp_common/*(.literal .text .literal.* .text.*)
    *esp_freertos_common/*(.literal .text .literal.* .text.*)
    *periph/*(.literal .text .literal.* .text.*)
    *esp_periph_common/*(.literal .text .literal.* .text.*)
    *mtd/**(.literal .text .literal.* .text.*)

    INCLUDE esp32.spiram.rom-functions-iram.ld
    _iram_text_end = ABSOLUTE(.);

    /* IRAM can't be used as heap since it only allows 32-bit aligned access */
    /*
    . = ALIGN (4);
    _sheap4 = ABSOLUTE(.);
    */
  } > iram0_0_seg

  /* IRAM can't be used as heap since it only allows 32-bit aligned access */
  /*
  . = 0x400a0000;
  _eheap4 = ABSOLUTE(.);
  */

  /* Starts at 0x3ffb000 if Bluetooth is not enabled, 0x3ffc0000 otherwise */
  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    KEEP (*(SORT(.xfa.*)))
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    *(.dram1 .dram1.*)
    *libesp32.a:panic.o(.rodata .rodata.*)
    *libphy.a:(.rodata .rodata.*)
    *libsoc.a:rtc_clk.o(.rodata .rodata.*)
    *libapp_trace.a:(.rodata .rodata.*)
    *libgcov.a:(.rodata .rodata.*)
    *libheap.a:multi_heap.o(.rodata .rodata.*)
    *libheap.a:multi_heap_poisoning.o(.rodata .rodata.*)
    INCLUDE esp32.spiram.rom-functions-dram.ld
    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >dram0_0_seg

  /* This section holds data that should not be initialized at power up.
     The section located in Internal SRAM memory region. The macro _NOINIT
     can be used as attribute to place data into this section.
     See the esp_attr.h file for more information.
  */
  .noinit (NOLOAD):
  {
    . = ALIGN(4);
    _noinit_start = ABSOLUTE(.);
    __noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _noinit_end = ABSOLUTE(.);
    __noinit_end = ABSOLUTE(.);
  } > dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _heap_start = ABSOLUTE(.);
    _sheap = ABSOLUTE(.);
  } >dram0_0_seg

  /* Reserved ROM/ETS data start at 0x3ffe000. */
  . = 0x3ffe0000;
  _heap_top = ABSOLUTE(.);
  _eheap = ABSOLUTE(.);

  /* Reserved ROM/ETS data region for PRO CPU: 0x3ffe0000 ... 0x3ffe440 */
  . = 0x3ffe0440;
  _sheap2 = ABSOLUTE(.);
  . = 0x3ffe4000;
  _eheap2 = ABSOLUTE(.);

  /* Reserved ROM/ETS data region for APP CPU: 0x3ffe4000 ... 0x3ffe4350 */
  . = 0x3ffe4350;
  _sheap3 = ABSOLUTE(.);
  . = 0x40000000;
  _eheap3 = ABSOLUTE(.);

  .flash.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    KEEP (*(SORT(.roxfa.*)))
    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >drom0_0_seg

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    /* place everything else in iram0_2_seg (cached ROM) */
    *(.literal .text .literal.* .text.* .stub)
    *(.gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);
  } >iram0_2_seg
}
