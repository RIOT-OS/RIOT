/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "cc26x2_cc13x2_rfc.h"
#include "cc26xx_cc13xx_power.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void cc26x2_cc13x2_rfc_request_on(void)
{
    /* We don't use the modes on PRCM->RFCMODEHWOPT since that is not
     * documented, writing a 0 means to select the mode automatically. On cc13x0
     * it's needed to write one of the valid modes specified at
     * PRCM->RFCMODEHWOPT. On cc26x0 it's fine to write 0 */
    PRCM->RFCMODESEL = 0;

    /* Enable RF Core power domain */
    if (!power_is_domain_enabled(POWER_DOMAIN_RFC)) {
        power_enable_domain(POWER_DOMAIN_RFC);
    }

    /* Enable RF Core clock */
    power_clock_enable_rfc();

    /* At this point RF Core registers are available, disable and remove
     * previous interrupts (if any) */
    RFC_DBELL_NONBUF->RFCPEIFG = 0;
    RFC_DBELL_NONBUF->RFCPEIEN = 0;

    /* Select all interrupts for RF_CPE0_IRQN, and CPE_IRQ_INTERNAL_ERROR */
    RFC_DBELL_NONBUF->RFCPEISL = 0;
    RFC_DBELL_NONBUF->RFCPEISL |= CPE_IRQ_INTERNAL_ERROR;

    NVIC_ClearPendingIRQ(RF_CPE0_IRQN);
    NVIC_ClearPendingIRQ(RF_CPE1_IRQN);
    NVIC_EnableIRQ(RF_CPE0_IRQN);
    NVIC_EnableIRQ(RF_CPE1_IRQN);

    /* Enable internal error interrupt (handled on isr_rfc_cpe1) */
    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_INTERNAL_ERROR;

    /* Let CPE boot */
    RFC_PWR_NONBUF->PWMCLKEN = PWMCLKEN_CPERAM | PWMCLKEN_CPE | PWMCLKEN_RFC;
}

int cc26x2_cc13x2_rfc_confirm_on(void)
{
    if (!(RFC_DBELL->RFCPEIFG & CPE_IRQ_MODULES_UNLOCKED) &&
        !(RFC_DBELL->RFCPEIFG & CPE_IRQ_BOOT_DONE)) {
        DEBUG("[cc26x2_cc13x2_rfc]: boot unfinished, RFCPEIFG=%08lx\n",
              RFC_DBELL->RFCPEIFG);
        return -EAGAIN;
    }

    DEBUG("[cc26x2_cc13x2_rfc]: boot done, RFCPEIFG=%08lx\n",
          RFC_DBELL->RFCPEIFG);

    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_MODULES_UNLOCKED;
    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_BOOT_DONE;

    return 0;
}

void cc26x2_cc13x2_rfc_finish_on(void (* cpe_patch_fn)(void))
{
    DEBUG("[cc26x2_cc13x2_rfc]: changing HF osc to XOSC\n");
    /* Perform the switch to XOSC_HF */
    osc_hf_source_switch(OSC_XOSC_HF);

    /* Patch command and packet engine (CPE) */
    if (cpe_patch_fn != NULL) {
        DEBUG("[cc26x2_cc13x2_rfc]: patching CPE\n");
        cpe_patch_fn();
    }

    DEBUG("[cc26x2_cc13x2_rfc]: turning AON_RTC clock line ON\n");
    /* Turn on the clock line to the radio core, this is necessary to use the
     * CMD_SYNC_START_RAT and the CMD_SYNC_STOP_RAT commands. */
    AON_RTC->CTL |= AON_RTC_CTL_RTC_UPD_EN;
}

/**
 * @brief   Send a command syncrhonously to the RF Core
 *
 * @param[in] cmd The command.
 */
uint32_t cc26x2_cc13x2_rfc_request_execute(uintptr_t cmd)
{
    assert((cmd & 3) == 0 || (cmd & 0x00000001));

    unsigned key = irq_disable();
    /* Wait until the doorbell becomes available */
    while (RFC_DBELL->CMDR != 0) {}
    RFC_DBELL->RFACKIFG = 0;

    /* Submit the command to the CM0 through the doorbell */
    RFC_DBELL->CMDR = cmd;

    /* Wait until the CM0 starts to parse the command */
    while (!RFC_DBELL->RFACKIFG) {}
    RFC_DBELL->RFACKIFG = 0;

    /* Return the content of status register */
    uint32_t cmdsta = RFC_DBELL->CMDSTA;
    irq_restore(key);

    return cmdsta;
}

int cc26x2_cc13x2_rfc_confirm_execute(void)
{
    if (!(RFC_DBELL->RFCPEIFG & CPE_IRQ_LAST_COMMAND_DONE)) {
        return -EAGAIN;
    }

    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;

    return 0;
}

void cc26x2_cc13x2_rfc_abort_cmd(void)
{
    uint32_t cmdsta;
    cmdsta = cc26x2_cc13x2_rfc_request_execute(RFC_CMDR_DIR_CMD(RFC_CMD_STOP));
    assert((cmdsta & 0xFF) == RFC_CMDSTA_DONE);
}

void isr_rfc_cpe0(void)
{
    extern void cc26x2_cc13x2_rfc_isr(void);
    cc26x2_cc13x2_rfc_isr();

    cortexm_isr_end();
}

void isr_rfc_cpe1(void)
{
    DEBUG_PUTS("[cc26x2_cc13x2_rfc]: internal error");
    RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_INTERNAL_ERROR;
    cortexm_isr_end();
}
