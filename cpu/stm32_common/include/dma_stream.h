/*
 * Copyright (C) 2015 Engineering-Spirit
 *               2015-2016 Freie Universität Berlin
 *               2016-2017 OTA keys S.A.
 *               2017 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32_common
 * @{
 *
 * @file
 * @brief           Shared DMA definitions for the STM32 families using streams
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Víctor Ariño <victor.arino@zii.aero>
 */

#ifndef DMA_STREAM_H
#define DMA_STREAM_H

#define STM32_DMA_STREAM

/**
 * @brief Get the stream number of a DMA unit
 */
static inline int dma_strnum(dma_t dma)
{
    return (dma & 0xff);
}

/**
 * @brief Get the channel number of a DMA unit
 */
static inline int dma_chnum(dma_t dma)
{
    return (dma >> 8);
}

/**
 * @brief Wrapper for the unit identifier of a DMA unit
 */
static inline int dma_unit(dma_t dma)
{
    return dma_strnum(dma);
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] dma    DMA unit
 *
 * @return  base address for the selected DMA stream
 */
static inline DMA_Stream_TypeDef *dma_stream(dma_t dma)
{
    uint32_t base = (uint32_t)dma_base(dma);

    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (dma_strnum(dma) & 0x7))));
}

/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] dma    DMA unit
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(int dma)
{
    return ((dma_strnum(dma) & 0x4) >> 2);
}

/**
 * @brief   Get DMA base register
 *
 * @param[in] dma    DMA unit
 */
static inline DMA_TypeDef *dma_base(dma_t dma)
{
    return (dma_strnum(dma) < 8) ? DMA1 : DMA2;
}

#ifndef DOXYGEN
/**
 * @brief     Define specific DMA_UNIT generator macro
 *
 *
 * @param[in] channel DMA channel (from 0 to 7).
 * @param[in] stream  DMA stream (from 0 to 15). DMA1 stream0 equals 0, DMA2
 *                    stream0 equals 8, DMA2 stream 7 equals 15 and so on.
 */
#define DMA_UNIT(channel, stream) ((dma_t)((channel << 8) | stream))

static inline void dma_end_isr(dma_t dma)
{
    int stream = (dma_strnum(dma) & 0x03);
    uint32_t bit = (1 << (5 + (6 * stream)));
    if (stream > 1)  {
        bit <<= 4;
    }
    dma_base(dma)->IFCR[dma_hl(dma)] = bit;
}

#endif /* DOXYGEN */

#endif /* DMA_STREAM_H */
/** @} */
