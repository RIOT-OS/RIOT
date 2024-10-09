/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level DMA driver implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "periph_cpu.h"
#include "periph_conf.h"
#include "mutex.h"
#include "bitarithm.h"
#include "pm_layered.h"
#include "thread_flags.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_DMA_NUMOF
#define CONFIG_DMA_NUMOF DMAC_CH_NUM
#endif

/* In memory DMA transfer descriptors */
static DmacDescriptor DMA_DESCRIPTOR_ATTRS descriptors[CONFIG_DMA_NUMOF];
static DmacDescriptor DMA_DESCRIPTOR_ATTRS writeback[CONFIG_DMA_NUMOF];

/* Bitmap of dma channels available */
static uint32_t channels_free = (1LLU << CONFIG_DMA_NUMOF) - 1;

struct dma_ctx {
    mutex_t sync_lock;
};

struct dma_ctx dma_ctx[CONFIG_DMA_NUMOF];

static void _poweron(void)
{
#if defined(MCLK)
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
#else
    PM->AHBMASK.reg |= PM_AHBMASK_DMAC;
    PM->APBBMASK.reg |= PM_APBBMASK_DMAC;
#endif
}

void dma_init(void)
{
    _poweron();
    for (unsigned i = 0; i < CONFIG_DMA_NUMOF; i++) {
        mutex_init(&dma_ctx[i].sync_lock);
    }
    /* Enable all priorities with RR scheduling */
    DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 |
                     DMAC_CTRL_LVLEN1 |
                     DMAC_CTRL_LVLEN2 |
                     DMAC_CTRL_LVLEN3;
    DMAC->PRICTRL0.reg = DMAC_PRICTRL0_RRLVLEN0 |
                         DMAC_PRICTRL0_RRLVLEN1 |
                         DMAC_PRICTRL0_RRLVLEN2 |
                         DMAC_PRICTRL0_RRLVLEN3;

    DMAC->BASEADDR.reg = (uint32_t)descriptors;
    DMAC->WRBADDR.reg = (uint32_t)writeback;

#if defined(CPU_FAM_SAML11) || defined(CPU_FAM_SAML10)
    NVIC_EnableIRQ(DMAC_0_IRQn);
    NVIC_EnableIRQ(DMAC_1_IRQn);
    NVIC_EnableIRQ(DMAC_2_IRQn);
    NVIC_EnableIRQ(DMAC_3_IRQn);
#elif defined(CPU_COMMON_SAMD5X)
    NVIC_EnableIRQ(DMAC_0_IRQn);
    NVIC_EnableIRQ(DMAC_1_IRQn);
    NVIC_EnableIRQ(DMAC_2_IRQn);
    NVIC_EnableIRQ(DMAC_3_IRQn);
    NVIC_EnableIRQ(DMAC_4_IRQn);
#else
    NVIC_EnableIRQ(DMAC_IRQn);
#endif

    DMAC->CTRL.reg |= DMAC_CTRL_DMAENABLE;
}

dma_t dma_acquire_channel(void)
{
    dma_t channel = UINT8_MAX;
    unsigned state = irq_disable();

    if (channels_free) {
        channel = bitarithm_lsb(channels_free);
        /* Clear channel bit */
        channels_free &= ~(1 << channel);
        /* ensure the sync lock is locked */
        mutex_trylock(&dma_ctx[channel].sync_lock);
    }
    irq_restore(state);
    return channel;
}

void dma_release_channel(dma_t dma)
{
    unsigned state = irq_disable();
#ifdef REG_DMAC_CHID
    DMAC->CHID.reg = dma;
    /* Reset DMA channel */
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
#else
    DMAC->Channel[dma].CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
#endif
    channels_free |= 1 << dma;
    irq_restore(state);
}

static inline void _set_source(DmacDescriptor *descr, const void *src)
{
    descr->SRCADDR.reg = (uint32_t)src;
}

static inline void _set_destination(DmacDescriptor *descr, void *dst)
{
    descr->DSTADDR.reg = (uint32_t)dst;
}

static inline void _set_num(DmacDescriptor *descr, size_t num)
{
    descr->BTCNT.reg = num;
}

static inline void _set_next_descriptor(DmacDescriptor *descr, void *next)
{
    descr->DESCADDR.reg = (uint32_t)next;
}

void dma_setup(dma_t dma, unsigned trigger, uint8_t prio, bool irq)
{
#ifdef REG_DMAC_CHID
    /* Ensure that this set of register writes is atomic */
    unsigned state = irq_disable();
    DMAC->CHID.reg = dma;
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_TRIGACT_BEAT |
                        (trigger << DMAC_CHCTRLB_TRIGSRC_Pos) |
                        (prio << DMAC_CHCTRLB_LVL_Pos);
    /* Clear everything in case a previous user left it configured */
    DMAC->CHINTENCLR.reg = 0xFF;
    if (irq) {
        DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    }
    irq_restore(state);
#else
    DMAC->Channel[dma].CHCTRLA.reg = DMAC_CHCTRLA_TRIGACT_BURST |
                                     (trigger << DMAC_CHCTRLA_TRIGSRC_Pos);
    DMAC->Channel[dma].CHPRILVL.reg = prio;
    DMAC->Channel[dma].CHINTENCLR.reg = 0xFF;
    if (irq) {
        DMAC->Channel[dma].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    }
#endif
}

void dma_prepare(dma_t dma, uint8_t width, const void *src, void *dst,
                 size_t num, dma_incr_t incr)
{
    DEBUG("[DMA]: Prepare %u, num: %u\n", dma, (unsigned)num);
    DmacDescriptor *descr = &descriptors[dma];
    _set_num(descr, num);
    _set_source(descr, src);
    _set_destination(descr, dst);
    descr->DESCADDR.reg = (uint32_t)NULL;
    descr->BTCTRL.reg = width << DMAC_BTCTRL_BEATSIZE_Pos |
                        incr << DMAC_BTCTRL_SRCINC_Pos |
                        DMAC_BTCTRL_VALID;
}

void dma_prepare_src(dma_t dma, const void *src, size_t num, bool incr)
{
    DEBUG("[dma]: %u: prep src %p, %u, %u\n", dma, src, (unsigned)num, incr);
    DmacDescriptor *descr = &descriptors[dma];
    _set_num(descr, num);
    _set_source(descr, src);
    descr->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_SRCINC) |
                        (incr << DMAC_BTCTRL_SRCINC_Pos);
    _set_next_descriptor(descr, NULL);
}

void dma_prepare_dst(dma_t dma, void *dst, size_t num, bool incr)
{
    DEBUG("[dma]: %u: prep dst %p, %u, %u\n", dma, dst, (unsigned)num, incr);
    DmacDescriptor *descr = &descriptors[dma];
    _set_num(descr, num);
    _set_destination(descr, dst);
    descr->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_DSTINC) |
                        (incr << DMAC_BTCTRL_DSTINC_Pos);
    _set_next_descriptor(descr, NULL);
}

void _fmt_append(DmacDescriptor *descr, DmacDescriptor *next,
                 const void *src, void *dst, size_t num)
{
    /* Configure the full descriptor besides the BTCTRL data */
    _set_next_descriptor(descr, next);
    _set_next_descriptor(next, NULL);
    _set_source(next, src);
    _set_num(next, num);
    _set_destination(next, dst);
}

void dma_append(dma_t dma, DmacDescriptor *next, uint8_t width,
                const void *src, void *dst, size_t num, dma_incr_t incr)
{
    DmacDescriptor *descr = &descriptors[dma];

    next->BTCTRL.reg = width << DMAC_BTCTRL_BEATSIZE_Pos |
                       incr << DMAC_BTCTRL_SRCINC_Pos |
                       DMAC_BTCTRL_VALID;
    _fmt_append(descr, next, src, dst, num);
}

void dma_append_src(dma_t dma, DmacDescriptor *next, const void *src,
                    size_t num, bool incr)
{
    DmacDescriptor *descr = &descriptors[dma];

    /* Copy the original descriptor config and modify the increment */
    next->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_SRCINC) |
                       (incr << DMAC_BTCTRL_SRCINC_Pos);
    _fmt_append(descr, next, src, (void *)descr->DSTADDR.reg, num);
}

void dma_append_dst(dma_t dma, DmacDescriptor *next, void *dst, size_t num,
                    bool incr)
{
    DmacDescriptor *descr = &descriptors[dma];

    /* Copy the original descriptor config and modify the increment */
    next->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_DSTINC) |
                       (incr << DMAC_BTCTRL_DSTINC_Pos);
    _fmt_append(descr, next, (void *)descr->SRCADDR.reg, dst, num);
}

void dma_start(dma_t dma)
{
    DEBUG("[dma]: starting: %u\n", dma);

#ifdef REG_DMAC_CHID
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_ENABLE;
    irq_restore(state);
#else
    DMAC->Channel[dma].CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
#endif
}

void dma_wait(dma_t dma)
{
    DEBUG("[DMA]: mutex lock: %u\n", dma);
    mutex_lock(&dma_ctx[dma].sync_lock);
}

void dma_cancel(dma_t dma)
{
    DEBUG("[DMA]: Cancelling active transfer: %u\n", dma);
#ifdef REG_DMAC_CHID
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    /* Write zero to the enable bit */
    DMAC->CHCTRLA.reg = 0;
    /* Wait until the active beat is finished */
    while (DMAC->CHCTRLA.reg & DMAC_CHCTRLA_ENABLE) {}
    irq_restore(state);
#else
    DMAC->Channel[dma].CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    while (DMAC->Channel[dma].CHCTRLA.reg & DMAC_CHCTRLA_ENABLE) {}
#endif
}

void isr_dmac(void)
{
    /* Always holds the interrupt status for the highest priority channel with
     * pending interrupts */
    uint16_t status = DMAC->INTPEND.reg;
    dma_t dma = status & DMAC_INTPEND_ID_Msk;

    /* Clear the pending interrupt flags for this channel by writing the
     * channel ID together with the flags to clear */
    DMAC->INTPEND.reg = status;
    if (status & DMAC_INTPEND_TCMPL) {
        mutex_unlock(&dma_ctx[dma].sync_lock);
    }
    DEBUG("[DMA] IRQ: %u: %x\n", dma, status);
    cortexm_isr_end();
}

void isr_dmac0(void)
{
    isr_dmac();
}

void isr_dmac1(void)
{
    isr_dmac();
}

void isr_dmac2(void)
{
    isr_dmac();
}

void isr_dmac3(void)
{
    isr_dmac();
}

void isr_dmac4(void)
{
    isr_dmac();
}
