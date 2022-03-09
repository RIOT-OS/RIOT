SECTIONS
{
    .data :
    {
        KEEP (*(SORT(.xfa.*)))
    }
}

INSERT AFTER .text;

SECTIONS
{
    .rodata :
    {
        KEEP (*(SORT(.roxfa.*)))
    }
}

INSERT AFTER .text;
