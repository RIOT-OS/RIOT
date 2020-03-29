/*
 * Copyright 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fmt_stdio
 * @{
 *
 * @file
 * @brief       Implementation of the stdio compatibility functions
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#include "fmt.h"
#include "kernel_defines.h"

static int handle_format_error(const char *format)
{
    /* The best for the user would be to just skip that value and continue
     * formatting the rest of the expression. However, as we don't know the size
     * of the variadic argument, we cannot pop it from the argument stack. Thus,
     * we cannot access subsequent arguments and sadly just have to give up
     * here.
     */
    print_str("???");

    if (IS_USED(DEVELHELP)) {
        print_str("\nUnsupported format string was: \"");
        print_str(format);
        print_str("\"\n");
    }

    return 0;
}

int printf_nano(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    /* When devel help is enabled, the format string is printed verbatim when
     * parsing it failed. Thus, we need to back up the original value of format
     */
    const char *format_orig = format;

    while (*format) {
        if (*format == '%') {
            /* Handle format specifier */
            format++;

            /* "%%" just prints a percent char */
            if (*format == '%') {
                print(format, 1);
                continue;
            }

            /* We don't want to support advanced stuff like aligned output
             * (e.g. "%-30s" or "%08x") in a nano version of printf. But at
             * least skipping the alignment specification is trivial and results
             * in content-wise correct output with incorrect alignment, which
             * is better than just giving up.
             */
            if (*format == '-') {
                format++;
            }
            while ((*format >= '0') && (*format <= '9')) {
                format++;
            }
            /* Printing e.g. `short int` is done with `%hd`, `signed char` with
             * `%hhd`. However, the C standard requires integer variables in
             * a variadic function to be promoted to at least `int`. Thus, we
             * can just ignore these type specifiers. This will fall into the
             * common `int` sized format specifier handling code, which is just
             * what we want.
             */
            while (*format == 'h') {
                format++;
            }
            switch (*format) {
                /* Handle formatting strings (%s) */
                case 's':
                    {
                        const char *str = va_arg(args, char *);
                        print_str(str);
                    }
                    break;
                /* Handle formatting `int` sized numbers, either `signed int`
                 * (%d or %i), `unsigned int` (%u) or hexadecimal output of the
                 * binary representation of the `int` sized value.
                 * Note: If `int` is of smaller width than `int32_t` (e.g. on
                 * AVR), it will be implicitly promoted to `int32_t`. So no
                 * casting is needed in any case.
                 */
                case 'i':
                case 'd':
                    {
                        int num = va_arg(args, int);
                        print_s32_dec(num);
                    }
                    break;
                case 'u':
                    {
                        unsigned num = va_arg(args, unsigned);
                        print_u32_dec(num);
                    }
                    break;
                case 'x':
                    {
                        unsigned num = va_arg(args, unsigned);
                        print_u32_hex(num);
                    }
                    break;
                /* Handle formatting pointers (%p) */
                case 'p':
                    {
                        /* `uintptr_t` is an unsigned integer with the same
                         * width as a pointer. Just what we need to get the
                         * numeric value of a pointer.
                         * An `uintptr_t` can be implicitly promoted to
                         * `uint32_t` for platforms (e.g. AVR) where pointers
                         * are 16 bit rather than 32 bit.
                         */
                        uintptr_t ptr = (uintptr_t)va_arg(args, void *);
                        print_u32_hex(ptr);
                    }
                    break;
                /* Handle formatting `long` sized integers such as
                 * `signed long` (%ld or %li), `unsigned long` (%lu), or
                 * hexadecimal representation of the binary contents of a `long`
                 * sized integer (`%lx`).
                 * Note: On all supported platforms `long` is 32 bit wide.
                 */
                case 'l':
                    format++;
                    switch (*format) {
                        case 'i':
                        case 'd':
                            {
                                long num = va_arg(args, long);
                                print_s32_dec(num);
                            }
                            break;
                        case 'u':
                            {
                                unsigned long num = va_arg(args, unsigned long);
                                print_u32_dec(num);
                            }
                            break;
                        case 'x':
                            {
                                unsigned long num = va_arg(args, unsigned long);
                                print_u32_hex(num);
                            }
                            break;
                        default:
                            /* Either illegal or unsupported format specifier */
                            return handle_format_error(format_orig);
                    }
                    break;
                default:
                    /* Either illegal or unsupported format specifier */
                    return handle_format_error(format_orig);
            }
        }
        else {
            print(format, 1);
        }
        format++;
    }

    /* The correct behavior is to return the number of printed chars. But a
     * nano version of printf does not need to provide such exotic features.
     * However, returning the correct type for ABI compatibility is nice.
     */
    return 0;
}
