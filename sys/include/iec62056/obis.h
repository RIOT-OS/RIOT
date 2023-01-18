/*
 * SPDX-FileCopyrightText: 2023 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_iec62056_obis IEC 62056-61 - OBIS Identification System
 * @ingroup     sys_iec62056
 * @brief       OBject Identification System (OBIS) for meter data addressing
 *
 * OBIS (OBject Identification System), defined in IEC 62056-61, provides a
 * hierarchical identification scheme for data items in utility meters. Each
 * OBIS code consists of six value groups (A through F) that identify:
 *
 * - **Group A**: Media/energy type (electricity, gas, water, heat, etc.)
 * - **Group B**: Measurement channel number
 * - **Group C**: Abstract or physical data item identifier
 * - **Group D**: Type or processing method of the measured quantity
 * - **Group E**: Further processing or classification (e.g., tariff rates)
 * - **Group F**: Historical data storage identifier
 *
 * OBIS codes are represented in reduced format as `A-B:C.D.E.F` where groups
 * may be omitted if not applicable (defaulting to 255).
 *
 * This module provides functions for parsing OBIS codes from IEC 62056-21
 * telegram strings and comparing OBIS identifiers.
 *
 * @see https://en.wikipedia.org/wiki/IEC_62056
 *
 * @{
 *
 * @file
 * @brief       OBIS code parsing and comparison API
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

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
    COSEM_OBIS_GROUP_A_HEAT_COST    = 4,    /**< Heat cost allocator related */
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
    uint8_t a;  /**< Group A: Media/energy type */
    uint8_t b;  /**< Group B: Measurement channel number */
    uint8_t c;  /**< Group C: Abstract or physical data item identifier */
    uint8_t d;  /**< Group D: Type or result of processing of the physical quantity */
    uint8_t e;  /**< Group E: Further processing or classification */
    uint8_t f;  /**< Group F: Identifies historical values of data */
} iec62056_obis_t;

/**
 * @brief Parse an OBIS string to @ref iec62056_obis_t object
 *
 * @param[out] obis             Obis struct to parse into
 * @param[in] str               String to parse from
 * @param[in] len               Length in bytes of the string
 *
 * @retval 0 on success
 * @retval <0 on error
 */
int iec62056_obis_from_string(iec62056_obis_t *obis, const char *str, size_t len);

/**
 * @brief Check two @ref iec62056_obis_t structs for equality
 *
 * @param[in] first             First obis struct to compare
 * @param[in] second            Second obis struct to compare
 * @param[in] include_channel   Whether to include the channel in the comparison
 *
 * @retval true if the two structs represent the same address
 * @retval false if they differ
 */
bool iec62056_obis_equal(const iec62056_obis_t *first, const iec62056_obis_t *second,
                         bool include_channel);

/**
 * @brief Initialize an OBIS structure
 *
 * @param[in] a                 Group A value
 * @param[in] b                 Group B value
 * @param[in] c                 Group C value
 * @param[in] d                 Group D value
 * @param[in] e                 Group E value
 * @param[in] f                 Group F value
 *
 * @retval The initialized obis struct
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
/** @} */
