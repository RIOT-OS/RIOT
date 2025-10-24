/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_phydat
 * @{
 *
 * @file
 * @brief       String helper functions for formatting and dumping phydat data
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "assert.h"
#include "flash_utils.h"
#include "fmt.h"
#include "phydat.h"

void phydat_dump(phydat_t *data, uint8_t dim)
{
    if (data == NULL || dim > PHYDAT_DIM) {
        printf("Unable to display data object\n");
        return;
    }
    printf("Data:");

    if (data->unit == UNIT_TIME) {
        assert(dim == 3);
        printf("\t%02d:%02d:%02d\n",
               data->val[2], data->val[1], data->val[0]);
        return;
    }
    if (data->unit == UNIT_DATE) {
        assert(dim == 3);
        printf("\t%04d-%02d-%02d\n",
               data->val[2], data->val[1], data->val[0]);
        return;
    }

    for (uint8_t i = 0; i < dim; i++) {
        char scale_prefix;

        switch (data->unit) {
        case UNIT_UNDEF:
        case UNIT_NONE:
        case UNIT_M2:
        case UNIT_M3:
        case UNIT_PERCENT:
        case UNIT_TEMP_C:
        case UNIT_TEMP_F:
        case UNIT_DBM:
            /* no string conversion */
            scale_prefix = '\0';
            break;
        default:
            scale_prefix = phydat_prefix_from_scale(data->scale);
        }

        printf("\t");
        if (dim > 1) {
            printf("[%u] ", (unsigned int)i);
        }
        else {
            printf("    ");
        }
        if (scale_prefix) {
            printf("%11d %c", (int)data->val[i], scale_prefix);
        }
        else if (data->scale == 0) {
            printf("%11d ", (int)data->val[i]);
        }
        else if ((data->scale > -6) && (data->scale < 0)) {
            char num[9];
            size_t len = fmt_s16_dfp(num, data->val[i], data->scale);
            assert(len < 9);
            num[len] = '\0';
            printf("%11s ", num);
        }
        else {
            char num[12];
            snprintf(num, sizeof(num), "%ie%i",
                     (int)data->val[i], (int)data->scale);
            printf("%11s ", num);
        }

        if ((data->unit != UNIT_NONE) && (data->unit != UNIT_UNDEF)
            && (data->unit != UNIT_BOOL)) {
            phydat_unit_print(data->unit);
        }
        puts("");
    }
}

static FLASH_ATTR const char _unit_celsius[] = "°C";
static FLASH_ATTR const char _unit_fahrenheit[] = "°F";
static FLASH_ATTR const char _unit_kelvin[] = "K";
static FLASH_ATTR const char _unit_lux[] = "lx";
static FLASH_ATTR const char _unit_metre[] = "m";
static FLASH_ATTR const char _unit_square_metre[] = "m^2";
static FLASH_ATTR const char _unit_cubic_metre[] = "m^3";
static FLASH_ATTR const char _unit_g_force[] =  "gₙ";
static FLASH_ATTR const char _unit_degree_per_second[] = "dps";
static FLASH_ATTR const char _unit_revolutions_per_minute[] = "RPM";
static FLASH_ATTR const char _unit_gram[] = "g";
static FLASH_ATTR const char _unit_ampere[] = "A";
static FLASH_ATTR const char _unit_volt[] = "V";
static FLASH_ATTR const char _unit_watt[] = "W";
static FLASH_ATTR const char _unit_decibel_milliwatts[] = "dBm";
static FLASH_ATTR const char _unit_gauss[] = "Gs";
static FLASH_ATTR const char _unit_tesla[] = "T";
static FLASH_ATTR const char _unit_bar[] = "Bar";
static FLASH_ATTR const char _unit_pascal[] = "Pa";
static FLASH_ATTR const char _unit_permille[] = "permille";
static FLASH_ATTR const char _unit_parts_per_million[] = "ppm";
static FLASH_ATTR const char _unit_parts_per_billion[] = "ppb";
static FLASH_ATTR const char _unit_candela[] = "cd";
static FLASH_ATTR const char _unit_percent[] = "%";
static FLASH_ATTR const char _unit_counts[] = "cts";
static FLASH_ATTR const char _unit_coulomb[] = "C";
static FLASH_ATTR const char _unit_gram_per_cubic_metre[] = "g/m^3";
static FLASH_ATTR const char _unit_farad[] = "F";
static FLASH_ATTR const char _unit_potential_of_hydrogen[] = "pH";
static FLASH_ATTR const char _unit_count_per_cubic_metre[] = "#/m^3";
static FLASH_ATTR const char _unit_ohm[] = "ohm";
static FLASH_ATTR const char _unit_undefined[] = "undefined";
static FLASH_ATTR const char _unit_none[] = "none";
static FLASH_ATTR const char _unit_time[] = "time";
static FLASH_ATTR const char _unit_date[] = "date";

static FLASH_ATTR const char * FLASH_ATTR const _unit_to_str[] = {
    [UNIT_TEMP_C] = _unit_celsius,
    [UNIT_TEMP_F] = _unit_fahrenheit,
    [UNIT_TEMP_K] = _unit_kelvin,
    [UNIT_LUX] = _unit_lux,
    [UNIT_M] = _unit_metre,
    [UNIT_M2] = _unit_square_metre,
    [UNIT_M3] = _unit_cubic_metre,
    [UNIT_G_FORCE] = _unit_g_force,
    [UNIT_DPS] = _unit_degree_per_second,
    [UNIT_RPM] = _unit_revolutions_per_minute,
    [UNIT_GRAM] = _unit_gram,
    [UNIT_A] = _unit_ampere,
    [UNIT_V] = _unit_volt,
    [UNIT_W] = _unit_watt,
    [UNIT_DBM] = _unit_decibel_milliwatts,
    [UNIT_GS] = _unit_gauss,
    [UNIT_T] = _unit_tesla,
    [UNIT_BAR] = _unit_bar,
    [UNIT_PA] = _unit_pascal,
    [UNIT_PERMILL] = _unit_permille,
    [UNIT_PPM] = _unit_parts_per_million,
    [UNIT_PPB] = _unit_parts_per_billion,
    [UNIT_CD] = _unit_candela,
    [UNIT_PERCENT] = _unit_percent,
    [UNIT_CTS] = _unit_counts,
    [UNIT_COULOMB] = _unit_coulomb,
    [UNIT_GPM3] = _unit_gram_per_cubic_metre,
    [UNIT_F] = _unit_farad,
    [UNIT_PH] = _unit_potential_of_hydrogen,
    [UNIT_CPM3] = _unit_count_per_cubic_metre,
    [UNIT_OHM] = _unit_ohm,
    [UNIT_UNDEF] = _unit_undefined,
    [UNIT_NONE] = _unit_none,
    [UNIT_BOOL] = _unit_none,
    [UNIT_TIME] = _unit_time,
    [UNIT_DATE] = _unit_date,
};

ssize_t phydat_unit_write(char *dest, size_t max_size, uint8_t unit)
{
    if ((unit >= ARRAY_SIZE(_unit_to_str)) || (_unit_to_str[unit]) == NULL) {
        return -EINVAL;
    }
    size_t len = flash_strlen(_unit_to_str[unit]);
    if (dest) {
        if (max_size < len) {
            return -EOVERFLOW;
        }
        flash_memcpy(dest, _unit_to_str[unit], len);
    }

    return len;
}

void phydat_unit_print(uint8_t unit)
{
    if ((unit < ARRAY_SIZE(_unit_to_str)) && (_unit_to_str[unit]) != NULL) {
        flash_print_str(_unit_to_str[unit]);
    }
}

char phydat_prefix_from_scale(int8_t scale)
{
    static FLASH_ATTR const char _prefix[] = {
        'f', '\0', '\0',
        'p', '\0', '\0',
        'n', '\0', '\0',
        'u', '\0', '\0',
        'm', '\0', '\0',
        '\0', '\0', 'h',
        'k', '\0', '\0',
        'M', '\0', '\0',
        'G', '\0', '\0',
        'T', '\0', '\0',
        'P',
    };

    int8_t idx = scale + ARRAY_SIZE(_prefix) / 2;

    if ((idx < 0) || (idx >= (int8_t)ARRAY_SIZE(_prefix))) {
        return '\0';
    }

    return _prefix[idx];
}
