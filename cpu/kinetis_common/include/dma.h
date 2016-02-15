/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_dma Kinetis DMA
 * @ingroup     cpu_kinetis_common
 * @brief       Driver for Freescale Kinetis DMA module
 *
 * @{

 * @file
 * @brief       Interface definition for the Kinetis DMA driver
 *
 * This is for internal use in the Kinetis drivers. The API allows for detailed
 * control over DMA transfers. Users of this driver are expected to be familiar
 * with Cortex-M and the Kinetis processor internals as well as its DMA module.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef KINETIS_COMMON_DMA_H
#define KINETIS_COMMON_DMA_H

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   DMA device to use
 */
#define DMA_DEV DMA0

/**
 * @brief   DMA access width settings
 */
typedef enum {
    DMA_ACCESS_SIZE_8BIT       = 0,
    DMA_ACCESS_SIZE_16BIT      = 1,
    DMA_ACCESS_SIZE_32BIT      = 2,
    DMA_ACCESS_SIZE_RESERVED3  = 3,
    DMA_ACCESS_SIZE_16BYTE     = 4,
    DMA_ACCESS_SIZE_32BYTE     = 5,
    DMA_ACCESS_SIZE_RESERVED6  = 6,
    DMA_ACCESS_SIZE_RESERVED7  = 7,
} dma_access_size_t;

/**
 * @brief   Initialize the DMA hardware module and set up the given channel
 *
 * @param[in]  channel  The channel to configure
 * @param[in]  source   DMA channel activation source
 *
 * @return 0  on success
 * @return <0 on error
 */
int dma_init(const uint8_t channel, const dma_source_t source);

/**
 * @brief   Enable a configured DMA channel
 *
 * @param[in]  channel  The channel to enable
 */
static inline void dma_channel_enable(const uint8_t channel)
{
    DMA_DEV->SERQ = DMA_SERQ_SERQ(channel);
}

/**
 * @brief   Disable a configured DMA channel
 *
 * @param[in]  channel  The channel to enable
 */
static inline void dma_channel_disable(const uint8_t channel)
{
    DMA_DEV->CERQ = DMA_CERQ_CERQ(channel);
}

/**
 * @brief   Clear the interrupt flag for the given channel
 *
 * @param[in]  channel  The DMA channel interrupt to clear
 */
static inline void dma_clear_irq(const uint8_t channel)
{
    DMA_DEV->CINT = DMA_CINT_CINT(channel);
}

#ifdef __cplusplus
}
#endif

#endif /* KINETIS_COMMON_DMA_H */
/** @} */
