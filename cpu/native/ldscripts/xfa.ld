SECTIONS
{
      .xfa     :
      {
          KEEP (*(SORT(.xfa.*)))
      }
}

INSERT AFTER .init;
