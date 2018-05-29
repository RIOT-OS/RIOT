/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level DMA driver implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "mutex.h"
#include "assert.h"

#if !(defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7))
#error "DMA is not supported for target CPU"
#endif

#define DMA_STREAM_IT_MASK   (DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                              DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                              DMA_LISR_TCIF0)

struct dma_ctx {
    mutex_t conf_lock;
    mutex_t sync_lock;
    uint16_t len;
};

static struct dma_ctx dma_ctx[DMA_NUMOF];

static inline uint32_t dma_all_flags(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    switch (dma_config[dma].stream & 0x3) {
        case 0: /* 0 and 4 */
            return (DMA_STREAM_IT_MASK);
        case 1: /* 1 and 5 */
            return (DMA_STREAM_IT_MASK << 6);
        case 2: /* 2 and 6 */
            return (DMA_STREAM_IT_MASK << 16);
        case 3: /* 3 and 7 */
            return (DMA_STREAM_IT_MASK << 22);
        default:
            return 0;
    }
}

static void dma_clear_all_flags(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    DMA_TypeDef *stream = dma_base(dma_config[dma].stream);

    /* Clear all flags */
    if (dma_hl(dma_config[dma].stream) == 0) {
        stream->LIFCR = dma_all_flags(dma);
    }
    else {
        stream->HIFCR = dma_all_flags(dma);
    }
}


void dma_init(void)
{
    for (unsigned i = 0; i < DMA_NUMOF; i++) {
        mutex_init(&dma_ctx[i].conf_lock);
        mutex_init(&dma_ctx[i].sync_lock);
        mutex_lock(&dma_ctx[i].sync_lock);
    }
}

int dma_transfer(dma_t dma, int chan, const void *src, void *dst, size_t len,
                 dma_mode_t mode, uint8_t flags)
{
    int ret = dma_configure(dma, chan, src, dst, len, mode, flags);
    if (ret != 0) {
        return ret;
    }
    dma_start(dma);
    dma_wait(dma);
    dma_stop(dma);

    return len;
}

void dma_acquire(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    mutex_lock(&dma_ctx[dma].conf_lock);
}

void dma_release(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    mutex_unlock(&dma_ctx[dma].conf_lock);
}

int dma_configure(dma_t dma, int chan, const void *src, void *dst, size_t len,
                  dma_mode_t mode, uint8_t flags)
{
    assert(src != NULL);
    assert(dst != NULL);
    assert(dma < DMA_NUMOF);

    int stream_n = dma_config[dma].stream;
    uint32_t inc_periph;
    uint32_t inc_mem;

    DMA_Stream_TypeDef *stream = dma_stream(stream_n);
    dma_poweron(stream_n);
    dma_clear_all_flags(dma);

    switch (mode) {
        case DMA_MEM_TO_MEM:
        case DMA_PERIPH_TO_MEM:
            stream->PAR = (uint32_t)src;
            stream->M0AR = (uint32_t)dst;
            inc_periph = (flags & DMA_INC_SRC_ADDR);
            inc_mem = (flags & DMA_INC_DST_ADDR) >> 1;
            break;
        case DMA_MEM_TO_PERIPH:
            stream->PAR = (uint32_t)dst;
            stream->M0AR = (uint32_t)src;
            inc_periph = (flags & DMA_INC_DST_ADDR) >> 1;
            inc_mem = (flags & DMA_INC_SRC_ADDR);
            break;
        default:
            return -1;
    }

    uint32_t width = (flags & DMA_DATA_WIDTH_MASK) >> DMA_DATA_WIDTH_SHIFT;
    /* Set channel, data width, inc and mode */
    stream->CR = (chan & 0xF) << DMA_SxCR_CHSEL_Pos |
                 width << DMA_SxCR_MSIZE_Pos | width << DMA_SxCR_PSIZE_Pos |
                 inc_periph << DMA_SxCR_PINC_Pos | inc_mem << DMA_SxCR_MINC_Pos |
                 (mode & 3) << DMA_SxCR_DIR_Pos;
    /* Enable interrupts */
    stream->CR |= DMA_SxCR_TCIE | DMA_SxCR_TEIE;
    /* Configure FIFO */
    stream->FCR = 0;

    /* Set length */
    stream->NDTR = len;
    dma_ctx[dma].len = len;

    dma_isr_enable(stream_n);

    return 0;
}

void dma_start(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    DMA_Stream_TypeDef *stream = dma_stream(dma_config[dma].stream);

    stream->CR |= DMA_SxCR_EN;
}

uint16_t dma_suspend(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    int stream_n = dma_config[dma].stream;
    DMA_Stream_TypeDef *stream = dma_stream(stream_n);
    uint16_t left = 0;

    if ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {
        dma_isr_disable(stream_n);
        stream->CR &= ~(uint32_t)DMA_SxCR_EN;
        while ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {}
        dma_clear_all_flags(dma);
        left = stream->NDTR;
        dma_isr_clear(stream_n);
    }
    return left;

}

void dma_resume(dma_t dma, uint16_t remaining)
{
    assert(dma < DMA_NUMOF);

    int stream_n = dma_config[dma].stream;
    DMA_Stream_TypeDef *stream = dma_stream(stream_n);

    if (remaining > 0) {
        dma_isr_enable(stream_n);
        stream->NDTR = remaining;
        stream->M0AR += dma_ctx[dma].len - remaining;
        dma_ctx[dma].len = remaining;
        stream->CR |= (uint32_t)DMA_SxCR_EN;
    }
}

void dma_stop(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    DMA_Stream_TypeDef *stream = dma_stream(dma_config[dma].stream);

    stream->CR &= ~(uint32_t)DMA_SxCR_EN;
}

void dma_wait(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    mutex_lock(&dma_ctx[dma].sync_lock);
}

void dma_isr_handler(dma_t dma)
{
    dma_clear_all_flags(dma);

    mutex_unlock(&dma_ctx[dma].sync_lock);

    cortexm_isr_end();
}

#ifdef DMA_0_ISR
void DMA_0_ISR(void)
{
    dma_isr_handler(0);
}
#endif

#ifdef DMA_1_ISR
void DMA_1_ISR(void)
{
    dma_isr_handler(1);
}
#endif

#ifdef DMA_2_ISR
void DMA_2_ISR(void)
{
    dma_isr_handler(2);
}
#endif

#ifdef DMA_3_ISR
void DMA_3_ISR(void)
{
    dma_isr_handler(3);
}
#endif

#ifdef DMA_4_ISR
void DMA_4_ISR(void)
{
    dma_isr_handler(4);
}
#endif

#ifdef DMA_5_ISR
void DMA_5_ISR(void)
{
    dma_isr_handler(5);
}
#endif

#ifdef DMA_6_ISR
void DMA_6_ISR(void)
{
    dma_isr_handler(6);
}
#endif

#ifdef DMA_7_ISR
void DMA_7_ISR(void)
{
    dma_isr_handler(7);
}
#endif

#ifdef DMA_8_ISR
void DMA_8_ISR(void)
{
    dma_isr_handler(8);
}
#endif

#ifdef DMA_9_ISR
void DMA_9_ISR(void)
{
    dma_isr_handler(9);
}
#endif
