/*
 * Copyright (C) 2015 Engineering-Spirit
 *               2017 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32_common
 * @ingroup         drivers_periph_dma
 * @{
 *
 * @file
 * @brief           Shared DMA definitions for the STM32 families using channels
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Víctor Ariño <victor.arino@zii.aero>
 */

#ifndef DMA_CHANNEL_H
#define DMA_CHANNEL_H

#define STM32_DMA_CHANNEL

/**
 * @brief Get the channel number of a DMA unit
 */
static inline int dma_chnum(dma_t dma)
{
    return (int)dma;
}

/**
 * @brief Wrapper for the unit identifier of a DMA unit
 */
static inline int dma_unit(dma_t dma)
{
    return dma_chnum(dma);
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] channel   logical DMA stream
 *
 * @return  base address for the selected DMA channel
 */
static inline DMA_Channel_TypeDef *dma_channel(dma_t dma)
{
    uint32_t base = (uint32_t)dma_base(dma);
    int channel = dma_chnum(dma);
    if (channel >= 7) {
        channel -= 7;
    }
    return (DMA_Channel_TypeDef *)(base + (0x08 + (0x14 * channel)));
}

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA channel handling, we map the DMA channels/streams
 * transparently to one integer number.
 *
 * @param[in] dma    DMA unit
 */
static inline DMA_TypeDef *dma_base(dma_t dma)
{
#if defined(DMA2)
    if (dma_chnum(dma) >= 7) {
        return DMA2;
    }
#endif

    return DMA1;
}

#ifndef DOXYGEN
/**
 * @brief   Define specific DMA_UNIT generator macro
 *
 * @param[in] channel  Channel as given in the stm specification (from 1 to 12)
                       DMA1 channel1 equals 0, DMA2 channel1 equals 7, DMA2
                       channel7 equals 14 and so on.
 */
#define DMA_UNIT(channel) ((dma_t)(channel - 1))

static inline void dma_end_isr(dma_t dma)
{
    int channel = dma_chnum(dma);
    if (channel >= 7) {
        channel -= 7;
    }
    /* clear global interrupt and tx complete flags */
    dma_base(dma)->IFCR = (0x3 << (channel << 2));
}

#endif /* DOXYGEN */

#endif /* DMA_CHANNEL_H */
/** @} */
