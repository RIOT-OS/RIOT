/* MCU Sepcific Section Definitions */
SECTIONS
{
    .ccfg :
    {
        KEEP(*(.ccfg))
    } > ccfg

    .gpram :
    {
    } > gpram

    .heap_gpram (NOLOAD) : ALIGN(4)
    {
        _sheap1 = . ;
        _eheap1 = ORIGIN(gpram) + LENGTH(gpram);
    } > gpram
}
