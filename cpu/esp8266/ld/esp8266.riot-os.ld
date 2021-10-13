/**
 * This linker script is a combined and modified version of esp8266.ld and
 * esp8266.common.ld from ESP8266-RTOS-SDK.
 */

MEMORY
{
  dport0_0_seg : org = 0x3FF00000, len = 0x10
  dram0_0_seg  : org = 0x3FFE8000, len = 0x18000
  iram1_0_seg  : org = 0x40100000, len = 0xC000
  irom0_0_seg  : org = 0x40200010 + 0x10000, len = 0x80000 - 0x10 - 0x10000
  rtc_seg      : org = 0x60001200, len = 0x200
}

PHDRS
{
  dport0_0_phdr PT_LOAD;
  dram0_0_phdr PT_LOAD;
  dram0_0_bss_phdr PT_LOAD;
  iram1_0_phdr PT_LOAD;
  irom0_0_phdr PT_LOAD;
}


/*  Default entry point:  */
ENTRY(call_user_start)
EXTERN(_DebugExceptionVector)
EXTERN(_DoubleExceptionVector)
EXTERN(_KernelExceptionVector)
EXTERN(_NMIExceptionVector)
EXTERN(_UserExceptionVector)
PROVIDE(_memmap_vecbase_reset = 0x40000000);
/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x00000110;
_memmap_cacheattr_wt_base = 0x00000110;
_memmap_cacheattr_bp_base = 0x00000220;
_memmap_cacheattr_unused_mask = 0xFFFFF00F;
_memmap_cacheattr_wb_trapnull = 0x2222211F;
_memmap_cacheattr_wba_trapnull = 0x2222211F;
_memmap_cacheattr_wbna_trapnull = 0x2222211F;
_memmap_cacheattr_wt_trapnull = 0x2222211F;
_memmap_cacheattr_bp_trapnull = 0x2222222F;
_memmap_cacheattr_wb_strict = 0xFFFFF11F;
_memmap_cacheattr_wt_strict = 0xFFFFF11F;
_memmap_cacheattr_bp_strict = 0xFFFFF22F;
_memmap_cacheattr_wb_allvalid = 0x22222112;
_memmap_cacheattr_wt_allvalid = 0x22222112;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

  .dport0.rodata : ALIGN(4)
  {
    _dport0_rodata_start = ABSOLUTE(.);
    *(.dport0.rodata)
    *(.dport.rodata)
    _dport0_rodata_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.literal : ALIGN(4)
  {
    _dport0_literal_start = ABSOLUTE(.);
    *(.dport0.literal)
    *(.dport.literal)
    _dport0_literal_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.data : ALIGN(4)
  {
    _dport0_data_start = ABSOLUTE(.);
    *(.dport0.data)
    *(.dport.data)
    _dport0_data_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  /* RTC memory holds user's data/rodata */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    *(.rtc.data)
    *(.rtc.rodata)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_seg

  .rtc.bss :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *(.rtc.bss)
    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_seg

  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    KEEP (*(SORT(.xfa.*)))
    _data_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .rodata : ALIGN(4)
  {
    _rodata_start = ABSOLUTE(.);
    /* TODO put only necessary .rodata to dram */
    /* *(.rodata .rodata.*) */
    *libc.a:*.o(.rodata.* .rodata)
    *core/*(.rodata.* .rodata)
    *cpu/*(.rodata .rodata.*)
    *libpp.a:(.rodata.* .rodata)
    *liblog.a:(.rodata.* .rodata)

    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE__ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame)
    . = (. + 3) & ~ 3;
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
    KEEP (*(SORT(.roxfa.*)))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS__ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    . = ALIGN(4);       /* this table MUST be 4-byte aligned */
    _bss_table_start = ABSOLUTE(.);
    LONG(_bss_start)
    LONG(_bss_end)
    _bss_table_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .bss ALIGN(8) (NOLOAD) : ALIGN(4)
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
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _sheap = ABSOLUTE(.);
    _heap_start = ABSOLUTE(.);

  } >dram0_0_seg :dram0_0_bss_phdr

  . = 0x3FFFFFF0;
  _heap_top = ABSOLUTE(.);
  _eheap = ABSOLUTE(.);

  .text : ALIGN(4) /* IRAM */
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    LONG(_text_start)
    . = ALIGN(16);
    *(.DebugExceptionVector.text)   /* 0x40100010 */
    . = ALIGN(16);
    *(.NMIExceptionVector.text)     /* 0x40100020 */
    . = ALIGN(16);
    *(.KernelExceptionVector.text)  /* 0x40100030 */
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.UserExceptionVector.text)    /* 0x40100050 */
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.DoubleExceptionVector.text)  /* 0x40100070 */
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN (16);
    *(.UserExceptionTrampoline.text)  /* 0x40100090 */
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)

    /* normal code should be in irom0 */
    /*
    *(.literal .text)
    *core.a:*(.literal .text .literal.* .text.*)
    */
    *gdbstub/*(.literal .text .literal.* .text.*)
    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    /* RIOT-OS compiled source files that use the .iram1.* section names for IRAM
       functions, etc. */
    *(.iram1 .iram1.*)

    /* SDK libraries that expect their .text or .data sections to link to iram */
    /* TODO *libcore.a:(.bss .data .bss.* .data.* COMMON) */
    *esp_idf_spi_flash/spi_flash_raw.o(.literal .text .literal.* .text.*)
    *esp_idf_esp8266/ets_printf.o(.literal .text .literal.* .text.*)
    /*
    *cpu.a:*.o(.literal .text .literal.* .text.*)
    */
    *core/sched.o(.literal .text .literal.* .text.*)
    *esp_wifi/*(.literal .text .literal.* .text.*)
    *freertos/*(.literal .text .literal.* .text.*)
    *periph/*(.literal .text .literal.* .text.*)
    *xtimer/*(.literal .text .literal.* .text.*)

    *libhal.a:clock.o(.literal .text .literal.* .text.*)
    *libhal.a:int_asm--set_intclear.o(.literal .text .literal.* .text.*)
    *libpp.a:esf_buf.o(.literal .text .literal.* .text.*)
    *libpp.a:lmac.o(.literal .text .literal.* .text.*)
    *libpp.a:pp.o(.literal .text .literal.* .text.*)
    *libpp.a:rate_control.o(.literal .text .literal.* .text.*)
    *libpp.a:trc.o(.literal .text .literal.* .text.*)
    *libpp.a:wdev.o(.literal .text .literal.* .text.*)
    *libphy.a:phy.o(.literal .text .literal.* .text.*)
    *libphy.a:phy_chip_v6_cal.o(.literal .text .literal.* .text.*)
    *libphy.a:phy_sleep.o(.literal .text .literal.* .text.*)

    /* Xtensa basic functionality written in assembler should be placed in iram */
    *xtensa/*(.literal .text .literal.* .text.*)

    /* libgcc functions required for debugging have to be in IRAM */
    *libgcc.a:unwind-dw2.o(.literal .text .literal.* .text.*)

    /* libgcc integer functions also need to be in .text */
    /* some are called before flash is mapped and also for performance) */
    *libgcc.a:*i3.o(.literal .text .literal.* .text.*)

    *libgcc.a:*mulsf3.o(.literal .text .literal.* .text.*)
    *libgcc.a:*divsf3.o(.literal .text .literal.* .text.*)
    *libgcc.a:*fixsfsi.o(.literal .text .literal.* .text.*)

    /* libc also in IRAM */
    /*
    *libc.a:*malloc.o(.literal .text .literal.* .text.*)
    *libc.a:*mallocr.o(.literal .text .literal.* .text.*)
    *libc.a:*freer.o(.literal .text .literal.* .text.*)
    *libc.a:*memcpy.o(.literal .text .literal.* .text.*)
    *libc.a:*memchr.o(.literal .text .literal.* .text.*)
    *libc.a:*memset.o(.literal .text .literal.* .text.*)
    *libc.a:*memcmp.o(.literal .text .literal.* .text.*)
    *libc.a:*memmove.o(.literal .text .literal.* .text.*)
    *libc.a:*rand.o(.literal .text .literal.* .text.*)
    *libc.a:*bzero.o(.literal .text .literal.* .text.*)
    *libc.a:*lock.o(.literal .text .literal.* .text.*)

    *libc.a:*printf.o(.literal .text .literal.* .text.*)
    *libc.a:*findfp.o(.literal .text .literal.* .text.*)
    *libc.a:*fputwc.o(.literal .text .literal.* .text.*)
    */

    *enc28j60/*(.literal .text .literal.* .text.*)

    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >iram1_0_seg :iram1_0_phdr

  .irom0.text : ALIGN(4)
  {
    _irom0_text_start = ABSOLUTE(.);

    /* RIOT-OS compiled code and RO data go into IROM by default */
    *(.literal .text .literal.* .text.*)
    *(.rodata .rodata.*)

    /* Anything explicitly marked as "irom" or "irom0" should go here */
    *(.irom0.literal .irom.literal .irom.text.literal .irom0.text .irom.text)

    . = ALIGN(16);
    __start_ksymatabesp_socket = .;
    *(ksymatabesp_socket)
    __stop_ksymatabesp_socket = .;

    _irom0_text_end = ABSOLUTE(.);
  } >irom0_0_seg :irom0_0_phdr

  .lit4 : ALIGN(4)
  {
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
  } >iram1_0_seg :iram1_0_phdr
}
