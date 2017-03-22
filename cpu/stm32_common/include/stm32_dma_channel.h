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
    return (int)dma;
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
    int channel = _dma_chnum(dma);
    if (channel >= 7) {
        channel -= 7;
    }
    return (DMA_Channel_TypeDef *)(base + (0x08 + (0x14 * channel)));
}

#ifndef DOXYGEN
/**
 * @brief   Define specific DMA_UNIT generator macro
 *
 * @param[in] channel  Channel as given in the stm specification (from 1 to 12)
 */
#define DMA_UNIT(channel) ((dma_t)(channel - 1))

#define HAVE_DMA_POWERON
static inline void dma_poweron(dma_t dma)
{
#if defined(DMA2)
    if (_dma_chnum(dma) >= 7) {
        printf("second\n");
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

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32F0)
    if (channel == 0) {
        /* DMA1_Channel1_IRQn */
    }
    else if(channel < 3) {
        /* shared ISR for 2 and 3 */
        channel = 1;
    }
    else if(channel < 5) {
        /* shared ISR for 4 and 5 */
        channel = 2;
    }
#if defined(CPU_FAM_STM32L0)
    else if (channel < 7) {
        /* shared ISR for 5 to 7 */
        channel = 2;
    }
#endif /* defined(CPU_FAM_STM32L0) */
    else {
        /* channel out of range */
        return;
    }
#endif /* defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32F0) */

    if (channel < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Channel1_IRQn + channel));
    }
    else if (channel < 12) {
#if defined(DMA2)
#ifdef CPU_FAM_STM32F1
        if (channel == 11) {
            channel = 10; /* STM32F1 shared ISR line for 10 and 11 */
        }
#endif /* CPU_FAM_STM32F1 */
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Channel1_IRQn + (channel - 7)));
#endif /* defined(DMA2) */
    }
}

static inline void dma_end_isr(dma_t dma)
{
    /* clear global interrupt and tx complete */
    int channel = _dma_chnum(dma);
    if (channel >= 7) {
        channel -= 7;
    }
    dma_base(dma)->IFCR = (0x3 << (channel << 2));
}

#define HAVE_DMA_TRANSFER
static inline void dma_transfer(dma_t dma, dma_mode_t mode,
        void *src, void *dst, unsigned len)
{
    DMA_Channel_TypeDef *channel = dma_channel(dma);
    channel->CCR &= ~(DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_PINC | DMA_CCR_MEM2MEM
            | DMA_CCR_TCIE);
    channel->CCR |= DMA_CCR_TCIE | DMA_CCR_MINC;

    if (mode == MEMORY_TO_PERIPH) {
        channel->CCR |= DMA_CCR_DIR;
        channel->CMAR = (uint32_t)src;
        channel->CPAR = (uint32_t)dst;
    }
    else {
        if (mode == MEMORY_TO_MEMORY) {
            channel->CCR |= DMA_CCR_MEM2MEM | DMA_CCR_PINC;
        }
        channel->CPAR = (uint32_t)src;
        channel->CMAR = (uint32_t)dst;
    }

    channel->CNDTR = (uint16_t)len;
    channel->CCR |= DMA_CCR_EN;
}
#endif /* DOXYGEN */

#endif /* STM32_DMA_CHANNEL_H */
/** @} */
