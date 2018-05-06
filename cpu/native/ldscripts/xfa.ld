SECTIONS
{
    .xfa     :
    {
        KEEP (*(SORT(.xfa.*)))
    }
}

INSERT AFTER .data;

SECTIONS
{
    .roxfa     :
    {
        KEEP (*(SORT(.roxfa.*)))
    }
}

INSERT AFTER .rodata;
