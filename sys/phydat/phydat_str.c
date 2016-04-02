/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_sensif
 * @{
 *
 * @file
 * @brief       Generic sensor/actuator data handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "fmt.h"
#include "phydat.h"

void phydat_dump(phydat_t *data, uint8_t dim)
{
    if (data == NULL || dim > PHYDAT_DIM) {
        puts("Unable to display data object");
        return;
    }
    printf("Data:");
    for (uint8_t i = 0; i < dim; i++) {
        char scale_str = phydat_scale_to_str(data->scale);

        printf("\t[%i] ", (int)i);

        if (scale_str) {
            printf("%i%c", (int)data->val[i], scale_str);
        }
        else if (data->scale == 0) {
            printf("%i", (int)data->val[i]);
        }
        else if ((data->scale > -5) && (data->scale < 0)) {
            char num[8];
            size_t len = fmt_s16_dfp(num, data->val[i], data->scale * -1);
            num[len] = '\0';
            printf("%s", num);
        }
        else {
            printf("%iE%i", (int)data->val[i], (int)data->scale);
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
        case UNIT_M:        return "m";
        case UNIT_G:        return "g";
        case UNIT_DPS:      return "dps";
        case UNIT_GR:       return "G";
        case UNIT_A:        return "A";
        case UNIT_V:        return "V";
        case UNIT_GS:       return "Gs";
        case UNIT_BAR:      return "Bar";
        case UNIT_PA:       return "Pa";
        case UNIT_CD:       return "cd";
        case UNIT_PERCENT:  return "%";
        default:            return "";
    }
}

char phydat_scale_to_str(int8_t scale)
{
    switch (scale) {
        case -3:    return 'm';
        case -6:    return 'u';
        case -9:    return 'n';
        case -12:   return 'p';
        case -15:   return 'f';
        case 3:     return 'k';
        case 6:     return 'M';
        case 9:     return 'G';
        case 12:    return 'T';
        case 15:    return 'P';
        default:    return '\0';
    }
}
