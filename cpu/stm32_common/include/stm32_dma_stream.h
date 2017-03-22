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

#ifndef STM32_DMA_STREAM_H
#define STM32_DMA_STREAM_H

/**
 * @brief Get the channel number of a DMA unit
 */
static inline int _dma_chnum(dma_t dma)
{
    return (dma >> 16);
}

/**
 * @brief Get the stream number of a DMA unit
 */
static inline int _dma_strnum(dma_t dma)
{
    return (dma & 0xff);
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
    int stream = _dma_strnum(dma);
    uint32_t base = (uint32_t)dma_base(dma);

    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (stream & 0x7))));
}

/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] stream    logical DMA stream
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(int stream)
{
    return ((stream & 0x4) >> 2);
}

/**
 * @brief   Get the interrupt flag clear bit position in the DMA LIFCR register
 *
 * @param[in] stream    logical DMA stream
 */
static inline uint32_t dma_ifc(int stream)
{
    switch (stream & 0x3) {
        case 0:
            return (1 << 5);
        case 1:
            return (1 << 11);
        case 2:
            return (1 << 21);
        case 3:
            return (1 << 27);
        default:
            return 0;
    }
}

#ifndef DOXYGEN
/**
 * @brief   Define specific DMA_UNIT generator macro
 */
#define DMA_UNIT(channel, stream) ((dma_t)((channel << 16) | stream))

static inline void dma_poweron(dma_t dma)
{
    if (_dma_strnum(dma) < 8) {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA1EN);
    }
    else {
        periph_clk_en(AHB1, RCC_AHB1ENR_DMA2EN);
    }
}

static inline void dma_poweroff(dma_t dma)
{
    if (_dma_strnum(dma) < 8) {
        periph_clk_dis(AHB1, RCC_AHB1ENR_DMA1EN);
    }
    else {
        periph_clk_dis(AHB1, RCC_AHB1ENR_DMA2EN);
    }
}

static inline DMA_TypeDef *dma_base(dma_t dma)
{
    return (_dma_strnum(dma) < 8) ? DMA1 : DMA2;
}

static inline void dma_end_isr(dma_t dma)
{
    int stream = _dma_strnum(dma);
    dma_base(dma)->IFCR[dma_hl(stream)] = dma_ifc(stream);
}

static inline void dma_isr_enable(dma_t dma)
{
    int stream = _dma_strnum(dma);

    if (stream < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    }
    else if (stream < 13) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
}

static inline void dma_transfer(dma_t dma, dma_mode_t mode,
        void *src, void *dst, unsigned len)
{
    DMA_Stream_TypeDef *stream = dma_stream(dma);
    stream->CR &= ~(DMA_SxCR_CHSEL | DMA_SxCR_DIR | DMA_SxCR_EN | DMA_SxCR_PINC);
    stream->CR |= (_dma_chnum(dma) << 25) | (mode << 6) | DMA_SxCR_MINC
            | DMA_SxCR_MBURST | DMA_SxCR_PBURST | DMA_SxCR_TCIE;

    if (mode == MEMORY_TO_PERIPH) {
        stream->M0AR = (uint32_t)src;
        stream->PAR = (uint32_t)dst;
    }
    else {
        if (mode == MEMORY_TO_MEMORY) {
            stream->CR |= DMA_SxCR_PINC;
        }
        stream->PAR = (uint32_t)src;
        stream->M0AR = (uint32_t)dst;
    }

    stream->NDTR = (uint16_t)len;
    stream->CR = DMA_SxCR_EN;
}
#endif /* DOXYGEN */

#endif /* STM32_DMA_STREAM_H */
/** @} */
