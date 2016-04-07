/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_dma
 *
 * @{
 *
 * @file
 * @brief       Low-level DMA driver implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#if DEVELHELP
#include <stdio.h>
#include <inttypes.h>
#endif

#include "cpu.h"
#include "periph_conf.h"
#include "dma.h"

#ifndef DMA_NUMOF
#error DMA_NUMOF is undefined!
#endif

int dma_init(const uint8_t channel, const dma_source_t source)
{
    if (channel >= DMA_NUMOF) {
        return -1;
    }

    DMA_CLOCK_GATE = 1;

    /* Set Halt on Error, enable minor loop linking, pause transfers during debug */
    DMA_DEV->CR = DMA_CR_HOE_MASK | DMA_CR_EMLM_MASK | DMA_CR_EDBG_MASK;

    DMAMUX_CLOCK_GATE = 1;

    /* Disable the channel completely */
    DMAMUX->CHCFG[channel] = 0;

    /* Enable DMA transfer complete IRQs */
    /* Refactor the line below if the DMA IRQs are not sequential in numbering... */
    NVIC_EnableIRQ(DMA0_IRQn + channel);

    /* Disable requests on the chosen DMA channel */
    dma_channel_disable(channel);

    DMAMUX->CHCFG[channel] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(source);

    /* Enable error interrupts for the channel in order to easier detect program errors */
    DMA_DEV->SEEI = DMA_SEEI_SEEI(channel);
    NVIC_EnableIRQ(DMA_Error_IRQn);

    return 0;
}

#if DEVELHELP
/** @brief Provide verbose error reporting when DEVELHELP is set */
void dma_error_verbose(void)
{
    uint32_t reg_cr  = DMA_DEV->CR;  /* Cached copy of control register */
    uint32_t reg_es  = DMA_DEV->ES;  /* Cached copy of error status register */
    uint16_t reg_err = DMA_DEV->ERR; /* Cached copy of error request register */
    uint16_t reg_int = DMA_DEV->INT; /* Cached copy of interrupt request register */
    uint16_t reg_hrs = DMA_DEV->HRS; /* Cached copy of hardware request register */
    uint8_t channel = (reg_es & DMA_ES_ERRCHN_MASK) >> DMA_ES_ERRCHN_SHIFT;

    /* Stop the channel from further activation */
    dma_channel_disable(channel);

    printf("DMA error on channel %" PRIu8 "!\n", channel);
    printf("DMA_CR:     0x%08" PRIx32 "\n", reg_cr);
    printf("DMA_ES:     0x%08" PRIx32 "\n", reg_es);
    printf("     flags: ");
    if (reg_es & DMA_ES_VLD_MASK) {
        printf("VLD ");
    }
    if (reg_es & DMA_ES_ECX_MASK) {
        printf("ECX ");
    }
    if (reg_es & DMA_ES_CPE_MASK) {
        printf("CPE ");
    }
    if (reg_es & DMA_ES_SAE_MASK) {
        printf("SAE ");
    }
    if (reg_es & DMA_ES_SOE_MASK) {
        printf("SOE ");
    }
    if (reg_es & DMA_ES_DAE_MASK) {
        printf("DAE ");
    }
    if (reg_es & DMA_ES_DOE_MASK) {
        printf("DOE ");
    }
    if (reg_es & DMA_ES_NCE_MASK) {
        printf("NCE ");
    }
    if (reg_es & DMA_ES_SGE_MASK) {
        printf("SGE ");
    }
    if (reg_es & DMA_ES_SBE_MASK) {
        printf("SBE ");
    }
    if (reg_es & DMA_ES_DBE_MASK) {
        printf("DBE ");
    }
    puts("");
    printf("DMA_ERR:        0x%04" PRIx16 "\n", reg_err);
    printf("DMA_INT:        0x%04" PRIx16 "\n", reg_int);
    printf("DMA_HRS:        0x%04" PRIx16 "\n", reg_hrs);
    printf("TCD%" PRIu8 ":\n", channel);
    printf("     SADDR: 0x%08" PRIx32 "\n",     DMA_DEV->TCD[channel].SADDR);
    printf("      SOFF:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].SOFF);
    printf("      ATTR:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].ATTR);
    printf("    NBYTES: 0x%08" PRIx32 "\n",     DMA_DEV->TCD[channel].NBYTES_MLNO);
    printf("     SLAST: 0x%08" PRIx32 "\n",     DMA_DEV->TCD[channel].SLAST);
    printf("     DADDR: 0x%08" PRIx32 "\n",     DMA_DEV->TCD[channel].DADDR);
    printf("      DOFF:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].DOFF);
    printf("     CITER:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].CITER_ELINKNO);
    printf(" DLAST_SGA: 0x%08" PRIx32 "\n",     DMA_DEV->TCD[channel].DLAST_SGA);
    printf("       CSR:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].CSR);
    printf("     BITER:     0x%04" PRIx16 "\n", DMA_DEV->TCD[channel].BITER_ELINKNO);
}

void isr_dma_error(void)
{
    dma_error_verbose();
    while(1) {}
}
#endif
