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
 * @brief           DMA stream implementation
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Víctor Ariño <victor.arino@zii.aero>
 *
 * @}
 */
#include "cpu.h"
#include "periph/dma.h"

#if defined(DMA_STREAM) || defined(CPU_FAM_STM32L4)
#define DMA_BUS AHB1
#define DMA1_MASK RCC_AHB1ENR_DMA1EN
#define DMA2_MASK RCC_AHB1ENR_DMA2EN
#else
#define DMA_BUS AHB
#define DMA1_MASK RCC_AHBENR_DMA1EN
#define DMA2_MASK RCC_AHBENR_DMA2EN
#endif

#if DMA_STREAM
#define DMA1_UNITS 8
#define DMA2_UNITS 12
#define DMA1_IRQn_TYPE DMA1_Stream0_IRQn
#define DMA2_IRQn_TYPE DMA2_Stream0_IRQn
#else
#define DMA1_UNITS 7
#define DMA2_UNITS 13
#define DMA1_IRQn_TYPE DMA1_Channel1_IRQn
#define DMA2_IRQn_TYPE DMA2_Channel1_IRQn
#endif


void dma_poweron(dma_t dma)
{
    if (dma_unit(dma) < DMA1_UNITS) {
        periph_clk_en(DMA_BUS, DMA1_MASK);
    }
#ifdef DMA2
    else {
        periph_clk_en(DMA_BUS, DMA2_MASK);
    }
#endif
}

void dma_poweroff(dma_t dma)
{
    if (dma_unit(dma) < DMA1_UNITS) {
        periph_clk_dis(DMA_BUS, DMA1_MASK);
    }
#ifdef DMA2
    else {
        periph_clk_dis(DMA_BUS, DMA2_MASK);
    }
#endif
}

void dma_isr_enable(dma_t dma)
{
    int unit = dma_unit(dma);

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32F0)
    if (unit == 0) {
        /* DMA1_Channel1_IRQn */
    }
    else if(unit < 3) {
        unit = 1; /* shared ISR for 2 and 3 */
    }
    else if(unit < 5) {
        unit = 2; /* shared ISR for 4 and 5 */
    }
#if defined(CPU_FAM_STM32L0)
    else if (unit < 7) {
        /* shared ISR for 5 to 7 */
        unit = 2;
    }
#endif /* defined(CPU_FAM_STM32L0) */
    else {
        /* channel out of range */
        return;
    }
#endif /* defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32F0) */

    if (unit < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_IRQn_TYPE + unit));
    }
#if DMA_STREAM
    else if (unit == 7) {
        NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    }
#endif /* DMA_STREAM */
#ifdef DMA2
    else if (unit < DMA2_UNITS) {
#ifdef CPU_FAM_STM32F1
        if (unit == 11) {
            unit = 10; /* STM32F1 shared ISR for DMA2 Ch 4 and 5 */
        }
#endif /* CPU_FAM_STM32F1 */
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_IRQn_TYPE + (unit - DMA1_UNITS)));
    }
#if DMA_STREAM
    else if (unit < 16) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (unit - 13)));
    }
#endif /* DMA_STREAM */
#endif /* DMA2 */
}

#ifdef DMA_STREAM

void dma_transfer(dma_t dma, dma_mode_t mode,
        void *src, void *dst, unsigned len)
{
    DMA_Stream_TypeDef *stream = dma_stream(dma);
    stream->CR &= ~(DMA_SxCR_CHSEL | DMA_SxCR_DIR | DMA_SxCR_EN | DMA_SxCR_PINC);
    stream->CR |= (dma_chnum(dma) << 25) | (mode << 6) | DMA_SxCR_MINC
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
    stream->CR |= DMA_SxCR_EN;
}

#else

void dma_transfer(dma_t dma, dma_mode_t mode,
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

#endif /* DMA_STREAM */
