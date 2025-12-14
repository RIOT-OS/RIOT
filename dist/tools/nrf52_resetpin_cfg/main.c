/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tools
 * @{
 *
 * @file
 * @brief       Tool for programming the reset pin on nRF52x-based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "buildinfo/cpu.h"
#include "cpu.h"
#include "periph/pm.h"

/* guard against bad usage: only allow building for nRF52x CPUs */
#ifndef CPU_NRF52
#error This tool is only usable for nRF52x-based platforms
#endif

#ifndef RESET_PIN
#error RESET_PIN not specified
#endif

#define PORT_BIT        (0x00000020)
#define PORT_POS        (5U)
#define PIN_MASK        (0x0000001f)

#define RESET_VAL       (0xffffffff)

#define REG_NUM         (sizeof(NRF_UICR_Type) / 4)

/* allocate a copy of the registers in RAM */
static NRF_UICR_Type _buf;

static void _print_pin(uint32_t p)
{
    printf("P%i.%2i", (int)((p & PORT_BIT) >> PORT_POS), (int)(p & PIN_MASK));
}

static void _copy(volatile uint32_t *dst, volatile uint32_t *src, unsigned num)
{
    for (unsigned i = 0; i < num; i++) {
        *dst++ = *src++;
    }
}

static void _save_uicr(void)
{
    memcpy(&_buf, (void *)NRF_UICR, sizeof(NRF_UICR_Type));
}

static void _restore_uicr(void)
{
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}

    /* we copy the values back selectively, skipping the PSELRESET fields */
    _copy(NRF_UICR->NRFFW, _buf.NRFFW, 15);
    _copy(NRF_UICR->NRFHW, _buf.NRFHW, 12);
    _copy(NRF_UICR->CUSTOMER, _buf.CUSTOMER, 32);
    NRF_UICR->APPROTECT = _buf.APPROTECT;
    NRF_UICR->NFCPINS = _buf.NFCPINS;
#ifdef CPU_MODEL_NRF52840XXAA
    NRF_UICR->REGOUT0 = _buf.REGOUT0;
#endif

    /* we can leave the NVMC in write enable mode when leaving... */
}

int main(void)
{
    uint32_t target = (uint32_t)RESET_PIN;

    if ((NRF_UICR->PSELRESET[0] == target) &&
        (NRF_UICR->PSELRESET[1] == target)) {
        puts("\nAll good!");
        _print_pin(target);
        puts(" programmed as reset pin\n");
        puts("Now press the reset button to confirm its function!");
    }
    else {
        puts("\nReset pin is not programmed properly");
        printf("Will now program it to ");
        _print_pin(target);
        puts("\n\nPress any key (meaning send any char) to continue");
        getchar();

        puts("Programming the pin now...");
        if ((NRF_UICR->PSELRESET[0] != RESET_VAL) ||
            (NRF_UICR->PSELRESET[1] != RESET_VAL)) {
            /* we can only erase all UICR registers at once, so we need to save
             * and restore there values for clearing the PSELRESET registers */
            puts("save uicr");
            _save_uicr();
            NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
            NRF_NVMC->ERASEUICR = 1;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
            puts("restore");
            _restore_uicr();
        }

        puts("promming new value to PSELRESET");
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        NRF_UICR->PSELRESET[0] = target;
        NRF_UICR->PSELRESET[1] = target;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;

        /* verify result */
        puts("The changes will only take effect after reboot.\n"
             "Doing a reboot now...\n");
        pm_reboot();
    }

    return 0;
}
