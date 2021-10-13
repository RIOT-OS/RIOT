/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
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
#include "pm_layered.h"

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
#define STM32_DMA_Stream_Type   DMA_Stream_TypeDef
#define CLOCK                   AHB1
#define PERIPH_ADDR             PAR
#define MEM_ADDR                M0AR
#define NDTR_REG                NDTR
#define CONTROL_REG             CR
#define RCC_MASK_DMA1           RCC_AHB1ENR_DMA1EN
#define RCC_MASK_DMA2           RCC_AHB1ENR_DMA2EN
#define DMA_STREAM_IT_MASK      (DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                 DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                 DMA_LISR_TCIF0)
#define DMA_EN                  DMA_SxCR_EN
#else /* CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7 */
#define STM32_DMA_Stream_Type   DMA_Channel_TypeDef
#if CPU_FAM_STM32L4
#define CLOCK                   AHB1
#define RCC_MASK_DMA1           RCC_AHB1ENR_DMA1EN
#define RCC_MASK_DMA2           RCC_AHB1ENR_DMA2EN
#else /* CPU_FAM_STM32L4 */
#define CLOCK                   AHB
#if CPU_FAM_STM32F1 || CPU_FAM_STM32F3 || CPU_FAM_STM32L1
#define RCC_MASK_DMA1           RCC_AHBENR_DMA1EN
#else /* CPU_FAM_STM32F1 || CPU_FAM_STM32F3 || CPU_FAM_STM32L1 */
#define RCC_MASK_DMA1           RCC_AHBENR_DMAEN
#endif /* CPU_FAM_STM32F1 || CPU_FAM_STM32F3 || CPU_FAM_STM32L1 */
#define RCC_MASK_DMA2           RCC_AHBENR_DMA2EN
#endif /* CPU_FAM_STM32L4 */
#define PERIPH_ADDR             CPAR
#define MEM_ADDR                CMAR
#define NDTR_REG                CNDTR
#define CONTROL_REG             CCR
#define DMA_EN                  DMA_CCR_EN
#define DMA_STREAM_IT_MASK      (DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | \
                                 DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1)
#ifndef DMA_CCR_MSIZE_Pos
#define DMA_CCR_MSIZE_Pos       (10)
#endif
#ifndef DMA_CCR_PSIZE_Pos
#define DMA_CCR_PSIZE_Pos       (8)
#endif
#ifndef DMA_CCR_MINC_Pos
#define DMA_CCR_MINC_Pos        (7)
#endif
#ifndef DMA_CCR_PINC_Pos
#define DMA_CCR_PINC_Pos        (6)
#endif
#ifndef DMA_CCR_DIR_Pos
#define DMA_CCR_DIR_Pos         (4)
#endif
#ifndef DMA_CCR_MEM2MEM_Pos
#define DMA_CCR_MEM2MEM_Pos     (14)
#endif
#if defined(CPU_FAM_STM32F0) && !defined(DMA1_Channel4_5_6_7_IRQn)
#define DMA1_Channel4_5_6_7_IRQn    DMA1_Channel4_5_IRQn
#endif
#endif /* CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7 */

struct dma_ctx {
    STM32_DMA_Stream_Type *stream;
    mutex_t conf_lock;
    mutex_t sync_lock;
    uint16_t len;
};

static struct dma_ctx dma_ctx[DMA_NUMOF];

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 * @param[in] stream    physical DMA stream
 */
static inline DMA_TypeDef *dma_base(int stream)
{
#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    return (stream < 8) ? DMA1 : DMA2;
#elif defined(DMA2)
    return (stream < 7) ? DMA1 : DMA2;
#else
    (void)stream;
    return DMA1;
#endif
}

#if CPU_FAM_STM32F0 || CPU_FAM_STM32F3
static inline DMA_TypeDef *dma_req(int stream_n)
{
    return dma_base(stream_n);
}
#elif CPU_FAM_STM32L0 || CPU_FAM_STM32L4 || CPU_FAM_STM32G0
static inline DMA_Request_TypeDef *dma_req(int stream_n)
{
#ifdef DMA2
    return (stream_n < 7) ? DMA1_CSELR : DMA2_CSELR;
#else
    (void)stream_n;
    return DMA1_CSELR;
#endif
}
#endif

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] stream    physical DMA stream
 *
 * @return  base address for the selected DMA stream
 */
static inline STM32_DMA_Stream_Type *dma_stream(int stream)
{
    uint32_t base = (uint32_t)dma_base(stream);

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (stream & 0x7))));
#else
    return (DMA_Channel_TypeDef *)(base + (0x08 + (0x14 * (stream & 0x7))));
#endif
}

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] stream    physical DMA stream
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(int stream)
{
    return ((stream & 0x4) >> 2);
}
#endif

static IRQn_Type dma_get_irqn(int stream)
{
#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    if (stream < 7) {
        return ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream));
    }
    else if (stream == 7) {
        return DMA1_Stream7_IRQn;
    }
    else if (stream < 13) {
        return ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream - 8)));
    }
    else if (stream < 16) {
        return ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream - 13)));
    }
#elif CPU_FAM_STM32F0 || CPU_FAM_STM32L0 || CPU_FAM_STM32G0
    if (stream == 0) {
        return (DMA1_Channel1_IRQn);
    }
    else if (stream < 3 || (stream >= 8 && stream < 11)) {
        return (DMA1_Channel2_3_IRQn);
    }
    else if (stream < 7 || stream >= 11) {
        return (DMA1_Channel4_5_6_7_IRQn);
    }
#else
    if (stream < 7) {
        return ((IRQn_Type)((int)DMA1_Channel1_IRQn + stream));
    }
#if defined(DMA2_BASE)
#if defined(CPU_FAM_STM32F1)
    else if (stream < 11) {
#else
    else if (stream < 13 ) {
#endif
        return ((IRQn_Type)((int)DMA2_Channel1_IRQn + stream));
    }
#if !defined(CPU_FAM_STM32L1)
    else {
#if defined(CPU_FAM_STM32F1)
        return (DMA2_Channel4_5_IRQn);
#else
        return ((IRQn_Type)((int)DMA2_Channel6_IRQn + stream));
#endif
    }
#endif /* !defined(CPU_FAM_STM32L1) */
#endif /* defined(DMA2_BASE) */
#endif

    return -1;
}

/**
 * @brief   Disable the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_disable(int stream)
{
    NVIC_DisableIRQ(dma_get_irqn(stream));
}

/**
 * @brief   Clear the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_clear(int stream)
{
    NVIC_ClearPendingIRQ(dma_get_irqn(stream));
}

/**
 * @brief   Enable the interrupt of a given stream
 *
 * @param[in] stream    physical DMA stream
 */
static inline void dma_isr_enable(int stream)
{
    NVIC_EnableIRQ(dma_get_irqn(stream));
}

static inline uint32_t dma_all_flags(dma_t dma)
{
#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
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
#else
    return DMA_STREAM_IT_MASK << ((dma_config[dma].stream & 0x7) * 4);
#endif
}

static void dma_clear_all_flags(dma_t dma)
{
    DMA_TypeDef *dma_dev = dma_base(dma_config[dma].stream);

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    /* Clear all flags */
    if (dma_hl(dma_config[dma].stream) == 0) {
        dma_dev->LIFCR = dma_all_flags(dma);
    }
    else {
        dma_dev->HIFCR = dma_all_flags(dma);
    }
#else
    dma_dev->IFCR = dma_all_flags(dma);
#endif
}

static void dma_poweron(int stream)
{
    if (stream < 8) {
        periph_clk_en(CLOCK, RCC_MASK_DMA1);
    }
#if defined(DMA2)
    else {
        periph_clk_en(CLOCK, RCC_MASK_DMA2);
    }
#endif
}

void dma_init(void)
{
    for (unsigned i = 0; i < DMA_NUMOF; i++) {
        mutex_init(&dma_ctx[i].conf_lock);
        mutex_init(&dma_ctx[i].sync_lock);
        mutex_lock(&dma_ctx[i].sync_lock);
        int stream_n = dma_config[i].stream;
        dma_poweron(stream_n);
        dma_isr_enable(stream_n);
        dma_ctx[i].stream = dma_stream(stream_n);
    }
}

int dma_transfer(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
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

    dma_clear_all_flags(dma);

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;
    stream->FCR = 0;
#endif

#ifdef STM32_PM_STOP
    /* block STOP mode */
    pm_block(STM32_PM_STOP);
#endif
}

void dma_release(dma_t dma)
{
    assert(dma < DMA_NUMOF);

#ifdef STM32_PM_STOP
    /* unblock STOP mode */
    pm_unblock(STM32_PM_STOP);
#endif
    mutex_unlock(&dma_ctx[dma].conf_lock);
}

void dma_setup(dma_t dma, int chan, void *periph_addr, dma_mode_t mode,
               uint8_t width, bool inc_periph)
{
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    /* Set channel, data width, inc and mode */
    uint32_t cr_settings = (chan & 0xF) << DMA_SxCR_CHSEL_Pos |
                           (width << DMA_SxCR_MSIZE_Pos) |
                           (width << DMA_SxCR_PSIZE_Pos) |
                           (inc_periph << DMA_SxCR_PINC_Pos) |
                           (mode & 3) << DMA_SxCR_DIR_Pos |
                           DMA_SxCR_TCIE |
                           DMA_SxCR_TEIE;
    /* Configure FIFO */
    stream->CONTROL_REG  = cr_settings;
#else
#if defined(DMA_CSELR_C1S) || defined(DMA1_CSELR_DEFAULT)
    int stream_n = dma_config[dma].stream;
    dma_req(stream_n)->CSELR &= ~((0xF) << ((stream_n & 0x7) << 2));
    dma_req(stream_n)->CSELR |= (chan & 0xF) << ((stream_n & 0x7) << 2);
#else
    (void)chan;
#endif
    uint32_t ctr_reg = (width << DMA_CCR_MSIZE_Pos) |
                       (width << DMA_CCR_PSIZE_Pos) |
                       (inc_periph << DMA_CCR_PINC_Pos) |
                       (mode & 1) << DMA_CCR_DIR_Pos |
                       ((mode & 2) >> 1) << DMA_CCR_MEM2MEM_Pos |
                       DMA_CCR_TCIE |
                       DMA_CCR_TEIE;
    stream->CONTROL_REG = ctr_reg;
#endif
    stream->PERIPH_ADDR = (uint32_t)periph_addr;
}

void dma_prepare(dma_t dma, void *mem, size_t len, bool incr_mem)
{
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;
    uint32_t ctr_reg = stream->CONTROL_REG;

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    stream->CONTROL_REG = (ctr_reg & ~(DMA_SxCR_MINC)) |
                          (incr_mem << DMA_SxCR_MINC_Pos);
#else
    stream->CONTROL_REG = (ctr_reg & ~(DMA_CCR_MINC)) |
                          (incr_mem << DMA_CCR_MINC_Pos);
#endif
    stream->MEM_ADDR = (uint32_t)mem;

    /* Set length */
    stream->NDTR_REG = len;
    dma_ctx[dma].len = len;
}

int dma_configure(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
                  dma_mode_t mode, uint8_t flags)
{
    assert(src != NULL);
    assert(dst != NULL);

    bool inc_periph;
    bool inc_mem;
    void *periph_addr;
    void *mem_addr;

    switch (mode) {
        case DMA_MEM_TO_MEM:
        case DMA_PERIPH_TO_MEM:
            periph_addr = (void*)src;
            mem_addr = (void*)dst;
            inc_periph = (flags & DMA_INC_SRC_ADDR);
            inc_mem = (flags & DMA_INC_DST_ADDR);
            break;
        case DMA_MEM_TO_PERIPH:
            periph_addr = (void*)dst;
            /* This discards the const specifier which should be fine as the DMA
             * stream promises not to write to this location */
            mem_addr = (void*)src;
            inc_periph = (flags & DMA_INC_DST_ADDR);
            inc_mem = (flags & DMA_INC_SRC_ADDR);
            break;
        default:
            return -1;
    }

    uint32_t width = (flags & DMA_DATA_WIDTH_MASK) >> DMA_DATA_WIDTH_SHIFT;

    dma_setup(dma, chan, periph_addr, mode, width, inc_periph);
    dma_prepare(dma, mem_addr, len, inc_mem);

    return 0;
}

void dma_start(dma_t dma)
{
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;

    stream->CONTROL_REG |= DMA_EN;
}

uint16_t dma_suspend(dma_t dma)
{
    assert(dma < DMA_NUMOF);

    int stream_n = dma_config[dma].stream;
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;
    uint16_t left = 0;

    if ((stream->CONTROL_REG & DMA_EN) == DMA_EN) {
        dma_isr_disable(stream_n);
        stream->CONTROL_REG &= ~(uint32_t)DMA_EN;
        while ((stream->CONTROL_REG & DMA_EN) == DMA_EN) {}
        dma_clear_all_flags(dma);
        left = stream->NDTR_REG;
        dma_isr_clear(stream_n);
    }
    return left;

}

void dma_resume(dma_t dma, uint16_t remaining)
{
    assert(dma < DMA_NUMOF);

    int stream_n = dma_config[dma].stream;
    STM32_DMA_Stream_Type *stream = dma_ctx[dma].stream;

    if (remaining > 0) {
        dma_isr_enable(stream_n);
        stream->NDTR_REG = remaining;
        stream->MEM_ADDR += dma_ctx[dma].len - remaining;
        dma_ctx[dma].len = remaining;
        stream->CONTROL_REG |= DMA_EN;
    }
}

void dma_stop(dma_t dma)
{
    STM32_DMA_Stream_Type *stream = dma_stream(dma_config[dma].stream);

    stream->CONTROL_REG &= ~(uint32_t)DMA_EN;
}

void dma_wait(dma_t dma)
{
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

#if defined(DMA_SHARED_ISR_0) || defined(DMA_SHARED_ISR_1)
static int dma_is_isr(dma_t dma)
{
    DMA_TypeDef *dma_dev = dma_base(dma_config[dma].stream);

#if CPU_FAM_STM32F2 || CPU_FAM_STM32F4 || CPU_FAM_STM32F7
    /* Clear all flags */
    if (dma_hl(dma_config[dma].stream) == 0) {
        return dma_dev->LISR & dma_all_flags(dma);
    }
    else {
        return dma_dev->HISR & dma_all_flags(dma);
    }
#else
    return dma_dev->ISR & dma_all_flags(dma);
#endif
}

static void shared_isr(uint8_t *streams, size_t nb)
{
    for (size_t i = 0; i < nb; i++) {
        dma_t dma = streams[i];
        if (dma_is_isr(dma)) {
            dma_clear_all_flags(dma);
            mutex_unlock(&dma_ctx[dma].sync_lock);
        }
    }

    cortexm_isr_end();
}
#endif

#ifdef DMA_SHARED_ISR_0
void DMA_SHARED_ISR_0(void)
{
    uint8_t streams[] = DMA_SHARED_ISR_0_STREAMS;
    shared_isr(streams, ARRAY_SIZE(streams));
}
#endif

#ifdef DMA_SHARED_ISR_1
void DMA_SHARED_ISR_1(void)
{
    uint8_t streams[] = DMA_SHARED_ISR_1_STREAMS;
    shared_isr(streams, ARRAY_SIZE(streams));
}
#endif
