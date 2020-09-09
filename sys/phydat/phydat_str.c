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

#include <stdio.h>
#include <stdint.h>

#include "assert.h"
#include "fmt.h"
#include "phydat.h"

void phydat_dump(phydat_t *data, uint8_t dim)
{
    if (data == NULL || dim > PHYDAT_DIM) {
        puts("Unable to display data object");
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

        printf("%s\n", phydat_unit_to_str(data->unit));
    }
}

const char *phydat_unit_to_str(uint8_t unit)
{
    switch (unit) {
        case UNIT_TEMP_C:   return "°C";
        case UNIT_TEMP_F:   return "°F";
        case UNIT_TEMP_K:   return "K";
        case UNIT_LUX:      return "lx";
        case UNIT_M:        return "m";
        case UNIT_M2:       return "m^2";
        case UNIT_M3:       return "m^3";
        case UNIT_G:        return "g";
        case UNIT_DPS:      return "dps";
        case UNIT_GR:       return "G";
        case UNIT_A:        return "A";
        case UNIT_V:        return "V";
        case UNIT_W:        return "W";
        case UNIT_DBM:      return "dBm";
        case UNIT_GS:       return "Gs";
        case UNIT_BAR:      return "Bar";
        case UNIT_PA:       return "Pa";
        case UNIT_PERMILL:  return "permille";
        case UNIT_PPM:      return "ppm";
        case UNIT_PPB:      return "ppb";
        case UNIT_CD:       return "cd";
        case UNIT_PERCENT:  return "%";
        case UNIT_CTS:      return "cts";
        case UNIT_COULOMB:  return "C";
        case UNIT_GPM3:     return "g/m^3";
        case UNIT_F:        return "F";
        case UNIT_PH:       return "pH";
        case UNIT_CPM3:     return "#/m^3";
        case UNIT_OHM:      return "ohm";

        default:            return "";
    }
}

const char *phydat_unit_to_str_verbose(uint8_t unit)
{
    switch (unit) {
        case UNIT_UNDEF:    return "undefined";
        case UNIT_NONE:     /* fall through */
        case UNIT_BOOL:
            return "none";
        case UNIT_TIME:     return "time";
        case UNIT_DATE:     return "date";
        default:            return phydat_unit_to_str(unit);
    }
}

char phydat_prefix_from_scale(int8_t scale)
{
    switch (scale) {
        case -3:    return 'm';
        case -6:    return 'u';
        case -9:    return 'n';
        case -12:   return 'p';
        case -15:   return 'f';
        case 2:     return 'h';
        case 3:     return 'k';
        case 6:     return 'M';
        case 9:     return 'G';
        case 12:    return 'T';
        case 15:    return 'P';
        default:    return '\0';
    }
}
