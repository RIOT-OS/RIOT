SECTIONS
{
    .rodata :
    {
        KEEP (*(SORT(.roxfa.*)))
    } > ROM
}

INSERT AFTER .rodata;

SECTIONS
{
    .data :
    {
        KEEP (*(SORT(.xfa.*)))
    } > RAM AT> ROM

    . = ALIGN(2);
    _edata = .;
    PROVIDE (edata = .);
    PROVIDE (__dataend = .);
}

INSERT AFTER .data;
