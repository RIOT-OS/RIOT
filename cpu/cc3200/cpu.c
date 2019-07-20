/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200
 * @{
 *
 * @file
 * @brief           Implementation of the CPU initialization
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#include "vendor/hw_hib1p2.h"
#include "vendor/hw_hib3p3.h"
#include "vendor/hw_ints.h"
#include "vendor/hw_memmap.h"
#include "vendor/hw_ocp_shared.h"
#include "vendor/hw_types.h"

#include "cpu.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "vendor/hw_types.h"
#include "vendor/rom.h"

static void periph_reset(void);

/**
 * @brief Initialize CPU and reset device peripheral
 *
 */
void cpu_init(void)
{
    /* initializes the Cortex-M core */
    cortexm_init();

    /* reset board */
    periph_reset();

    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief init peripheral clock and perform a softreset
 *
 * @param reg pointer to register
 */
void reset_periph_clk(cc3200_arcm_reg_t *reg)
{
    volatile unsigned long ulDelay;

    /* enable & reset peripherals */
    reg->soft_reset |= PRCM_SOFT_RESET;

    /* wait for the hardware to perform reset */
    for (ulDelay = 0; ulDelay < 16; ulDelay++) {
    }

    /* deassert reset */
    reg->soft_reset &= ~PRCM_SOFT_RESET;
}

/**
 * @brief Get the sys reset cause object
 *
 * @return uint32_t restart reason code
 */
static uint32_t get_sys_reset_cause(void)
{
    uint32_t reset_cause;
    uint32_t hiber_status;
    /* reset read reset status */
    reset_cause = GPRCM->APPS_RESET_CAUSE & 0xFF;

    if (reset_cause == PRCM_POWER_ON) {
        hiber_status = HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_WAKE_STATUS);
        /* FIXME: wait for some reason test if this is needed*/
        ROM_UtilsDelay(PRCM_OP_DELAY);
        if (hiber_status & 0x1) {
            return PRCM_HIB_EXIT;
        }
    }
    return reset_cause;
}

/**
 * @brief periph_reset resets peripherals
 * based upon PRCMCC3200MCUInit from TIs prcm.c
 *
 */
static void periph_reset(void)
{
    cc3200_reg_t tmp;

    /* DIG DCDC LPDS ECO Enable */
    HWREG(HIB1P2_BASE + HIB1P2_O_ANA_DCDC_PARAMETERS16) |= 0x800000;

    /* enable hibernate ECO */
    tmp = HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_WAKE_STATUS);
    ROM_UtilsDelay(PRCM_OP_DELAY);
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_REG0) = tmp | (1 << 4);
    ROM_UtilsDelay(PRCM_OP_DELAY);

    /* enable clock switching */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_SPARE_REG_5) |= 0x3C;

    /* enable and reset default peripheral */
    reset_periph_clk(&ARCM->UDMA_A);

    /* disable udma */
    ARCM->UDMA_A.clk_gating &= ~PRCM_RUN_MODE_CLK;

    if (get_sys_reset_cause() == PRCM_POWER_ON) {
        /* enable RTC timer after hibernation */
        HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE) = 0x1;
        ROM_UtilsDelay(PRCM_OP_DELAY);
    }

    /* SWD mode */
    if (((HWREG(HIB1P2_BASE + HIB1P2_O_SOP_SENSE_VALUE) & 0xFF) == 0x2)) {
        HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_28) =
                ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_28) &
                  ~0xC0F) |
                 0x2);
        HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_29) =
                ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_29) &
                  ~0xC0F) |
                 0x2);
    }

    /* Override JTAG mux */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_CC3XX_DEV_PADCONF) |= 0x2;

    /* Change UART pins(55,57) mode to PIN_MODE_0 if they are in PIN_MODE_1
     */
    if ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_1) & 0xF) ==
        0x1) {
        HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_1) =
                ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_1) &
                  ~0xF));
    }

    if ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_2) & 0xF) ==
        0x1) {
        HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_2) =
                ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_2) &
                  ~0xF));
    }

    /* DIG DCDC VOUT trim settings based on PROCESS INDICATOR */
    if (((GPRCM->GPRCM_EFUSE_READ_REG0 >> 22) & 0xF) == 0xE) {
        HWREG(HIB1P2_BASE + HIB1P2_O_DIG_DCDC_VTRIM_CFG) =
                ((HWREG(HIB1P2_BASE + HIB1P2_O_DIG_DCDC_VTRIM_CFG) &
                  ~(0x00FC0000)) |
                 (0x32 << 18));
    } else {
        HWREG(HIB1P2_BASE + HIB1P2_O_DIG_DCDC_VTRIM_CFG) =
                ((HWREG(HIB1P2_BASE + HIB1P2_O_DIG_DCDC_VTRIM_CFG) &
                  ~(0x00FC0000)) |
                 (0x29 << 18));
    }

    /* Enable SOFT RESTART in case of DIG DCDC collapse */
    HWREG(HIB3P3_BASE + HIB3P3_O_HIBANA_SPARE_LOWV) &= ~(0x10000000);

    /* Disable the sleep for ANA DCDC */
    HWREG(HIB1P2_BASE + HIB1P2_O_ANA_DCDC_PARAMETERS_OVERRIDE) |= 0x00000004;
}