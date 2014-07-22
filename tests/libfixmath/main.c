/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     Print some calculations for libfixmath
 *
 * @author    René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "fix16.h"

#ifndef M_PI
#   define M_PI 3.14159265359
#endif

static void binary_ops(void)
{
    static const struct {
        const char *op;
        fix16_t (*fun)(fix16_t, fix16_t);
    } ops[] = {
        { "add", fix16_add },
        { "sub", fix16_sub },
        { "mul", fix16_mul },
        { "div", fix16_div },
        { "mod", fix16_mod },

        { "sadd", fix16_sadd },
        { "ssub", fix16_ssub },
        { "smul", fix16_smul },
        { "sdiv", fix16_sdiv },

        { "min", fix16_min },
        { "max", fix16_max },
    };

    for (fix16_t a = fix16_from_dbl(-5.0); a < fix16_from_dbl(5.0); a += fix16_from_dbl(0.25)) {
        for (fix16_t b = fix16_from_dbl(-5.0); b < fix16_from_dbl(5.0); b += fix16_from_dbl(0.25)) {
            if (b == 0) {
                continue;
            }


            for (unsigned o = 0; o < sizeof(ops) / sizeof(*ops); ++o) {
                fix16_t c = ops[o].fun(a, b);

                char buf[3][14];
                fix16_to_str(a, buf[0], 12);
                fix16_to_str(b, buf[1], 12);
                fix16_to_str(c, buf[2], 12);

                printf("%s(%s, %s) = %s\n", ops[o].op, buf[0], buf[1], buf[2]);
            }
        }
    }
}

static void unary_ops(void)
{
    /* range [-10 : +10 : 0.25] */
    {
        static const struct {
            const char *op;
            fix16_t (*fun)(fix16_t);
        } ops[] = {
            { "abs", fix16_abs },
            { "sq", fix16_sq },

            { "atan", fix16_atan },

            { "exp", fix16_exp },
        };

        for (fix16_t input = fix16_from_dbl(-10.0); input < fix16_from_dbl(+10.0); input += fix16_from_dbl(0.25)) {
            for (unsigned o = 0; o < sizeof(ops) / sizeof(*ops); ++o) {
                fix16_t result = ops[o].fun(input);

                char buf[2][14];
                fix16_to_str(input, buf[0], 12);
                fix16_to_str(result, buf[1], 12);

                printf("%s(%s) = %s\n", ops[o].op, buf[0], buf[1]);
            }
        }
    }

    /* range [-pi/2 : +pi/2 : 0.05] */
    {
        static const struct {
            const char *op;
            fix16_t (*fun)(fix16_t);
        } ops[] = {
            /* { "sin_parabola", fix16_sin_parabola }, FIXME: what is sin_parabola? */
            { "sin", fix16_sin },
            { "cos", fix16_cos },
            { "tan", fix16_tan },
        };

        for (fix16_t input = fix16_from_dbl(-M_PI/2); input < fix16_from_dbl(+M_PI/2); input += fix16_from_dbl(0.05)) {
            for (unsigned o = 0; o < sizeof(ops) / sizeof(*ops); ++o) {
                fix16_t result = ops[o].fun(input);

                char buf[2][14];
                fix16_to_str(input, buf[0], 12);
                fix16_to_str(result, buf[1], 12);

                printf("%s(%s) = %s\n", ops[o].op, buf[0], buf[1]);
            }
        }
    }

    /* range [-1 : +1 : 0.05] */
    {
        static const struct {
            const char *op;
            fix16_t (*fun)(fix16_t);
        } ops[] = {
            { "asin", fix16_asin },
            { "acos", fix16_acos },
        };

        for (fix16_t input = fix16_from_dbl(-1.0); input < fix16_from_dbl(+1.0); input += fix16_from_dbl(0.05)) {
            for (unsigned o = 0; o < sizeof(ops) / sizeof(*ops); ++o) {
                fix16_t result = ops[o].fun(input);

                char buf[2][14];
                fix16_to_str(input, buf[0], 12);
                fix16_to_str(result, buf[1], 12);

                printf("%s(%s) = %s\n", ops[o].op, buf[0], buf[1]);
            }
        }
    }

    /* range [+0.05 : +10 : 0.25] */
    {
        static const struct {
            const char *op;
            fix16_t (*fun)(fix16_t);
        } ops[] = {
            { "sqrt", fix16_sqrt },

            { "log", fix16_log },
            { "log2", fix16_log2 },
            { "slog2", fix16_slog2 },
        };

        for (fix16_t input = fix16_from_dbl(0.05); input < fix16_from_dbl(+10.0); input += fix16_from_dbl(0.25)) {
            for (unsigned o = 0; o < sizeof(ops) / sizeof(*ops); ++o) {
                fix16_t result = ops[o].fun(input);

                char buf[2][14];
                fix16_to_str(input, buf[0], 12);
                fix16_to_str(result, buf[1], 12);

                printf("%s(%s) = %s\n", ops[o].op, buf[0], buf[1]);
            }
        }
    }
}

int main(void)
{
    puts("Unary.");
    unary_ops();

    puts("Binary.");
    binary_ops();

    puts("Done.");
    return 0;
}
