/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_iec62056_obis OBIS
 * @ingroup     sys_iec62056
 * @brief       OBject Identification System Cmpanion Specification for Energy Monitoring
 *
 * This module mainly contains functions related to parsing OBIS codes from iec62056-21 modes.
 *
 * @see https://en.wikipedia.org/wiki/IEC_62056
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef IEC62056_OBIS_H
#define IEC62056_OBIS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Value group A definitions
 */
typedef enum {
    COSEM_OBIS_GROUP_A_ABSTRACT     = 0,    /**< Abstract objects */
    COSEM_OBIS_GROUP_A_ELECTRICITY  = 1,    /**< Electricity related */
    COSEM_OBIS_GROUP_A_HEAT_COST    = 4,    /**< Heat cost allocater related */
    COSEM_OBIS_GROUP_A_THERMAL_1    = 5,    /**< Thermal energy related */
    COSEM_OBIS_GROUP_A_THERMAL_2    = 6,    /**< More thermal energy related */
    COSEM_OBIS_GROUP_A_GAS          = 7,    /**< Gas related */
    COSEM_OBIS_GROUP_A_COLD_WATER   = 8,    /**< Cold water related */
    COSEM_OBIS_GROUP_A_HOT_WATER    = 9,    /**< Hot water related */
    COSEM_OBIS_GROUP_A_OTHER        = 15,   /**< Other media */
} iec62056_obis_group_a_t;

/**
 * @brief OBIS identifier struct
 */
typedef struct {
    uint8_t a;  /**< Group A: media/energy type */
    uint8_t b;  /**< Group B: Measurement channel number */
    uint8_t c;  /**< Group C: Abstract or physical data item identifier */
    uint8_t d;  /**< Group D: Type or result of processing of the physical quantity */
    uint8_t e;  /**< Group E: Further processing or classification */
    uint8_t f;  /**< Group F: Identifies historical values  of data */
} iec62056_obis_t;

/**
 * @brief Parse an obis string to @ref iec62056_obis_t object
 *
 * @param obis  Obis struct to parse into
 * @param str   String to parse from
 * @param len   Length in bytes of the string
 *
 * @return  Negative on error
 * @return  Zero on success
 */
int iec62056_obis_from_string(iec62056_obis_t *obis, const char *str, size_t len);

/**
 * @brief Check two @ref iec62056_obis_t structs for equality
 *
 * @param first             First obis struct to compare
 * @param second            Second obis struct to compare
 * @param include_channel   Whether to include the channel in the comparison
 *
 * @return True if the two structs represent the same address
 */
bool iec62056_obis_equal(const iec62056_obis_t *first, const iec62056_obis_t *second,
                         bool include_channel);

/**
 * @brief Initialize an obis structure
 *
 * @param   a   Group A value
 * @param   b   Group B value
 * @param   c   Group C value
 * @param   d   Group D value
 * @param   e   Group E value
 * @param   f   Group F value
 *
 * @return  The initialized obis struct
 */
static inline iec62056_obis_t iec62056_obis_init(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
                                                 uint8_t e,
                                                 uint8_t f)
{
    return (iec62056_obis_t){ a, b, c, d, e, f };
}
#ifdef __cplusplus
}
#endif

#endif /* IEC62056_OBIS_H */
/** @} */
