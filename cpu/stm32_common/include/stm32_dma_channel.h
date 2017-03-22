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
 * @brief   Get the DMA stream base address
 *
 * @param[in] channel   logical DMA stream
 *
 * @return  base address for the selected DMA channel
 */
static inline DMA_Channel_TypeDef *dma_channel(dma_t dma)
{
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

#define HAVE_DMA_POWERON
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

#define HAVE_DMA_POWEROFF
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

#define HAVE_DMA_ISR_ENABLE
static inline void dma_isr_enable(dma_t dma)
{
    int channel = _dma_chnum(dma);
    if (channel < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Channel1_IRQn + channel));
    }
    else if (channel < 12) {
#if defined(DMA2)
#if CPU_FAM_STM32F1
        if (channel == 11) {
            channel = 10; /* STM32F1 shares ISR line for 10 and 11 */
        }
#endif /* CPU_FAM_STM32F1 */
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Channel1_IRQn + (channel - 7)));
#endif /* defined(DMA2) */
    }
}

static inline void dma_end_isr(dma_t dma)
{
    dma_base(dma)->IFCR = dma_ifc(_dma_chnum(dma));
}

#define HAVE_DMA_TRANSFER
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
