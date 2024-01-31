#include <avr/pgmspace.h>
#include <stdio.h>

/* The outdated linker from Ubuntu's toolchain contains a bug in which it will
 * garbage collect symbols referenced only by --defsym= command line options,
 * and subsequently complain that the symbols are not defined. Adding other
 * references to those symbols from an unused function makes that buggy linker
 * happy. Since this function is never used, it will be garbage collected and
 * not impact the ROM size. */
void work_around_for_shitty_ubuntu_toolchain_and_not_expected_to_be_called(void)
{
    printf_P(NULL);
    fprintf_P(stdout, NULL);
    snprintf_P(NULL, 0, NULL);
}
