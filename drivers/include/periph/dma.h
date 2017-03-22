/*
 * Copyright (C) 2017 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_dma DMA
 * @ingroup     drivers_periph
 * @brief       Low-level DMA peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level DMA peripheral driver interface definitions
 *
 * @author      Victor Ariño <victor.arino@zii.aero>
 */

#ifndef DMA_H
#define DMA_H

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_DMA_T
/**
 * @brief   DMA type identifier
 */
typedef unsigned int dma_t;
#endif /* HAVE_DMA_T */

#ifndef HAVE_DMA_MODE_T
/**
 * @brief   Available DMA modes
 */
typedef enum {
    PERIPH_TO_MEMORY = 0,      /**< Transfer from the periph to the memory */
    MEMORY_TO_PERIPH,          /**< Transfer from the memory to the periph */
    MEMORY_TO_MEMORY,          /**< Transfer from the memory to the memory */
} dma_mode_t;
#endif /* HAVE_DMA_MODE_T */

#ifndef DMA_UNIT
/**
 * @brief   Convert a generic DMA unit to @c dma_t type
 */
#define DMA_UNIT(unit)       ((dma_t)(unit))
#endif

/**
 * @brief  Configure and start a DMA transfer
 *
 * This method sets up a DMA transfer between two MCU modules according to the
 * given @p mode.
 *
 * @note in case @p src or @p dst belong to a peripheral the pointer shall not
 * increase.
 *
 * @param[in] dma   The DMA unit to use
 * @param[in] mode  The transfer mode
 * @param[in] src   Pointer to the data origin
 * @param[in] dst   Pointer to the data destination
 * @param[in] len   Length of the data to transfer
 */
void dma_transfer(dma_t dma, dma_mode_t mode, void *src, void *dst,
        unsigned len);

/**
 * @brief  Power on a DMA unit
 *
 * @param[in] dma  DMA unit to power on
 */
void dma_poweron(dma_t dma);

/**
 * @brief  Power off a DMA unit
 *
 * @note This method might or might not take care whether powering off a unit
 * might have influence in other units
 *
 * @param[in] dma  DMA unit to power off
 */
void dma_poweroff(dma_t dma);

/**
 * @brief  Enable the ISR for a DMA unit
 *
 * @param[in] dma  DMA unit
 */
void dma_isr_enable(dma_t dma);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H */
/** @} */
