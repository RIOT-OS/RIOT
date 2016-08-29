/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Low-level DMA driver implementation
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Pieter Willemsen <pieter.willemsen@altran.com>
 *
 * @}
 */
#include <stdio.h>
#include <stddef.h>

#include <board.h>
#include <cpu.h>
#include <mutex.h>
#include <periph/dma.h>
#include <periph_conf.h>

#include "assert.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   initialization flag
 */
#define DMA_INIT_DONE        (uint16_t)(0x0001)
static uint16_t dma_init_status = 0x0000;

/**
 * @brief   INTERRUPT FLAG MASK
 */
#define DMA_STREAM_IT_MASK   (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                        DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                        DMA_LISR_TCIF0)

/**
 * @brief   Configuration locks
 */
static mutex_t dma_conf_sync[DMA_NUMOF];

/**
 * @brief   Transmission locks
 */
static mutex_t dma_trans_sync[DMA_NUMOF];

/**
 * @brief   start transfer length (used for suspend/resume functionality)
 */
static uint16_t dma_start_length[DMA_NUMOF];

/**
 * @brief   Power on the DMA device the given stream belongs to
 *
 * @param[in] stream_dev    logical DMA stream
 */
static inline void dma_poweron(dma_t stream_dev)
{
    if (stream_dev < 8) {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }
    else {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }
    __DSB();
}

/**
 * @brief   Get DMA base register
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 * @param[in] stream_dev    logical DMA stream
 */
static inline DMA_TypeDef *dma_base(dma_t stream_dev)
{
    return (stream_dev < 8) ? DMA1 : DMA2;
}

/**
 * @brief   Get the DMA stream base address
 *
 * @param[in] stream_dev    logical DMA stream
 *
 * @return  base address for the selected DMA stream
 */
static inline DMA_Stream_TypeDef *dma_stream(dma_t stream_dev)
{
    uint32_t base = (uint32_t)dma_base(stream_dev);

    return (DMA_Stream_TypeDef *)(base + (0x10 + (0x18 * (stream_dev & 0x7))));
}

/**
 * @brief   Select high or low DMA interrupt register based on stream number
 *
 * @param[in] stream_dev    logical DMA stream
 *
 * @return  0 for streams 0-3, 1 for streams 3-7
 */
static inline int dma_hl(dma_t stream_dev)
{
    return ((stream_dev & 0x4) >> 2);
}

/**
 * @brief   Get the interrupt flag clear bit position in the DMA LIFCR register
 *
 * @param[in] stream_dev    logical DMA stream
 */
static inline uint32_t dma_ifc(dma_t stream_dev)
{
    switch (stream_dev & 0x3) {
        case 0: /* 0 and 4 */
            return (1 << 5);
        case 1: /* 1 and 5 */
            return (1 << 11);
        case 2: /* 2 and 6 */
            return (1 << 21);
        case 3: /* 3 and 7 */
            return (1 << 27);
        default:
            return 0;
    }
}

/**
 * @brief   enable interrupt of the given DMA device
 *
 * @param[in] stream_dev    logical DMA stream
 */
static inline void dma_isr_enable(dma_t stream_dev)
{
    if (stream_dev < 7) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream_dev));
    }
    else if (stream_dev == 7) {
        NVIC_EnableIRQ((IRQn_Type)DMA1_Stream7_IRQn);
    }
    else if (stream_dev < 13) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream_dev - 8)));
    }
    else if (stream_dev < 16) {
        NVIC_EnableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream_dev - 13)));
    }
}

static inline void dma_isr_disable(dma_t stream_dev)
{
    if (stream_dev < 7) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream_dev));
    }
    else if (stream_dev == 7) {
        NVIC_DisableIRQ((IRQn_Type)DMA1_Stream7_IRQn);
    }
    else if (stream_dev < 13) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream_dev - 8)));
    }
    else if (stream_dev < 16) {
        NVIC_DisableIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream_dev - 13)));
    }
}

static inline void dma_isr_clear(dma_t stream_dev)
{
    if (stream_dev < 7) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA1_Stream0_IRQn + stream_dev));
    }
    else if (stream_dev == 7) {
        NVIC_ClearPendingIRQ((IRQn_Type)DMA1_Stream7_IRQn);
    }
    else if (stream_dev < 13) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA2_Stream0_IRQn + (stream_dev - 8)));
    }
    else if (stream_dev < 16) {
        NVIC_ClearPendingIRQ((IRQn_Type)((int)DMA2_Stream5_IRQn + (stream_dev - 13)));
    }
}

/**
 * @brief   Get all interrupt flag clear bits position in the DMA LIFCR register
 *
 * @param[in] stream_dev    logical DMA stream
 */
static inline uint32_t dma_all_flags(dma_t stream_dev)
{
    switch (stream_dev & 0x3) {
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

/**
 * @brief clear all flag to the giver DMA
 *
 * @param[out] stream_dev       DMA device to configure
 *
 */
static void dma_clear_all_flags(dma_t stream_dev)
{
    DMA_TypeDef *stream_base = dma_base(stream_dev);

    /* Clear all flags */
    if (dma_hl(stream_dev) == 0) {
        stream_base->LIFCR = dma_all_flags(stream_dev);
    }
    else {
        stream_base->HIFCR = dma_all_flags(stream_dev);
    }
}

int dma_conf_acquire(dma_t stream_dev)
{
    if (stream_dev >= DMA_NUMOF) {
        return -1;
    }
    mutex_lock(&dma_conf_sync[stream_dev]);
    return 0;
}

int dma_conf_release(dma_t stream_dev)
{
    if (stream_dev >= DMA_NUMOF) {
        return -1;
    }
    mutex_unlock(&dma_conf_sync[stream_dev]);
    return 0;
}

int dma_transmission_acquire(dma_t stream_dev)
{
    if (stream_dev >= DMA_NUMOF) {
        return -1;
    }
    mutex_lock(&dma_trans_sync[stream_dev]);
    return 0;
}

int dma_transmission_release(dma_t stream_dev)
{
    if (stream_dev >= DMA_NUMOF) {
        return -1;
    }
    mutex_unlock(&dma_trans_sync[stream_dev]);
    return 0;
}

void dma_stream_init(dma_t stream_dev)
{
    DMA_Stream_TypeDef *stream = dma_stream(stream_dev);

    /* check init already done */
    if (((dma_init_status >> stream_dev) & DMA_INIT_DONE) != DMA_INIT_DONE) {
        dma_init_status |= DMA_INIT_DONE << stream_dev;

        /* check if EN bit is set to clear it */
        if ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {
            /* disable the selected DMA Stream by clearing EN bit */
            stream->CR &= ~(uint32_t)DMA_SxCR_EN;

            /* Wait until the stream is ready to be configured */
            while ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {}
        }

        /* init all registers */
        stream->CR = 0;
        stream->PAR = 0;
        stream->NDTR = 0;
        stream->M0AR = 0;
        stream->M1AR = 0;
        stream->FCR = 0;

        /* clear all flags */
        dma_clear_all_flags(stream_dev);

        /* init configuration mutex */
        mutex_init(&dma_conf_sync[stream_dev]);

        /* init transmission mutex */
        mutex_init(&dma_trans_sync[stream_dev]);

        /* lock transmission mutex */
        dma_transmission_acquire(stream_dev);
    }
}

void dma_stream_config(dma_t stream_dev, uint32_t periph_addr_reg, uint32_t dma_config, void *data, uint16_t length)
{
    assert(data != NULL);
    DMA_Stream_TypeDef *stream = dma_stream(stream_dev);

    /* power on */
    dma_poweron(stream_dev);

    /* peripheral address register */
    stream->PAR = periph_addr_reg;

    /* configure the FIFO usage */
    stream->FCR = 0;

    /* configure dma stream */
    stream->CR = dma_config;

    /* set the memory address in the DMA_SxMA0R register */
    stream->M0AR = (uint32_t)data;

    /* configure the total number of data items to be transferred */
    stream->NDTR = length;

    /* save total number of data items */
    dma_start_length[stream_dev] = length;

    /* enable interrupt */
    dma_isr_enable(stream_dev);
}

void dma_enable(dma_t stream_dev)
{
    DMA_Stream_TypeDef *stream = dma_stream(stream_dev);

    /* enable the selected DMA Stream by setting EN bit */
    stream->CR |= (uint32_t)DMA_SxCR_EN;
    while( (stream->CR & DMA_SxCR_EN) != DMA_SxCR_EN){}
}

void dma_disable(dma_t stream_dev)
{
    DMA_Stream_TypeDef *stream = dma_stream(stream_dev);

    /* disable the selected DMA Stream by setting EN bit */
    stream->CR &= ~(uint32_t)DMA_SxCR_EN;
}

void dma_clear_ifc_flag(dma_t stream_dev)
{
    DMA_TypeDef *stream_base = dma_base(stream_dev);

    /* clear DMA flag */
    if (dma_hl(stream_dev) == 0) {
        stream_base->LIFCR = dma_ifc(stream_dev);
    }
    else {
        stream_base->HIFCR = dma_ifc(stream_dev);
    }
}

uint16_t dma_suspend(dma_t stream_dev)
{
    DMA_Stream_TypeDef *stream = dma_stream(stream_dev);
    uint16_t left = 0;
    
    if ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {
        dma_isr_disable(stream_dev);
        stream->CR &= ~(uint32_t)DMA_SxCR_EN;
        while ((stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN) {}
        dma_clear_all_flags(stream_dev);
        left = stream->NDTR;
        dma_isr_clear(stream_dev);
    }
    return left;
}

void dma_resume(dma_t stream_dev, uint16_t todo)
{
    if(todo > 0) {
        DMA_Stream_TypeDef *stream = dma_stream(stream_dev);
        dma_isr_enable(stream_dev);
        stream->NDTR = todo; 
        stream->M0AR += dma_start_length[stream_dev] - todo;
        stream->CR |= (uint32_t)DMA_SxCR_EN;
    }
}

static inline void dma_handler(dma_t stream_dev)
{
    /* clear interrupt flags */
    dma_clear_all_flags(stream_dev);

    /* unblock thread */
    dma_transmission_release(stream_dev);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**
 * Interruption for all DMA stream and channel
 */
void isr_dma1_stream0(void)
{
    dma_handler((dma_t)0);
}
void isr_dma1_stream1(void)
{
    dma_handler((dma_t)1);
}
void isr_dma1_stream2(void)
{
    dma_handler((dma_t)2);
}
void isr_dma1_stream3(void)
{
    dma_handler((dma_t)3);
}
void isr_dma1_stream4(void)
{
    dma_handler((dma_t)4);
}
void isr_dma1_stream5(void)
{
    dma_handler((dma_t)5);
}
void isr_dma1_stream6(void)
{
    dma_handler((dma_t)6);
}
void isr_dma1_stream7(void)
{
    dma_handler((dma_t)7);
}
void isr_dma2_stream0(void)
{
    dma_handler((dma_t)8);
}
void isr_dma2_stream1(void)
{
    dma_handler((dma_t)9);
}
void isr_dma2_stream2(void)
{
    dma_handler((dma_t)10);
}
void isr_dma2_stream3(void)
{
    dma_handler((dma_t)11);
}
void isr_dma2_stream4(void)
{
    dma_handler((dma_t)12);
}
void isr_dma2_stream5(void)
{
    dma_handler((dma_t)13);
}
void isr_dma2_stream6(void)
{
    dma_handler((dma_t)14);
}
void isr_dma2_stream7(void)
{
    dma_handler((dma_t)15);
}
