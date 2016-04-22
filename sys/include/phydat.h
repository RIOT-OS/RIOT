/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_phydat Phydat
 * @ingroup     sys
 * @brief       Generic data container for physical data and utility functions
 *
 * The purpose of this module is to introduce a common view on physical data
 * throughout RIOT. This data is typically the output from sensor readings, data
 * aggregation, and also the input for actuators.
 *
 * The idea is to enable different sensor/actuator drivers and other RIOT
 * modules to exchange and have the same view on this kind of data. Labeling
 * data with a unit type it's scaling makes it possible to pipe data between
 * modules in an automated fashion without the need of specialized software
 * wrappers and/or data normalization modules.
 *
 * @todo        It might make sense to introduce additional data types for
 *              increased precision, i.e. something like phydat_float_t...
 *
 * @{
 *
 * @file
 * @brief       Generic data container for physical data interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SECT_DATA_H
#define SECT_DATA_H

#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The fixed number of dimensions we work with
 *
 * We use a fixed number of 3 dimensions, as many physical values we encounter
 * can be expressed this way. In practice we have e.g. readings from
 * accelerometers, gyros, color sensors, or set data for RGB LEDs.
 *
 * When expressing 1-dimensional data we just ignore the 2 higher dimension.
 * This leads to a slight overhead of some byte of memory - but we benefit from
 * a unified data structure for passing around physical data.
 */
#define PHYDAT_DIM                  (3U)

/**
 * @brief   The maximum length of a scaling string
 */
#define PHYDAT_SCALE_STR_MAXLEN     (sizeof("*E-128\0"))

/**
 * @brief   Definition of physical units and comparable data types
 *
 * This list should contain all needed physical units (e.g. SI units), but also
 * non-physical units that can be used to define the type of data passed around.
 * This can be for example BOOL or aggregate values. As rule of thumb, the unit
 * list can contain anything that helps two modules automatically negotiate, if
 * they can understand each other.
 *
 * @note    Extent this list as needed.
 */
enum {
    /* generic values */
    UNIT_UNDEF,     /**< unit undefined */
    UNIT_NONE,      /**< data has no physical unit */
    /* temperature */
    UNIT_TEMP_C,    /**< degree Celsius */
    UNIT_TEMP_F,    /**< degree Fahrenheit */
    UNIT_TEMP_K,    /**< Kelvin */
    /* dimension */
    UNIT_M,         /**< meters */
    UNIT_M2,        /**< square meters */
    UNIT_M3,        /**< cubic meters */
    /* kinetic */
    UNIT_G,         /**< gravitational force */
    UNIT_DPS,       /**< degree per second */
    /* weight */
    UNIT_GR,        /**< grams - not using the SI unit (kg) here to make scale
                     *   handling simpler */
    /* electricity */
    UNIT_A,         /**< Ampere */
    UNIT_V,         /**< Volts */
    UNIT_GS,        /**< gauss */
    /* pressure */
    UNIT_BAR,       /**< Beer? */
    UNIT_PA,        /**< Pascal */
    /* light */
    UNIT_CD,        /**< Candela */
    /* logical */
    UNIT_BOOL,      /**< boolean value [0|1] */
    UNIT_PERCENT,   /**< out of 100 */
    UNIT_PERMILL,   /**< out of 1000 */
    UNIT_PPM,       /**< part per million */
    /* aggregate values */
    UNIT_TIME,      /**< the three dimensions contain sec, min, and hours */
    UNIT_DATE       /**< the 3 dimensions contain days, months and years */
    /* extend this list as needed */
};

/**
 * @brief   Generic data structure for expressing physical values
 *
 * Physical data is expressed in a 3-dimensional touple of values. In addition
 * to the data fields, this struct contains further the (physical) unit and the
 * scale factor of the data. The unit is expressed as constant. The scale factor
 * is expressed as power of 10 (10^factor).
 *
 * The combination of signed 16-bit numbers with and the scale factor gives us a
 * very high dynamic range (from -32*10^-131 to 32*10^130). In a wider sense we
 * are saving the values as fixed floating points...
 *
 * The scale factor is identical for all 3 values.
 *
 * In a traditional (scientific) computational system the obvious choice for the
 * used data type would be to use floats. We are however on heavily resource
 * constrained (even 8-bit) embedded systems, so we use int16_t here. As most
 * sensor are in some way ADC based, they normally do not use a higher accuracy
 * than 12-14bit, so using 16-bit integers to represent this data is good enough
 * in most cases.
 */
typedef struct {
    int16_t val[PHYDAT_DIM];    /**< the 3 generic dimensions of data */
    uint8_t unit;               /**< the (physical) unit of the data */
    int8_t scale;               /**< the scale factor, 10^*scale* */
} phydat_t;

/**
 * @brief   Dump the given data container to STDIO
 *
 * @param[in] data      data container to dump
 * @param[in] dim       number of dimension of @p data to dump
 */
void phydat_dump(phydat_t *data, uint8_t dim);

/**
 * @brief   Convert the given unit to a string
 *
 * @param[in] unit      unit to convert
 *
 * @return  string representation of given unit (e.g. V or m)
 * @return  NULL if unit was not recognized
 */
const char *phydat_unit_to_str(uint8_t unit);

/**
 * @brief   Convert the given scale factor to a NULL terminated string
 *
 * The given scaling factor will be given as SI unit (e.g. M for Mega, u for
 * micro, etc) for obvious cases or in scientific notation (e.g. 2E11, 1E-22,
 * etc) otherwise.
 *
 * @param[in] scale     scale factor to convert
 */
char phydat_scale_to_str(int8_t scale);

#ifdef __cplusplus
}
#endif

#endif /* SECT_DATA_H */
/** @} */
