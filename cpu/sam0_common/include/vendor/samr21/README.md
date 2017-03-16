# CMSIS include files from Atmel for SAMR21

It is unclear which version of ASF was used for the first implementation
of samr21 (it was called samd21).

Since then the files has been updated using a distribution from Atmel
Currently we're using sam0/utils/cmsis/samr21 from asf-3.21.0.7z.

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
