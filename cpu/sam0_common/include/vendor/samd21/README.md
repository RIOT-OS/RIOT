# CMSIS from Atmel Software Foundation (ASF)

The include files in the directory tree are copied from ASF.  See
https://spaces.atmel.com/gf/project/asf/frs/?action=FrsReleaseBrowse&frs_package_id=4
(dd. 2016-07-15 ASF version 3.30 was used)

The directory tree was copied "as is" and its structure is as follows:

    cmsis
    └── samd21
        ├── include
        │   ├── component
        │   ├── instance
        │   └── pio
        └── source
            ├── gcc
            └── iar

There is only one include file (per CPU variant) that should be included in
the source code.  For SAMD21 that is cmsis/samd21/include/samd21.h.  But
that will only work if the proper define is set.  The define is named after
the variant, for example `__SAMD21J18A__`.  This define must be set in the
`Makefile.include` of the board.

Be aware that if you want to make changes to any file in this tree that the
changes will be lost when a new ASF release is going to be used.

## Trailing White Space

Because of the whitespace check (dist/tools/whitespacecheck/check.sh) all
the trailing white space had to be removed.  Please take this into account
when comparing to the original ASF distribution.

    find include/ -name '*.h' -exec sed -i 's/\s*$//' '{}' +

## LITTLE_ENDIAN

These include files define `LITTLE_ENDIAN`.  But we think this is wrong.  It
seems more logical to let the compiler decide in which mode the ARM code is
to be translated.  In include/machine/endian.h there is already a define of
`LITTLE_ENDIAN` (and `BIG_ENDIAN`) for a different purpose.

So, we decided to remove the define from the ASF CMSIS files.  The command
for it (running from this directory) is:

    find include/ -name '*.h' -exec sed -i '/^#define\s\s*LITTLE_ENDIAN/d' '{}' +
