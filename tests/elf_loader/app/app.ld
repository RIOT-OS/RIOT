/* Section Definitions */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Section Definitions */
SECTIONS
{
    . = 0x0 + SIZEOF_HEADERS;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rel.plt : { *(.rel.plt) }
}
