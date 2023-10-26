SECTIONS
{
    .data : SUBALIGN(0)
    {
        KEEP (*(SORT(.xfa.*)))
    }
}

INSERT AFTER .text;

SECTIONS
{
    .rodata : SUBALIGN(0)
    {
        KEEP (*(SORT(.roxfa.*)))
    }
}

INSERT AFTER .text;
