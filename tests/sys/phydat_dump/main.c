/*
 * SPDX-FileCopyrightText: 2020 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Visual test for the conversion from phydat to a string
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#include <stdio.h>

#include "phydat.h"
#include "container.h"

typedef struct {
    phydat_t dat;
    uint8_t dim;
} _phydat_test_t;

_phydat_test_t _test_vector[] =
{
    { .dim = 1, .dat = { .val = { 1234 }, .unit = UNIT_TEMP_C, .scale = -2 } },
    { .dim = 1, .dat = { .val = { 1234 }, .unit = UNIT_TEMP_F, .scale = -2 } },
    { .dim = 1, .dat = { .val = { 1234 }, .unit = UNIT_TEMP_K, .scale = -1 } },
    { .dim = 1, .dat = { .val = { 1234 }, .unit = UNIT_LUX, .scale = -1 } },
    { .dim = 3, .dat = { .val = { 1234, 13456, -14567 }, .unit = UNIT_M, .scale = 0 } },
    { .dim = 3, .dat = { .val = { 1234, 13456, -14567 }, .unit = UNIT_M, .scale = 1 } },
    { .dim = 3, .dat = { .val = { 1234, 13456, -14567 }, .unit = UNIT_M, .scale = 3 } },
    { .dim = 3, .dat = { .val = { 1234, 13456, -14567 }, .unit = UNIT_M, .scale = -2 } },
    { .dim = 3, .dat = { .val = { 1234, 13456, -14567 }, .unit = UNIT_M, .scale = -3 } },
    { .dim = 1, .dat = { .val = { -12345 }, .unit = UNIT_M2, .scale = -5 } },
    { .dim = 1, .dat = { .val = { -12345 }, .unit = UNIT_M3, .scale = -6 } },
    { .dim = 3, .dat = { .val = { 12, 34, 5678}, .unit = UNIT_G_FORCE, .scale = -2 } },
    { .dim = 3, .dat = { .val = { 123, 345, 678 }, .unit = UNIT_DPS, .scale = -3 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_GRAM, .scale = -1 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_A, .scale = 3 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_V, .scale = 6 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_W, .scale = 7 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_GAUSS, .scale = -1 } },
    { .dim = 3, .dat = { .val = { 123, 456, 789 }, .unit = UNIT_T, .scale = -9 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_DBM, .scale = -3 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_COULOMB, .scale = 0 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_F, .scale = -6 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_F, .scale = -7 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PH, .scale = -2 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_BAR, .scale = 0 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PA, .scale = 2 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_CD, .scale = -2 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_CTS, .scale = -3 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PERCENT, .scale = -3 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PERMILL, .scale = -4 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PPM, .scale = -6 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_PPB, .scale = -9 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_GPM3, .scale = -1 } },
    { .dim = 1, .dat = { .val = { 12345 }, .unit = UNIT_CPM3, .scale = -1 } },
    { .dim = 3, .dat = { .val = { 56, 34, 12 }, .unit = UNIT_TIME, .scale = -1 } },
    { .dim = 3, .dat = { .val = { 27, 2, 2020 }, .unit = UNIT_DATE, .scale = -1 } },
};

int main(void)
{
    puts("Visual phydat test application");

    for (unsigned int i = 0; i < ARRAY_SIZE(_test_vector); i++) {
        phydat_dump(&_test_vector[i].dat, _test_vector[i].dim);
    }
    return 0;
}
