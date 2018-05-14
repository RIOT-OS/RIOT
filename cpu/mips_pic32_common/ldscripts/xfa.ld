SECTIONS
{
    .data :
    {
        KEEP (*(SORT(.xfa.*)))
    }
    _fdata = ADDR(.data);
    _edata = (_fdata + SIZEOF(.data));
}

INSERT BEFORE .sbss;

SECTIONS
{
    .rodata :
    {
        KEEP (*(SORT(.roxfa.*)))
    }
}

INSERT AFTER .dtors;
