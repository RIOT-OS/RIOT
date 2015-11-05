# msp430-elf-gcc riot patch


## stdio with tinyprintf

    > cp tinyprintf.h <GCC_ROOT_DIR>/msp430-elf/include/
    > cd <GCC_ROOT_DIR>/msp430-elf/include/
    > mv stdio.h stdio.h.orig
    > ln -s tinyprintf.h stdio.h

where <GCC_ROOT_DIR> is the msp430 gcc installation root dir, for example /usr/local/gcc-msp430-elf-dev-20141215

## linker script

    > cd <GCC_ROOT_DIR>/msp430-elf/lib
    
Add fini9 section to the linker script (cc430f6137.ld in case of chronos):

	  .text           :
	  {
	    . = ALIGN(2);
	    PROVIDE (_start = .);
	    KEEP (*(SORT(.crt_*)))
	    *(.lowtext .text .stub .text.* .gnu.linkonce.t.* .text:*)
	    KEEP (*(.text.*personality*))
	    /* .gnu.warning sections are handled specially by elf32.em.  */
	    *(.gnu.warning)
	    *(.interp .hash .dynsym .dynstr .gnu.version*)
	    PROVIDE (__etext = .);
	    PROVIDE (_etext = .);
	    PROVIDE (etext = .);
	    . = ALIGN(2);
	    KEEP (*(.init))
	    KEEP (*(.fini))
	    KEEP (*(.fini9))     <-- !!! THIS LINE 
