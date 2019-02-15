/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fr59xx
 * @ingroup     drivers_periph_dma
 * @{
 *
 * @file
 * @brief       Low-level direct memory access driver implementation
 *
 * @author      Tristan Delizy <tristan.delizy@inria.fr>
 * @author      Loïc Saos <loic.saos@insa-lyon.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph_dma.h"

void *dma_memset(void *dst, unsigned char val, size_t len)
{
    if (len == 0) {
        return NULL;
    }

    /* reset control register */
    DMA_CHAN_0->CTL = 0;

    /* use DMAREQ as trigger*/
    DMA_MODULE->CTL0 |= DMA_CTL0_SEL_REQ;

    DMA_CHAN_0->SA = (size_t)&val;
    DMA_CHAN_0->DA = (size_t)dst;
    DMA_CHAN_0->SZ = len;

    DMA_CHAN_0->CTL = DMA_CC_DT_BLOCK       /* Block transfer mode     */
                      | DMA_CC_DST_INCR     /* Increment dst address   */
                      | DMA_CC_SRC_CONST    /* Constant src address    */
                      | DMA_CC_EN           /* Enable DMA              */
                      | DMA_CC_REQ          /* Request DMA (runs copy) */
                      | DMA_CC_DST_BYTE     /* dst: byte unit          */
                      | DMA_CC_SRC_BYTE;    /* src: byte unit          */

    return dst;
}

void *dma_memcpy(void *dst, const void *src, size_t len)
{
    unsigned int copymode = 0; /* word unit for dst and src */
    size_t dma0sz;
    void *initial_dst = dst;

    if (!len) {
        return NULL;
    }

    if (((size_t)dst & 0x1) && ((size_t)src & 0x1)) {
        char *cdst = (char *)dst;
        char *csrc = (char *)src;
        /* Both dst and src are odd, copy one byte and copy the rest */
        /* word-aligned                                              */
        *cdst++ = *csrc++;
        --len;

        // word transfer
        dma0sz = len / 2;
    }
    else if (((size_t)dst & 0x1) || (size_t)src & 0x1) {
        /* dst and src have opposite parity, they cannot be word-copied. */
        /* Use byte copy instead                                         */
        copymode = DMA_CC_DST_BYTE      /* dst: byte unit                    */
                   | DMA_CC_SRC_BYTE;   /* src: byte unit                    */

        // byte transfer
        dma0sz = len;
    }
    else {
        // Any other case than the one above supports word transfer
        dma0sz = len / 2;
    }

    /* reset control register */
    DMA_CHAN_0->CTL = 0;

    /* use DMAREQ as trigger*/
    DMA_MODULE->CTL0 |= DMA_CTL0_SEL_REQ;

    DMA_CHAN_0->SA = (size_t)src;
    DMA_CHAN_0->DA = (size_t)dst;
    DMA_CHAN_0->SZ = dma0sz;

    DMA_CHAN_0->CTL = DMA_CC_DT_BLOCK   /* Block transfer mode     */
                      | DMA_CC_DST_INCR /* Increment dst address   */
                      | DMA_CC_SRC_INCR /* Increment src address   */
                      | DMA_CC_EN       /* Enable DMA              */
                      | DMA_CC_REQ      /* Request DMA (runs copy) */
                      | copymode;       /* Byte or word transfer   */

    /* Copy the last byte, if necessary */
    /* copymode == 0 <=> word transfer  */
    if ((copymode == 0) && (len & 0x1)) {
        *((char *)dst + len - 1) = *((char *)src + len - 1);
    }

    return initial_dst;
}
