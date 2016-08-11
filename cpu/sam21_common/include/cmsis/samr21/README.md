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
