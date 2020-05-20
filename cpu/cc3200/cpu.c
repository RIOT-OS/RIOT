/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup        cpu_cc3200
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
#include "stdio_uart.h"
#include "vendor/hw_types.h"
#include "vendor/rom.h"

/**
 * @brief configuration for the Digital DC/DC Voltage Trimmer
 *
 */
#define DIGI_DCDC_VTRIM_CFG \
    (*((volatile uint32_t *)(HIB1P2_BASE + HIB1P2_O_DIG_DCDC_VTRIM_CFG)))

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

    stdio_init();

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
    volatile uint32_t ulDelay;

    /* enable & reset peripherals */
    reg->soft_reset |= PRCM_SOFT_RESET;

    /* wait for the hardware to perform reset */
    for (ulDelay = 0; ulDelay < 16; ulDelay++) {}

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
    /* read reset status */
    uint32_t reset_cause = GPRCM->APPS_RESET_CAUSE & 0xFF;

    if (reset_cause == PRCM_POWER_ON) {
        uint32_t hiber_status = HIBERNATION_WAKE_STATUS_REG;
        /* reading hibernation status requires a small delay */
        USEC_DELAY(PRCM_OP_USEC_DELAY);
        if (hiber_status & 0x1) {
            return PRCM_HIB_EXIT;
        }
    }
    return reset_cause;
}

/**
 * @brief periph_reset performs essential MCU configuration.
 * This implementation is a cleaned up version of TIs PRCMCC3200MCUInit
 * (driverlib/prcm.c) Unfortunately most registers are not documented.
 * - Setup hardware to allow for Hibernation (ECO) and Low Power Deep Sleep
 *   (LPDS) modes
 * - Enable Clock Switching (presumably for the ECO and LPDS modes)
 * - Reset uDMA & disable it
 * - enable Real Time Clock if device was awaken from Hibernation (presumably
 * hibernation awake will trigger an restart interrupt, needs to be tested or
 * dropped)
 * - configure JTAG and JTAG FTDI pin connections for Software Development Mode
 * (SWD)
 * - Enable and configure various DC/DC converters
 */
static void periph_reset(void)
{
    /* This register activates the following MCU features:
        - Low Power Deep Sleep (LPDS)
        - Hibernation Mode (referred by TI as ECO)
        - Analogue DC/DC power converted (probably required by low power modes)
     */
    HWREG(HIB1P2_BASE + HIB1P2_O_ANA_DCDC_PARAMETERS16) |= 0x800000;
    USEC_DELAY(PRCM_OP_USEC_DELAY);

    /* enable hibernation power save mode on MCU (ECO) */
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_REG0) =
        HIBERNATION_WAKE_STATUS_REG | (1 << 4);
    USEC_DELAY(PRCM_OP_USEC_DELAY);

    /* enable clock switching */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_SPARE_REG_5) |= 0x3C;

    /* enable and reset default peripheral */
    reset_periph_clk(&ARCM->UDMA_A);

    /* disable udma */
    ARCM->UDMA_A.clk_gating &= ~PRCM_RUN_MODE_CLK;

    if (get_sys_reset_cause() == PRCM_POWER_ON) {
        /* enable RTC timer after hibernation */
        HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE) = 0x1;
        USEC_DELAY(PRCM_OP_USEC_DELAY);
    }

    /* configure software development mode pins (SWD) */
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

    /* DIG DCDC Voltage Out trim settings based on PROCESS INDICATOR (bits 22-25
     * of GPRCM_EFUSE_READ_REG0) of FUSE ROM
     */
    if (((GPRCM->GPRCM_EFUSE_READ_REG0 >> 22) & 0xF) == 0xE) {
        DIGI_DCDC_VTRIM_CFG =
            (DIGI_DCDC_VTRIM_CFG &
             ~HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_run_vtrim_M) |
            (0x32 << 18);
    }
    else {
        DIGI_DCDC_VTRIM_CFG =
            (DIGI_DCDC_VTRIM_CFG &
             ~HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_run_vtrim_M) |
            (0x29 << 18);
    }

    /* Enable SOFT RESTART in case of DIG DCDC converter collapse */
    HWREG(HIB3P3_BASE + HIB3P3_O_HIBANA_SPARE_LOWV) &= ~(0x10000000);

    /* Disable the sleep for ANA DCDC */
    HWREG(HIB1P2_BASE + HIB1P2_O_ANA_DCDC_PARAMETERS_OVERRIDE) |=
        HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_slp_mode_lowv_fsm_override_ctrl;
}
