/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "math.h"
#include "senml.h"
#include "senml/phydat.h"

static uint8_t phydat_unit_to_senml_unit(uint8_t unit)
{
    switch (unit) {
    /* Compatible units */
    case UNIT_TEMP_C:  return SENML_UNIT_CELSIUS;
    case UNIT_TEMP_K:  return SENML_UNIT_KELVIN;
    case UNIT_LUX:     return SENML_UNIT_LUX;
    case UNIT_M:       return SENML_UNIT_METER;
    case UNIT_M2:      return SENML_UNIT_SQUARE_METER;
    case UNIT_M3:      return SENML_UNIT_CUBIC_METER;
    case UNIT_GR:      return SENML_UNIT_GRAM;
    case UNIT_A:       return SENML_UNIT_AMPERE;
    case UNIT_V:       return SENML_UNIT_VOLT;
    case UNIT_W:       return SENML_UNIT_WATT;
    case UNIT_COULOMB: return SENML_UNIT_COULOMB;
    case UNIT_F:       return SENML_UNIT_FARAD;
    case UNIT_OHM:     return SENML_UNIT_OHM;
    case UNIT_PH:      return SENML_UNIT_PH;
    case UNIT_PA:      return SENML_UNIT_PASCAL;
    case UNIT_CD:      return SENML_UNIT_CANDELA;

    /* Compatible Secondary units */
    case UNIT_DBM:     return SENML_UNIT_DECIBEL_MILLIWATT;
    case UNIT_PERCENT: return SENML_UNIT_PERCENT;
    case UNIT_PERMILL: return SENML_UNIT_PERMILLE;
    case UNIT_PPM:     return SENML_UNIT_PARTS_PER_MILLION;
    case UNIT_PPB:     return SENML_UNIT_PARTS_PER_BILLION;

    /* Incompatible units */
    case UNIT_TEMP_F:  return SENML_UNIT_NONE;      /* use K or Cel instead */
    case UNIT_GS:      return SENML_UNIT_NONE;      /* use T instead */
    case UNIT_G:       return SENML_UNIT_NONE;      /* use m/s2 instead */
    case UNIT_BAR:     return SENML_UNIT_NONE;      /* use Pa or hPa instead */
    case UNIT_TIME:    return SENML_UNIT_NONE;      /* split into second/minute/hour */
    case UNIT_DATE:    return SENML_UNIT_NONE;      /* split into day/month/year */
    case UNIT_GPM3:    return SENML_UNIT_NONE;      /* use kg/m3 instead */
    case UNIT_DPS:     return SENML_UNIT_NONE;      /* no alternative */
    case UNIT_CPM3:    return SENML_UNIT_NONE;      /* no alternative */
    default:           return SENML_UNIT_NONE;
    }
}

void phydat_time_to_senml(senml_value_t *senml, const phydat_t *phydat)
{
    senml->value = senml_int((int32_t)phydat->val[0]
                             + 60 * (int32_t)phydat->val[1]
                             + 3600 * (int32_t)phydat->val[2]);
    senml->attr.unit = SENML_UNIT_SECOND;
}

/**
 * Offsets of the first day of the month starting with January.
 * Months after February have a negative offset to efficiently handle leap years.
 */
static int16_t month_to_yday[] = { 0,      31, -306, -275, -245, -214,
                                   -184, -153, -122,  -92,  -61,  -31 };

void phydat_date_to_senml(senml_value_t *senml, const phydat_t *phydat)
{
    senml->attr.unit = SENML_UNIT_SECOND;

    /* Zero-indexed month */
    int64_t month = (int64_t)phydat->val[1] - 1;

    /* Get year relative to 1900. */
    /* Add a year for months after Feb to deal with leap years. */
    int64_t year = (int64_t)phydat->val[2] + (month > 1) - 1900;

    /* Calculate days of the year based on the month */
    int64_t day = (int64_t)phydat->val[0] + (int64_t)month_to_yday[month] - 1;

    /* POSIX calculation of a UNIX timestamp. */
    /* See section 4.16 of the The Open Group Base Specifications Issue 7. */
    senml->value = senml_int(day * 86400 + (year - 70) * 31536000 +
                             ((year - 69) / 4) * 86400 -
                             ((year - 1) / 100) * 86400 +
                             ((year + 299) / 400) * 86400);
}

void phydat_to_senml_bool(senml_bool_value_t *senml, const phydat_t *phydat, const uint8_t dim)
{
    senml->value = phydat->val[dim] == 1;
    senml->attr.unit = SENML_UNIT_NONE;
}

void phydat_to_senml_float(senml_value_t *senml, const phydat_t *phydat, const uint8_t dim)
{
    float value = (float)(phydat->val[dim]);

    if (phydat->scale) {
        value *= pow(10, phydat->scale);
    }

    switch (phydat->unit) {
    /* time conversion */
    case UNIT_TIME:
        senml->attr.unit = (dim == 0)
                          ? SENML_UNIT_SECOND
                          : (dim == 1)
                            ? SENML_UNIT_MINUTE
                            : SENML_UNIT_HOUR;
        break;

    /* simple conversions */
    case UNIT_TEMP_F:
        value = (value + 459.67) * (5. / 9.);
        senml->attr.unit = SENML_UNIT_KELVIN;
        break;
    case UNIT_G:
        value *= 9.80665;
        senml->attr.unit = SENML_UNIT_METER_PER_SQUARE_SECOND;
        break;
    case UNIT_BAR:
        value *= 100000;
        senml->attr.unit = SENML_UNIT_PASCAL;
        break;
    case UNIT_GPM3:
        value *= 0.001;
        senml->attr.unit = SENML_UNIT_KILOGRAM_PER_CUBIC_METER;
        break;
    case UNIT_GS:
        value *= 0.0001;
        senml->attr.unit = SENML_UNIT_TESLA;
        break;

    /* compatible (or not converted) */
    default:
        senml->attr.unit = phydat_unit_to_senml_unit(phydat->unit);
        break;
    }

    senml->value = senml_float(value);
}

void phydat_to_senml_decimal(senml_value_t *senml, const phydat_t *phydat, const uint8_t dim)
{
    int32_t m = phydat->val[dim];
    int32_t e = phydat->scale;

    switch (phydat->unit) {
    /* time conversion */
    case UNIT_TIME:
        senml->attr.unit = (dim == 0)
                          ? SENML_UNIT_SECOND
                          : (dim == 1)
                            ? SENML_UNIT_MINUTE
                            : SENML_UNIT_HOUR;
        break;

    /* simple conversions */
    case UNIT_BAR:
        e += 5;
        senml->attr.unit = SENML_UNIT_PASCAL;
        break;
    case UNIT_GPM3:
        e -= 3;
        senml->attr.unit = SENML_UNIT_KILOGRAM_PER_CUBIC_METER;
        break;
    case UNIT_GS:
        e -= 4;
        senml->attr.unit = SENML_UNIT_TESLA;
        break;

    /* compatible, or not converted */
    default:
        senml->attr.unit = phydat_unit_to_senml_unit(phydat->unit);
        break;
    }

    senml->value = senml_decfrac(m, e);
}
