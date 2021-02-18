SECTIONS
{
    .data :
    {
        /* Special case for AVR (Harvard architecture) where .rodata is merged
         * into .data by the toolchain default ldscripts. */
        KEEP (*(SORT(.roxfa.*)))
        KEEP (*(SORT(.xfa.*)))
    }
    __data_start = ADDR(.data);
    __data_load_start = LOADADDR(.data);
    __data_end = (__data_start + SIZEOF(.data));
    __data_load_end = (__data_load_start + SIZEOF(.data));
}

INSERT AFTER .text;
