/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "senml.h"

const char *senml_unit_to_str(senml_unit_t unit)
{
    switch (unit) {
    case SENML_UNIT_NONE:                         return "";
    case SENML_UNIT_METER:                        return "m";
    case SENML_UNIT_KILOGRAM:                     return "kg";
    case SENML_UNIT_GRAM:                         return "g";
    case SENML_UNIT_SECOND:                       return "s";
    case SENML_UNIT_AMPERE:                       return "A";
    case SENML_UNIT_KELVIN:                       return "K";
    case SENML_UNIT_CANDELA:                      return "cd";
    case SENML_UNIT_MOLE:                         return "mol";
    case SENML_UNIT_HERTZ:                        return "Hz";
    case SENML_UNIT_RADIAN:                       return "rad";
    case SENML_UNIT_STERADIAN:                    return "sr";
    case SENML_UNIT_NEWTON:                       return "N";
    case SENML_UNIT_PASCAL:                       return "Pa";
    case SENML_UNIT_JOULE:                        return "J";
    case SENML_UNIT_WATT:                         return "W";
    case SENML_UNIT_COULOMB:                      return "C";
    case SENML_UNIT_VOLT:                         return "V";
    case SENML_UNIT_FARAD:                        return "F";
    case SENML_UNIT_OHM:                          return "Ohm";
    case SENML_UNIT_SIEMENS:                      return "S";
    case SENML_UNIT_WEBER:                        return "Wb";
    case SENML_UNIT_TESLA:                        return "T";
    case SENML_UNIT_HENRY:                        return "H";
    case SENML_UNIT_CELSIUS:                      return "Cel";
    case SENML_UNIT_LUMEN:                        return "lm";
    case SENML_UNIT_LUX:                          return "lx";
    case SENML_UNIT_BECQUEREL:                    return "Bq";
    case SENML_UNIT_GRAY:                         return "Gy";
    case SENML_UNIT_SIEVERT:                      return "Sv";
    case SENML_UNIT_KATAL:                        return "kat";
    case SENML_UNIT_SQUARE_METER:                 return "m2";
    case SENML_UNIT_CUBIC_METER:                  return "m3";
    case SENML_UNIT_LITER:                        return "l";
    case SENML_UNIT_METER_PER_SECOND:             return "m/s";
    case SENML_UNIT_METER_PER_SQUARE_SECOND:      return "m/s2";
    case SENML_UNIT_CUBIC_METER_PER_SECOND:       return "m3/s";
    case SENML_UNIT_LITER_PER_SECOND:             return "l/s";
    case SENML_UNIT_WATT_PER_SQUARE_METER:        return "W/m2";
    case SENML_UNIT_CANDELA_PER_SQUARE_METER:     return "cd/m2";
    case SENML_UNIT_BIT:                          return "bit";
    case SENML_UNIT_BIT_PER_SECOND:               return "bit/s";
    case SENML_UNIT_LATITUDE:                     return "lat";
    case SENML_UNIT_LONGITUDE:                    return "lon";
    case SENML_UNIT_PH:                           return "pH";
    case SENML_UNIT_DECIBEL:                      return "dB";
    case SENML_UNIT_DBW:                          return "dBW";
    case SENML_UNIT_BEL:                          return "Bspl";
    case SENML_UNIT_COUNT:                        return "count";
    case SENML_UNIT_RATIO:                        return "/";
    case SENML_UNIT_RATIO_2:                      return "%";
    case SENML_UNIT_RELATIVE_HUMIDITY_PERCENT:    return "%RH";
    case SENML_UNIT_REMAINING_BATTERY_PERCENT:    return "%EL";
    case SENML_UNIT_REMAINING_BATTERY_SECONDS:    return "EL";
    case SENML_UNIT_RATE:                         return "1/s";
    case SENML_UNIT_RPM:                          return "1/min";
    case SENML_UNIT_BEAT_PER_MINUTE:              return "beat/min";
    case SENML_UNIT_BEATS:                        return "beats";
    case SENML_UNIT_SIEMENS_PER_METER:            return "S/m";

    case SENML_UNIT_BYTE:                         return "B";
    case SENML_UNIT_VOLT_AMPERE:                  return "VA";
    case SENML_UNIT_VOLT_AMPERE_SECOND:           return "VAs";
    case SENML_UNIT_VOLT_AMPERE_REACTIVE:         return "var";
    case SENML_UNIT_VOLT_AMPERE_REACTIVE_SECOND:  return "vars";
    case SENML_UNIT_JOULE_PER_METER:              return "J/m";
    case SENML_UNIT_KILOGRAM_PER_CUBIC_METER:     return "kg/m3";
    case SENML_UNIT_DEGREE:                       return "deg";

    case SENML_UNIT_NEPHELOMETRIC_TURBIDITY_UNIT: return "NTU";

    case SENML_UNIT_MILLISECOND:                  return "ms";
    case SENML_UNIT_MINUTE:                       return "min";
    case SENML_UNIT_HOUR:                         return "h";
    case SENML_UNIT_MEGAHERTZ:                    return "MHz";
    case SENML_UNIT_KILOWATT:                     return "kW";
    case SENML_UNIT_KILOVOLT_AMPERE:              return "kVA";
    case SENML_UNIT_KILOVAR:                      return "kvar";
    case SENML_UNIT_AMPERE_HOUR:                  return "Ah";
    case SENML_UNIT_WATT_HOUR:                    return "Wh";
    case SENML_UNIT_KILOWATT_HOUR:                return "kWh";
    case SENML_UNIT_VAR_HOUR:                     return "varh";
    case SENML_UNIT_KILOVAR_HOUR:                 return "kvarh";
    case SENML_UNIT_KILOVOLT_AMPERE_HOUR:         return "kVAh";
    case SENML_UNIT_WATT_HOUR_PER_KILOMETER:      return "Wh/km";
    case SENML_UNIT_KIBIBYTE:                     return "KiB";
    case SENML_UNIT_GIGABYTE:                     return "GB";
    case SENML_UNIT_MEGABIT_PER_SECOND:           return "MBit/s";
    case SENML_UNIT_BYTE_PER_SECOND:              return "B/s";
    case SENML_UNIT_MEGABYTE_PER_SECOND:          return "MB/s";
    case SENML_UNIT_MILLIVOLT:                    return "mV";
    case SENML_UNIT_MILLIAMPERE:                  return "mA";
    case SENML_UNIT_DECIBEL_MILLIWATT:            return "dBm";
    case SENML_UNIT_MICROGRAM_PER_CUBIC_METER:    return "ug/m3";
    case SENML_UNIT_MILLIMETER_PER_HOUR:          return "mm/h";
    case SENML_UNIT_METER_PER_HOUR:               return "m/h";
    case SENML_UNIT_PARTS_PER_MILLION:            return "ppm";
    case SENML_UNIT_PERCENT:                      return "/100";
    case SENML_UNIT_PERMILLE:                     return "/1000";
    case SENML_UNIT_HECTOPASCAL:                  return "hPa";
    case SENML_UNIT_MILLIMETER:                   return "mm";
    case SENML_UNIT_CENTIMETER:                   return "cm";
    case SENML_UNIT_KILOMETER:                    return "km";
    case SENML_UNIT_KILOMETER_PER_HOUR:           return "km/h";
    case SENML_UNIT_PARTS_PER_BILLION:            return "ppb";
    case SENML_UNIT_PARTS_PER_TRILLION:           return "ppt";
    case SENML_UNIT_VOLT_AMPERE_HOUR:             return "VAh";
    case SENML_UNIT_MILLIGRAM_PER_LITER:          return "mg/l";
    case SENML_UNIT_MICROGRAM_PER_LITER:          return "ug/l";
    case SENML_UNIT_GRAM_PER_LITER:               return "g/l";

    default:                                      return "";
    }
}
