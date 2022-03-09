/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx ROM Hard-API
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_HARD_API_H
#define CC26XX_CC13XX_HARD_API_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ROM Hard-API
 */
typedef struct {
    /** CRC-32 */
    uint32_t (* crc32)(uint8_t *data, uint32_t bytecount, uint32_t repeatcount);
    uint32_t (* get_fl_size)(void); /**< Get flash size */
    uint32_t (* get_chip_id)(void); /**< Get chip ID */
    uint32_t (* __reserved0)(uint32_t); /**< Reserved */
    uint32_t (* __reserved1)(void); /**< Reserved */
    uint32_t (* __reserved2)(uint8_t *, uint32_t, uint32_t); /**< Reserved */
    void (* resetdev)(void); /**< Reset device */
    uint32_t (* fletcher32)(uint16_t *data, uint16_t wordcount,
                            uint16_t repeatcount); /**< Fletcher-32 */
    /** Min. value */
    uint32_t (* min_val)(uint32_t *buffer, uint32_t datacount);
    /** Max. value */
    uint32_t (* max_val)(uint32_t *databuffer, uint32_t datacount);
    /** Mean. value */
    uint32_t (* mean_val)(uint32_t *databuffer, uint32_t datacount);
    /** Standard deviation value */
    uint32_t (* stdd_val)(uint32_t *databuffer, uint32_t datacount);
    void (* hf_source_safe_switch)(void); /**< HF source safe-switch */
    void (* __reserved3)(uint32_t); /**< Reserved */
    void (* __reserved4)(uint32_t); /**< Reserved */
    void (* compa_in)(uint8_t signal); /**< Select CompA input */
    void (* compa_ref)(uint8_t signal); /**< Select CompA reference */
    void (* adc_compb_in)(uint8_t signal); /**< Select ADC CompB input */
#ifdef CPU_VARIANT_X2
    void (* dac_vref)(uint8_t signal); /**< Select DAC vref */
#else
    void (* compb_ref)(uint8_t signal); /**< Select CompB reference */
#endif
} hard_api_t;

/**
 * @brief   Hard-API function table
 */
#define HARD_API ((hard_api_t *) ROM_HARD_API_BASE)

/**
 * @brief   Hard-API functions
 * @{
 */
#define rom_hapi_crc32(a, b, c)            (HARD_API->crc32((a), (b), (c)))
#define rom_hapi_get_flashsize()           (HARD_API->get_fl_size())
#define rom_hapi_get_chipid()              (HARD_API->get_chip_id())
#define rom_hapi_reset_device()            (HARD_API->resetdev())
#define rom_hapi_fletcher32(a, b, c)       (HARD_API->fletcher32((a), (b), (c)))
#define rom_hapi_min_value(a, b)           (HARD_API->min_val((a), (b)))
#define rom_hapi_max_value(a, b)           (HARD_API->max_val((a), (b)))
#define rom_hapi_mean_value(a, b)          (HARD_API->mean_val((a), (b)))
#define rom_hapi_std_deviation_value(a, b) (HARD_API->stdd_val((a), (b)))
#define rom_hapi_hf_source_safe_switch()   (HARD_API->hf_source_safe_switch())
#define rom_hapi_select_compa_input(a)     (HARD_API->compa_in((a)))
#define rom_hapi_select_compa_ref(a)       (HARD_API->compa_ref((a)))
#define rom_hapi_select_adc_compb_input(a) (HARD_API->adc_compb_in((a)))
#ifdef CPU_VARIANT_X2
#define rom_hapi_select_dac_vref(a)        (HARD_API->dac_vref((a)))
#else
#define rom_hapi_select_compb_ref(a)       (HARD_API->compb_vref((a)))
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_HARD_API_H */
/** @} */
