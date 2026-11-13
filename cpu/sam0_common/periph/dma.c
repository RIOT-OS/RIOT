/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <stdint.h>
#include <stdio.h>

#include "periph_cpu.h"
#include "periph_conf.h"
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
    dma_cb_t cb;
    void *ctx;
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

static inline void _set_next_descriptor(DmacDescriptor *descr, const DmacDescriptor *next)
{
    descr->DESCADDR.reg = (uint32_t)next;
}

static inline DmacDescriptor *_get_next_descriptor(const DmacDescriptor *descr)
{
    return (DmacDescriptor *)descr->DESCADDR.reg;
}

void dma_set_cb_arg(dma_t dma, void *ctx)
{
    dma_ctx[dma].ctx = ctx;
}

void dma_setup(dma_t dma, dma_trigact_t granularity, unsigned trigger, uint8_t prio,
               dma_cb_t cb, void *ctx)
{
#ifdef REG_DMAC_CHID
    /* Ensure that this set of register writes is atomic */
    unsigned state = irq_disable();
    DMAC->CHID.reg = dma;
    DMAC->CHCTRLB.reg = (granularity << DMAC_CHCTRLB_TRIGACT_Pos) |
                        (trigger << DMAC_CHCTRLB_TRIGSRC_Pos) |
                        (prio << DMAC_CHCTRLB_LVL_Pos);
    /* Clear everything in case a previous user left it configured */
    DMAC->CHINTENCLR.reg = 0xFF;
    if (cb) {
        DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    }
    irq_restore(state);
#else
    DMAC->Channel[dma].CHCTRLA.reg = (granularity << DMAC_CHCTRLA_TRIGACT_Pos) |
                                     (trigger << DMAC_CHCTRLA_TRIGSRC_Pos);
    DMAC->Channel[dma].CHPRILVL.reg = prio;
    DMAC->Channel[dma].CHINTENCLR.reg = 0xFF;
    if (cb) {
        DMAC->Channel[dma].CHINTENSET.reg = cb ? DMAC_CHINTENSET_TCMPL : 0;
    }
#endif

    dma_ctx[dma].cb = cb;
    dma_ctx[dma].ctx = ctx;
}

void dma_prepare(dma_t dma, uint8_t width, const void *src, void *dst,
                 size_t num, dma_incr_t incr, dma_blockact_t blockact)
{
    dma_prepare_descriptor(&descriptors[dma], width, src, dst, num, incr, blockact);
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

static void _fmt_append(DmacDescriptor *descr, DmacDescriptor *next,
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
    while (_get_next_descriptor(descr)) {
        descr = _get_next_descriptor(descr);
    }

    next->BTCTRL.reg = width << DMAC_BTCTRL_BEATSIZE_Pos |
                       incr << DMAC_BTCTRL_SRCINC_Pos |
                       DMAC_BTCTRL_VALID;
    _fmt_append(descr, next, src, dst, num);
}

void dma_append_src(dma_t dma, DmacDescriptor *next, const void *src,
                    size_t num, bool incr)
{
    DmacDescriptor *descr = &descriptors[dma];
    while (_get_next_descriptor(descr)) {
        descr = _get_next_descriptor(descr);
    }
    /* Copy the original descriptor config and modify the increment */
    next->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_SRCINC) |
                       (incr << DMAC_BTCTRL_SRCINC_Pos);
    _fmt_append(descr, next, src, (void *)descr->DSTADDR.reg, num);
}

void dma_append_dst(dma_t dma, DmacDescriptor *next, void *dst, size_t num,
                    bool incr)
{
    DmacDescriptor *descr = &descriptors[dma];
    while (_get_next_descriptor(descr)) {
        descr = _get_next_descriptor(descr);
    }
    /* Copy the original descriptor config and modify the increment */
    next->BTCTRL.reg = (descr->BTCTRL.reg & ~DMAC_BTCTRL_DSTINC) |
                       (incr << DMAC_BTCTRL_DSTINC_Pos);
    _fmt_append(descr, next, (void *)descr->SRCADDR.reg, dst, num);
}

void dma_enable_loop(dma_t dma)
{
    DmacDescriptor *first = &descriptors[dma];
    DmacDescriptor *last = first;
    while (_get_next_descriptor(last)) {
        last = _get_next_descriptor(last);
        if (last == first) {
            /* loop already exists */
            return;
        }
    }
    _set_next_descriptor(last, first);
}

void dma_disable_loop(dma_t dma)
{
    DmacDescriptor *first = &descriptors[dma];
    DmacDescriptor *last = first;
    while (_get_next_descriptor(last) != first) {
        last = _get_next_descriptor(last);
        if (last == NULL) {
            /* loop already disabled */
            return;
        }
    }
    _set_next_descriptor(last, NULL);
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

void dma_resume(dma_t dma)
{
    DEBUG("[dma]: resuming: %u\n", dma);
#ifdef REG_DMAC_CHID
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    DMAC->CHCTRLB.reg |= DMAC_CHCTRLB_CMD_RESUME;
    irq_restore(state);
#else
    DMAC->Channel[dma].CHCTRLB.reg |= DMAC_CHCTRLB_CMD_RESUME;
#endif
}

dma_channel_status_t dma_channel_status(dma_t dma)
{
    dma_channel_status_t ch_status;
#ifdef REG_DMAC_CHID
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    uint8_t status = DMAC->CHSTATUS.reg;
    irq_restore(state);
#else
    uint8_t status = DMAC->Channel[dma].CHSTATUS.reg;
#endif
#ifdef DMAC_CHSTATUS_PEND_Pos
    ch_status.pending = (status & DMAC_CHSTATUS_PEND) >> DMAC_CHSTATUS_PEND_Pos;
#endif
#ifdef DMAC_CHSTATUS_BUSY_Pos
    ch_status.busy = (status & DMAC_CHSTATUS_BUSY) >> DMAC_CHSTATUS_BUSY_Pos;
#endif
#ifdef DMAC_CHSTATUS_FERR_Pos
    ch_status.ferr = (status & DMAC_CHSTATUS_FERR) >> DMAC_CHSTATUS_FERR_Pos;
#endif
#ifdef DMAC_CHSTATUS_CRCERR_Pos
    ch_status.crcerr = (status & DMAC_CHSTATUS_CRCERR) >> DMAC_CHSTATUS_CRCERR_Pos;
#endif
    return ch_status;
}

void dma_prepare_descriptor(void *desc, uint8_t width, const void *src, void *dst,
                            size_t num, dma_incr_t incr, dma_blockact_t blockact)
{
    DEBUG("[DMA]: Prepare desc %p, num: %u\n", desc, (unsigned)num);
    DmacDescriptor *d = (DmacDescriptor *)desc;
    _set_num(d, num);
    _set_source(d, src);
    _set_destination(d, dst);
    d->DESCADDR.reg = (uint32_t)NULL;
    d->BTCTRL.reg = width << DMAC_BTCTRL_BEATSIZE_Pos |
                    incr << DMAC_BTCTRL_SRCINC_Pos |
                    blockact << DMAC_BTCTRL_BLOCKACT_Pos |
                    DMAC_BTCTRL_VALID;
}

const void *dma_descriptor(dma_t dma)
{
    return &descriptors[dma];
}

const void *dma_get_next_descriptor(const void *desc)
{
    return _get_next_descriptor(desc);
}

void dma_set_next_descriptor(void *desc, const void *next)
{
    _set_next_descriptor(desc, next);
}

void dma_append_descriptor(void *desc, const void *next)
{
    while (_get_next_descriptor(desc)) {
        desc = _get_next_descriptor(desc);
    }
    _set_next_descriptor(desc, next);
}

void dma_print_descriptor(const void *desc)
{
    DmacDescriptor *d = (DmacDescriptor *)desc;
    do {
        /* print descriptor information */
        printf("BTCTRL: 0x%08x\n", (unsigned)d->BTCTRL.reg);
        printf("\tVALID: %u\n", (unsigned)d->BTCTRL.bit.VALID);
        printf("\tEVOSEL: %u\n", (unsigned)d->BTCTRL.bit.EVOSEL);
        printf("\tBLOCKACT: %u\n", (unsigned)d->BTCTRL.bit.BLOCKACT);
        printf("\tBEATSIZE: %u\n", (unsigned)d->BTCTRL.bit.BEATSIZE);
        printf("\tSRCINC: %u\n", (unsigned)d->BTCTRL.bit.SRCINC);
        printf("\tDSTINC: %u\n", (unsigned)d->BTCTRL.bit.DSTINC);
        printf("\tSTEPSEL: %u\n", (unsigned)d->BTCTRL.bit.STEPSEL);
        printf("\tSTEPSIZE: %u\n", (unsigned)d->BTCTRL.bit.STEPSIZE);
        printf("BTCNT: 0x%08x\n", (unsigned)d->BTCNT.reg);
        printf("SRCADDR: 0x%08x\n", (unsigned )d->SRCADDR.reg);
        printf("DSTADDR: 0x%08x\n", (unsigned )d->DSTADDR.reg);
        printf("DESCADDR: 0x%08x\n", (unsigned )d->DESCADDR.reg);
    } while ((d = (DmacDescriptor *)d->DESCADDR.reg));
}

#if MODULE_PERIPH_DMA_EVENT
void dma_event_use(dma_t dma, event_channel_t ch)
{
    periph_event_attach(ch, EVENT_USER_DMAC_CH_0 + dma);
}

void dma_event_disuse(dma_t dma, event_channel_t ch)
{
    periph_event_detach(ch, EVENT_USER_DMAC_CH_0 + dma);
}

void dma_event_setup(dma_t dma, dma_evact_t evact)
{
#ifdef DMAC_CHCTRLB_EVACT
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    DMAC->CHCTRLB.reg
        = (DMAC->CHCTRLB.reg & ~DMAC_CHCTRLB_EVACT_Msk) | (evact << DMAC_CHCTRLB_EVACT_Pos);
    irq_restore(state);
#else
    DMAC->Channel[dma].CHEVCTRL.reg
        = (DMAC->Channel[dma].CHEVCTRL.reg & ~DMAC_CHEVCTRL_EVACT_Msk) | (evact << DMAC_CHEVCTRL_EVACT_Pos);
#endif
}

void dma_event_input_enable(dma_t dma)
{
#ifdef DMAC_CHCTRLB_EVIE
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    DMAC->CHCTRLB.reg |= DMAC_CHCTRLB_EVIE;
    irq_restore(state);
#else
    DMAC->Channel[dma].CHEVCTRL.reg |= DMAC_CHEVCTRL_EVIE;
#endif
}

void dma_event_input_disable(dma_t dma)
{
#ifdef DMAC_CHCTRLB_EVIE
    unsigned state = irq_disable();
    DMAC->CHID.reg = DMAC_CHID_ID(dma);
    DMAC->CHCTRLB.reg &= ~DMAC_CHCTRLB_EVIE;
    irq_restore(state);
#else
    DMAC->Channel[dma].CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVIE;
#endif
}
#endif /* MODULE_PERIPH_DMA_EVENT */

void isr_dmac(void)
{
    /* Always holds the interrupt status for the highest priority channel with
     * pending interrupts */
    uint16_t status = DMAC->INTPEND.reg;
    dma_t dma = status & DMAC_INTPEND_ID_Msk;

    /* Clear the pending interrupt flags for this channel by writing the
     * channel ID together with the flags to clear */
    DMAC->INTPEND.reg = status;
    if ((status & DMAC_INTPEND_TCMPL) && dma_ctx[dma].cb) {
        dma_ctx[dma].cb(dma_ctx[dma].ctx);
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
