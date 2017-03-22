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

#ifndef STM32_DMA_CHANNEL_H
#define STM32_DMA_CHANNEL_H

/**
 * @brief Get the channel number of a DMA unit
 */
static inline int _dma_chnum(dma_t dma)
{
    return (dma & 0xff);
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
    /**
     * DMA Channel register map is calculated with:
     *      Address offset: 0x08 + 0d20 × (channel number – 1)
     *
     * STM32's Channel 1 is our Channel 0 so no - 1 needed
     */
    uint32_t base = (uint32_t)dma_base(dma);
    int channel = _dma_chnum(dma);
    if (channel >= 7) {
        channel -= 7;
    }
    return (DMA_Channel_TypeDef *)(base + (0x08 + (0x14 * channel)));
}

/**
 * @brief   Get the transfer complete flag position for the corresponding channel
 *
 * @param[in] channel   logical DMA channel
 */
static inline uint32_t dma_ifc(int channel)
{
    /**
     * DMA_ISR & DMA_IFCR have per channel 4 interrupt bits
     * | b3      | b2     | b1       | b0     |
     * |--------------------------------------|
     * | TEIFx   | HTIFx  | TCIFx    | GIFx   |
     * | Error   | Half   | Complete | Global |
     *
     * `2 << (channel * 4)` marks the TCIFx bit
     */

    if (channel >= 7) {
        channel -= 7;
    }

    return (0x2 << (channel << 2));
}

#ifndef DOXYGEN
/**
 * @brief   Define specific DMA_UNIT generator macro
 */
#define DMA_UNIT(channel) ((dma_t)(channel))

static inline void dma_poweron(dma_t dma)
{
#if defined(DMA2)
    if (_dma_chnum(dma) >= 7) {
        periph_clk_en(AHB, RCC_AHBENR_DMA2EN);
        return;
    }
#endif

    periph_clk_en(AHB, RCC_AHBENR_DMA1EN);
}

static inline void dma_poweroff(dma_t dma)
{
#if defined(DMA2)
    if (_dma_chnum(dma) >= 7) {
        periph_clk_dis(AHB, RCC_AHBENR_DMA2EN);
        return;
    }
#endif

    periph_clk_dis(AHB, RCC_AHBENR_DMA1EN);
}

static inline DMA_TypeDef *dma_base(dma_t dma)
{
#if defined(DMA2)
    if (_dma_chnum(dma) >= 7) {
        return DMA2;
    }
#endif

    return DMA1;
}

static inline void dma_isr_enable(dma_t dma)
{
    /**
     * | ch   | IRQn                | ISR   |
     * -------------------------------------|
     * | 0    | DMA1_Channel1_IRQn  | 11    |
     * | 1    | DMA1_Channel2_IRQn  | 12    |
     * | 2    | DMA1_Channel3_IRQn  | 13    |
     * | 3    | DMA1_Channel4_IRQn  | 14    |
     * | 4    | DMA1_Channel5_IRQn  | 15    |
     * | 5    | DMA1_Channel6_IRQn  | 16    |
     * | 6    | DMA1_Channel7_IRQn  | 17    |
     * | 7    | DMA2_Channel1_IRQn  | 56    |
     * | 8    | DMA2_Channel2_IRQn  | 57    |
     * | 9    | DMA2_Channel3_IRQn  | 58    |
     * | 10-11| DMA2_Channel4_5_IRQn| 59    |
     */

    int channel = _dma_chnum(dma);
    if (channel < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Channel1_IRQn + channel));
    }
#if defined(DMA2)
    else if (channel < 11) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Channel1_IRQn + (channel - 7)));
    }
    else if (channel == 11) {
        NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
    }
#endif
}

static inline void dma_end_isr(dma_t dma)
{
    dma_base(dma)->IFCR = dma_ifc(_dma_chnum(dma));
}

static inline void dma_transfer(dma_t dma, dma_mode_t mode,
        void *src, void *dst, unsigned len)
{
    DMA_Channel_TypeDef *channel = dma_channel(_dma_chnum(dma));
    channel->CCR &= ~(DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_PINC | DMA_CCR_MEM2MEM);
    channel->CCR |= DMA_CCR_TCIE | DMA_CCR_MINC;

    if (mode == MEMORY_TO_PERIPH) {
        channel->CCR |= DMA_CCR_DIR;
        channel->CMAR = (uint32_t)src;
        channel->CPAR = (uint32_t)dst;
    }
    else {
        if (mode == MEMORY_TO_MEMORY) {
            channel->CCR |= DMA_CCR_MEM2MEM;
        }
        channel->CPAR = (uint32_t)src;
        channel->CMAR = (uint32_t)dst;
    }

    channel->CNDTR = (uint16_t)len;
    channel->CCR = DMA_CCR_EN;
}
#endif /* DOXYGEN */

#endif /* STM32_DMA_CHANNEL_H */
/** @} */
