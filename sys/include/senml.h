/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_senml SenML
 * @ingroup     sys
 * @brief       Basic SenML types.
 *
 * The `senml` module contains the building blocks for using
 * [SenML](https://www.rfc-editor.org/rfc/rfc8428).
 * This module provides the basic types that can be used with (for example)
 * @ref sys_senml_cbor for encoding measurement data.
 *
 * Some attributes defined in SenML need to be enabled explicitly,
 * see @ref senml_attr_t for details. To enable all attributes, set:
 *
 * ```
 * CFLAGS += -DCONFIG_SENML_ATTR_SUM=1
 * CFLAGS += -DCONFIG_SENML_ATTR_VERSION=1
 * CFLAGS += -DCONFIG_SENML_ATTR_UPDATE_TIME=1
 * ```
 *
 * @{
 *
 * @file
 * @brief       Basic SenML types.
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#ifndef SENML_H
#define SENML_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the SenML 'sum' and 'base sum' attributes.
 */
#ifndef CONFIG_SENML_ATTR_SUM
#define CONFIG_SENML_ATTR_SUM 0
#endif

/**
 * @brief Enable the SenML 'version' and 'base version' attributes.
 */
#ifndef CONFIG_SENML_ATTR_VERSION
#define CONFIG_SENML_ATTR_VERSION 0
#endif

/**
 * @brief Enable the SenML 'update time' attribute.
 */
#ifndef CONFIG_SENML_ATTR_UPDATE_TIME
#define CONFIG_SENML_ATTR_UPDATE_TIME 0
#endif

/**
 * @brief   SenML units and secondary units
 *
 * This list contains the SenML units and secondary units as assigned by
 * [IANA](https://www.iana.org/assignments/senml/senml.xhtml).
 * Units in italic are not recommended to be produced by new implementations.
 * Secondary units include the equivalent primary unit.
 */
typedef enum {
    /* SenML units from RFC8428 */
    SENML_UNIT_NONE,                        /**< No unit specified */
    SENML_UNIT_METER,                       /**< meter (m) */
    SENML_UNIT_KILOGRAM,                    /**< kilogram (kg) */
    SENML_UNIT_GRAM,                        /**< *gram* (g) */
    SENML_UNIT_SECOND,                      /**< second (s) */
    SENML_UNIT_AMPERE,                      /**< ampere (A) */
    SENML_UNIT_KELVIN,                      /**< kelvin (K) */
    SENML_UNIT_CANDELA,                     /**< candela (cd) */
    SENML_UNIT_MOLE,                        /**< mole (mol) */
    SENML_UNIT_HERTZ,                       /**< hertz (Hz) */
    SENML_UNIT_RADIAN,                      /**< radian (rad) */
    SENML_UNIT_STERADIAN,                   /**< steradian (sr) */
    SENML_UNIT_NEWTON,                      /**< newton (N) */
    SENML_UNIT_PASCAL,                      /**< pascal (Pa) */
    SENML_UNIT_JOULE,                       /**< joule (J) */
    SENML_UNIT_WATT,                        /**< watt (W) */
    SENML_UNIT_COULOMB,                     /**< coulomb (C) */
    SENML_UNIT_VOLT,                        /**< volt (V) */
    SENML_UNIT_FARAD,                       /**< farad (F) */
    SENML_UNIT_OHM,                         /**< ohm (Ohm) */
    SENML_UNIT_SIEMENS,                     /**< siemens (S) */
    SENML_UNIT_WEBER,                       /**< weber (Wb) */
    SENML_UNIT_TESLA,                       /**< tesla (T) */
    SENML_UNIT_HENRY,                       /**< henry (H) */
    SENML_UNIT_CELSIUS,                     /**< degrees Celsius (Cel) */
    SENML_UNIT_LUMEN,                       /**< lumen (lm) */
    SENML_UNIT_LUX,                         /**< lux (lx) */
    SENML_UNIT_BECQUEREL,                   /**< becquerel (Bq) */
    SENML_UNIT_GRAY,                        /**< gray (Gy) */
    SENML_UNIT_SIEVERT,                     /**< sievert (Sv) */
    SENML_UNIT_KATAL,                       /**< katal (kat) */
    SENML_UNIT_SQUARE_METER,                /**< square meter (area) (m2) */
    SENML_UNIT_CUBIC_METER,                 /**< cubic meter (volume) (m3) */
    SENML_UNIT_LITER,                       /**< *liter (volume)* (l) */
    SENML_UNIT_METER_PER_SECOND,            /**< meter per second (velocity) (m/s) */
    SENML_UNIT_METER_PER_SQUARE_SECOND,     /**< meter per square second (acceleration) (m/s2) */
    SENML_UNIT_CUBIC_METER_PER_SECOND,      /**< cubic meter per second (flow rate) (m3/s) */
    SENML_UNIT_LITER_PER_SECOND,            /**< *liter per second (flow rate)* (l/s) */
    SENML_UNIT_WATT_PER_SQUARE_METER,       /**< watt per square meter (irradiance) (W/m2) */
    SENML_UNIT_CANDELA_PER_SQUARE_METER,    /**< candela per square meter (luminance) (cd/m2) */
    SENML_UNIT_BIT,                         /**< bit (information content) (bit) */
    SENML_UNIT_BIT_PER_SECOND,              /**< bit per second (data rate) (bit/s) */
    SENML_UNIT_LATITUDE,                    /**< degrees latitude (lat) */
    SENML_UNIT_LONGITUDE,                   /**< degrees longitude (lon) */
    SENML_UNIT_PH,                          /**< pH value (acidity; logarithmic quantity) (pH) */
    SENML_UNIT_DECIBEL,                     /**< decibel (logarithmic quantity) (dB) */
    SENML_UNIT_DBW,                         /**< decibel relative to 1 W (power level) (dBW) */
    SENML_UNIT_BEL,                         /**< *bel (sound pressure level; logarithmic quantity)* (Bspl) */
    SENML_UNIT_COUNT,                       /**< 1 (counter value) (count) */
    SENML_UNIT_RATIO,                       /**< 1 (ratio e.g., value of a switch) (/) */
    SENML_UNIT_RATIO_2,                     /**< *1 (ratio e.g., value of a switch)* (%) */
    SENML_UNIT_RELATIVE_HUMIDITY_PERCENT,   /**< Percentage (Relative Humidity) (%RH) */
    SENML_UNIT_REMAINING_BATTERY_PERCENT,   /**< Percentage (remaining battery energy level) (%EL) */
    SENML_UNIT_REMAINING_BATTERY_SECONDS,   /**< seconds (remaining battery energy level) (EL) */
    SENML_UNIT_RATE,                        /**< 1 per second (event rate) (1/s) */
    SENML_UNIT_RPM,                         /**< *1 per minute (event rate, "rpm")* (1/min) */
    SENML_UNIT_BEAT_PER_MINUTE,             /**< *1 per minute (heart rate in beats per minute)* (beat/min)) */
    SENML_UNIT_BEATS,                       /**< *1 (Cumulative number of heart beats)* (beats) */
    SENML_UNIT_SIEMENS_PER_METER,           /**< Siemens per meter (conductivity) (S/m) */

    /* SenML units from RFC8798 */
    SENML_UNIT_BYTE,                        /**< Byte (information content) (B) */
    SENML_UNIT_VOLT_AMPERE,                 /**< volt-ampere (Apparent Power) (VA) */
    SENML_UNIT_VOLT_AMPERE_SECOND,          /**< volt-ampere second (Apparent Energy) (VAs) */
    SENML_UNIT_VOLT_AMPERE_REACTIVE,        /**< volt-ampere reactive (Reactive Power) (var) */
    SENML_UNIT_VOLT_AMPERE_REACTIVE_SECOND, /**< volt-ampere-reactive second (Reactive Energy) (vars) */
    SENML_UNIT_JOULE_PER_METER,             /**< joule per meter (Energy per distance) (J/m) */
    SENML_UNIT_KILOGRAM_PER_CUBIC_METER,    /**< kilogram per cubic meter (mass density, mass concentration) (kg/m3) */
    SENML_UNIT_DEGREE,                      /**< *degree (angle)* (deg) */

    /* SenML units from ISO7027-1:2016 */
    SENML_UNIT_NEPHELOMETRIC_TURBIDITY_UNIT, /**< Nephelometric Turbidity Unit (NTU) */

    /* SenML secondary units from RFC8798 */
    SENML_UNIT_MILLISECOND,                 /**< millisecond (ms, equivalent to 1/1000 s) */
    SENML_UNIT_MINUTE,                      /**< minute (min, equivalent to 60 s) */
    SENML_UNIT_HOUR,                        /**< hour (h, equivalent to 3600 s) */
    SENML_UNIT_MEGAHERTZ,                   /**< megahertz (MHz, equivalent to 1000000 Hz) */
    SENML_UNIT_KILOWATT,                    /**< kilowatt (kW, equivalent to 1000 W) */
    SENML_UNIT_KILOVOLT_AMPERE,             /**< kilovolt-ampere (kVA, equivalent to 1000 VA) */
    SENML_UNIT_KILOVAR,                     /**< kilovar (kvar, equivalent to 1000 var) */
    SENML_UNIT_AMPERE_HOUR,                 /**< ampere-hour (Ah, equivalent to 3600 C) */
    SENML_UNIT_WATT_HOUR,                   /**< watt-hour (Wh, equivalent to 3600 J) */
    SENML_UNIT_KILOWATT_HOUR,               /**< kilowatt-hour (kWh, equivalent to 3600000 J) */
    SENML_UNIT_VAR_HOUR,                    /**< var-hour (varh, equivalent to 3600 vars) */
    SENML_UNIT_KILOVAR_HOUR,                /**< kilovar-hour (kvarh, equivalent to 3600000 vars) */
    SENML_UNIT_KILOVOLT_AMPERE_HOUR,        /**< kilovolt-ampere-hour (kVAh, equivalent to 3600000 VAs) */
    SENML_UNIT_WATT_HOUR_PER_KILOMETER,     /**< watt-hour per kilometer (Wh/km, equivalent to 3.6 J/m) */
    SENML_UNIT_KIBIBYTE,                    /**< kibibyte (KiB, equivalent to 1024 B) */
    SENML_UNIT_GIGABYTE,                    /**< gigabyte (GB, equivalent to 1e9 B) */
    SENML_UNIT_MEGABIT_PER_SECOND,          /**< megabit per second (Mbit/s, equivalent to 1000000 bit/s) */
    SENML_UNIT_BYTE_PER_SECOND,             /**< byte per second (B/s, equivalent to 8 bit/s) */
    SENML_UNIT_MEGABYTE_PER_SECOND,         /**< megabyte per second (MB/s, equivalent to 8000000 bit/s) */
    SENML_UNIT_MILLIVOLT,                   /**< millivolt (mV, equivalent to 1/1000 V) */
    SENML_UNIT_MILLIAMPERE,                 /**< milliampere (mA, equivalent to 1/1000 A) */
    SENML_UNIT_DECIBEL_MILLIWATT,           /**< decibel (milliwatt) (dBm, equivalent to -29 dBW) */
    SENML_UNIT_MICROGRAM_PER_CUBIC_METER,   /**< microgram per cubic meter (ug/m3, equivalent to 1e-9 kg/m3) */
    SENML_UNIT_MILLIMETER_PER_HOUR,         /**< millimeter per hour (mm/h, equivalent to 1/3600000 m/s) */
    SENML_UNIT_METER_PER_HOUR,              /**< meter per hour (m/h, equivalent to 1/3600 m/s) */
    SENML_UNIT_PARTS_PER_MILLION,           /**< parts per million (ppm, equivalent to 1e-6 '/') */
    SENML_UNIT_PERCENT,                     /**< percent (/100, equivalent to 1/100 '/') */
    SENML_UNIT_PERMILLE,                    /**< permille (/1000, equivalent to 1/1000 '/') */
    SENML_UNIT_HECTOPASCAL,                 /**< hectopascal (hPa, equivalent to 100 Pa) */
    SENML_UNIT_MILLIMETER,                  /**< millimeter (mm, equivalent to 1/1000 m) */
    SENML_UNIT_CENTIMETER,                  /**< centimeter (cm, equivalent to 1/100 m) */
    SENML_UNIT_KILOMETER,                   /**< kilometer (km, equivalent to 1000 m) */
    SENML_UNIT_KILOMETER_PER_HOUR,          /**< kilometer per hour (km/h, equivalent to 1/3.6 m/s) */

    /* SenML secondary units from CoRE-1 */
    SENML_UNIT_PARTS_PER_BILLION,   /**< parts per billion (ppb, equivalent to 1e-9 '/') */
    SENML_UNIT_PARTS_PER_TRILLION,  /**< parts per trillion (ppt, equivalent to 1e-12 '/') */
    SENML_UNIT_VOLT_AMPERE_HOUR,    /**< volt-ampere-hour (VAh, equivalent to 3600 VAs) */
    SENML_UNIT_MILLIGRAM_PER_LITER, /**< milligram per liter (mg/l, equivalent to 1/1000 kg/m3) */
    SENML_UNIT_MICROGRAM_PER_LITER, /**< microgram per liter (ug/l, equivalent to 1e-6 kg/m3) */
    SENML_UNIT_GRAM_PER_LITER,      /**< gram per liter (g/l, equivalent to 1 kg/m3) */
} senml_unit_t;

/**
 * @brief SenML numeric value types.
 *
 */
typedef enum {
    SENML_TYPE_NUMERIC_UINT,    /**< Unsigned integer */
    SENML_TYPE_NUMERIC_INT,     /**< Integer */
    SENML_TYPE_NUMERIC_FLOAT,   /**< Floating point number */
    SENML_TYPE_NUMERIC_DOUBLE,  /**< Double-precision floating point number */
    SENML_TYPE_NUMERIC_DECFRAC, /**< Decimal fraction */
} senml_value_type_t;

/**
 * @brief Decimal fraction containing a value in the form of m * 10^e.
 */
typedef struct {
    int32_t e;  /**< Exponent */
    int32_t m;  /**< Mantissa */
} senml_decfrac_t;

/**
 * @brief SenML numeric value.
 *
 * Various SenML attributes (see @ref senml_attr_t) may contain any 'numeric'
 * types. This struct is used to contain these.
 */
typedef struct {
    senml_value_type_t type; /**< Type of the value */
    union {
        uint64_t u;
        int64_t i;
        float f;
        double d;
        struct { int32_t e; int32_t m; } df /** Decimal fraction */;
    } value; /**< Value data */
} senml_numeric_t;

/**
 * @brief SenML common record attributes.
 * All of these values are optional: empty or 0 values will not be encoded.
 * Note that some attributes need to be enabled explicitly.
 */
typedef struct {
    const char *base_name;          /**< Base Name */
    senml_numeric_t base_time;      /**< Base Time */
    senml_unit_t base_unit;         /**< Base Unit */
    senml_numeric_t base_value;     /**< Base Value */
#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM) || defined(DOXYGEN)
    senml_numeric_t base_sum;       /**< Base Sum, set `CONFIG_SENML_ATTR_SUM` to 1 to enable */
#endif
#if IS_ACTIVE(CONFIG_SENML_ATTR_VERSION) || defined(DOXYGEN)
    uint64_t base_version;          /**< Base Version, set `CONFIG_SENML_ATTR_VERSION` to 1 to enable */
#endif
    const char *name;               /**< Name of the measurement */
    senml_unit_t unit;              /**< Unit */
#if IS_ACTIVE(CONFIG_SENML_ATTR_SUM) || defined(DOXYGEN)
    senml_numeric_t sum;            /**< Sum, set `CONFIG_SENML_ATTR_SUM` to 1 to enable */
#endif
    senml_numeric_t time;           /**< Time of the measurement (relative or Unix) in seconds */
#if IS_ACTIVE(CONFIG_SENML_ATTR_UPDATE_TIME) || defined(DOXYGEN)
    senml_numeric_t update_time;    /**< Maximum time before the next sensor value, set `CONFIG_SENML_ATTR_UPDATE_TIME` to 1 to enable */
#endif
} senml_attr_t;

/**
 * @brief SenML string value.
 */
typedef struct {
    senml_attr_t attr;      /**< SenML attributes */
    senml_numeric_t value;  /**< Value */
} senml_value_t;

/**
 * @brief SenML string value.
 */
typedef struct {
    senml_attr_t attr;  /**< SenML attributes */
    const char *value;  /**< Value */
    size_t len;         /**< Value length */
} senml_string_value_t;

/**
 * @brief SenML boolean value.
 */
typedef struct {
    senml_attr_t attr;  /**< SenML attributes */
    bool value;         /**< Value */
} senml_bool_value_t;

/**
 * @brief SenML data value.
 */
typedef struct {
    senml_attr_t attr;      /**< SenML attributes */
    const uint8_t *value;   /**< Value */
    size_t len;             /**< Value length */
} senml_data_value_t;

/**
 * @brief Create a floating point numeric value.
 *
 * @param v Value to encode.
 * @return Numeric value containing the given value.
 */
static inline senml_numeric_t senml_float(float v)
{
    return (senml_numeric_t){ .type = SENML_TYPE_NUMERIC_FLOAT,
                              .value = { .f = v } };
}

/**
 * @brief Set a floating point numeric value.
 *
 * @param n Numeric value to set.
 * @param v Value to encode.
 */
static inline void senml_set_float(senml_numeric_t *n, float v)
{
    n->type = SENML_TYPE_NUMERIC_FLOAT;
    n->value.f = v;
}

/**
 * @brief Create a double precision floating point numeric value.
 *
 * @param v Value to encode.
 * @return Numeric value containing the given value.
 */
static inline senml_numeric_t senml_double(double v)
{
    return (senml_numeric_t){ .type = SENML_TYPE_NUMERIC_DOUBLE,
                              .value = { .d = v } };
}

/**
 * @brief Set a double precision floating point numeric value.
 *
 * @param n Numeric value to set.
 * @param v Value to encode.
 */
static inline void senml_set_double(senml_numeric_t *n, double v)
{
    n->type = SENML_TYPE_NUMERIC_DOUBLE;
    n->value.d = v;
}

/**
 * @brief Create an integer numeric value.
 *
 * @param v Value to encode.
 * @return Numeric value containing the given value.
 */
static inline senml_numeric_t senml_int(int64_t v)
{
    return (senml_numeric_t){ .type = SENML_TYPE_NUMERIC_INT,
                              .value = { .i = v } };
}

/**
 * @brief Set an integer numeric value.
 *
 * @param n Numeric value to set.
 * @param v Value to encode.
 */
static inline void senml_set_int(senml_numeric_t *n, int64_t v)
{
    n->type = SENML_TYPE_NUMERIC_INT;
    n->value.i = v;
}

/**
 * @brief Create an unsigned integer numeric value.
 *
 * @param v Value to encode.
 * @return Numeric value containing the given value.
 */
static inline senml_numeric_t senml_uint(uint64_t v)
{
    return (senml_numeric_t){ .type = SENML_TYPE_NUMERIC_UINT,
                              .value = { .u = v } };
}

/**
 * @brief Set an unsigned integer numeric value.
 *
 * @param n Numeric value to set.
 * @param v Value to encode.
 */
static inline void set_senml_uint(senml_numeric_t *n, uint64_t v)
{
    n->type = SENML_TYPE_NUMERIC_UINT;
    n->value.u = v;
}

/**
 * @brief Create a decimal fraction numeric value in the form `m*10^e`.
 *
 * @param m Mantissa (value) to encode.
 * @param e Exponent (scale) to encode.
 * @return Numeric value containing the given value.
 */
static inline senml_numeric_t senml_decfrac(int32_t m, int32_t e)
{
    return (senml_numeric_t){ .type = SENML_TYPE_NUMERIC_DECFRAC,
                              .value = { .df = { .e = e, .m = m } } };
}

/**
 * @brief Set a decimal fraction numeric value in the form `m*10^e`.
 *
 * @param n Numeric value to set.
 * @param m Mantissa (value) to encode.
 * @param e Exponent (scale) to encode.
 */
static inline void senml_set_decfrac(senml_numeric_t *n, int32_t m, int32_t e)
{
    n->type = SENML_TYPE_NUMERIC_DECFRAC;
    n->value.df.e = e;
    n->value.df.m = m;
}

/**
 * @brief Get an integer representation of a duration in seconds.
 *
 * @param s Duration in seconds.
 * @return Numeric representation of the duration in seconds.
 */
static inline senml_numeric_t senml_duration_s(int64_t s)
{
    return senml_int(s);
}

/**
 * @brief Set an integer representation of a duration in seconds.
 *
 * @param n Numeric value to set.
 * @param s Duration in seconds.
 */
static inline void senml_set_duration_s(senml_numeric_t *n, int64_t s)
{
    senml_set_int(n, s);
}

/**
 * @brief Get a @ref senml_decfrac_t representation of a duration in milliseconds.
 *
 * @param ms Duration in milliseconds.
 * @return Numeric representation of the duration.
 */
static inline senml_numeric_t senml_duration_ms(int32_t ms)
{
    return senml_decfrac(ms, -3);
}

/**
 * @brief Set a @ref senml_decfrac_t representation of a duration in milliseconds.
 *
 * @param n Numeric value to set.
 * @param ms Duration in milliseconds.
 * @return Numeric representation of the duration.
 */
static inline void senml_set_duration_ms(senml_numeric_t *n, int32_t ms)
{
    senml_set_decfrac(n, ms, -3);
}

/**
 * @brief Get a @ref senml_decfrac_t representation of a duration in microseconds.
 *
 * @param us Duration in microseconds.
 * @return Numeric representation of the duration.
 */
static inline senml_numeric_t senml_duration_us(int32_t us)
{
    return senml_decfrac(us, -6);
}

/**
 * @brief Get a @ref senml_decfrac_t representation of a duration in microseconds.
 *
 * @param n Numeric value to set.
 * @param us Duration in microseconds.
 * @return Numeric representation of the duration.
 */
static inline void senml_set_duration_us(senml_numeric_t *n, int32_t us)
{
    senml_set_decfrac(n, us, -6);
}

/**
 * @brief Get a @ref senml_decfrac_t representation of a duration in nanoseconds.
 *
 * @param ns Duration in nanoseconds.
 * @return Numeric representation of the duration.
 */
static inline senml_numeric_t senml_duration_ns(int32_t ns)
{
    return senml_decfrac(ns, -9);
}

/**
 * @brief Set a @ref senml_decfrac_t representation of a duration in nanoseconds.
 *
 * @param n Numeric value to set.
 * @param ns Duration in nanoseconds.
 */
static inline void senml_set_duration_ns(senml_numeric_t *n, int32_t ns)
{
    senml_set_decfrac(n, ns, -9);
}

/**
 * @brief Convert a SenML unit to a string.
 *
 * See the [SenML units](https://www.iana.org/assignments/senml/senml.xhtml#senml-units) and
 * [Secondary units](https://www.iana.org/assignments/senml/senml.xhtml#secondary-units) from IANA.
 * Values not defined in @ref senml_unit_t will result in an empty string.
 *
 * @param unit Unit to convert to string.
 *
 * @return String representation of the unit.
 */
const char *senml_unit_to_str(senml_unit_t unit);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SENML_H */
