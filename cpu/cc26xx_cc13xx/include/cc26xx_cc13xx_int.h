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
 * @brief           CC26xx/CC13xx MCU interrupt handler configuration
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_INT_H
#define CC26XX_CC13XX_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CC26xx/CC13xx interrupt handlers
 */
typedef struct {
    void (* isr_rfc_cpe0)(void); /**< CPE interrupt line 0 */
    void (* isr_rfc_cpe1)(void); /**< CPE interrupt line 1 */
} cc26xx_cc13xx_ints_t;

/**
 * @brief   Global variable storing interrupt handlers.
 */
extern cc26xx_cc13xx_ints_t cc26xx_cc13xx_ints;

#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_INT_H */

/*@}*/
