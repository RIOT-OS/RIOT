SECTIONS
{
    .heap2 ALIGN(4) (NOLOAD) :
    {
        _sheap2 = . ;
        _eheap2 = ORIGIN(sram4) + LENGTH(sram4);
    } > sram4
}

