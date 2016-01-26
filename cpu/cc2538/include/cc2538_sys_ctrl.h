/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc2538_sysctrl CC2538 System Control
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           CC2538 System Control interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef CC2538_SYS_CTRL_H
#define CC2538_SYS_CTRL_H

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System Control component registers
 */
typedef struct {

    /**
     * @brief Clock control register
     */
    union {
        cc2538_reg_t CLOCK_CTRL;             /**< Clock control register */
        struct {
            cc2538_reg_t SYS_DIV       : 3;  /**< System clock rate setting */
            cc2538_reg_t RESERVED1     : 5;  /**< Reserved bits */
            cc2538_reg_t IO_DIV        : 3;  /**< I/O clock rate setting */
            cc2538_reg_t RESERVED2     : 5;  /**< Reserved bits */
            cc2538_reg_t OSC           : 1;  /**< System clock oscillator selection */
            cc2538_reg_t OSC_PD        : 1;  /**< Oscillator power-down */
            cc2538_reg_t RESERVED3     : 3;  /**< Reserved bits */
            cc2538_reg_t AMP_DET       : 1;  /**< Amplitude detector of XOSC during power up */
            cc2538_reg_t RESERVED4     : 2;  /**< Reserved bits */
            cc2538_reg_t OSC32K        : 1;  /**< 32-kHz clock oscillator selection */
            cc2538_reg_t OSC32K_CADIS  : 1;  /**< Disable calibration 32-kHz RC oscillator */
            cc2538_reg_t RESERVED5     : 6;  /**< Reserved bits */
        } CLOCK_CTRLbits;
    } cc2538_sys_ctrl_clk_ctrl;

    /**
     * @brief Clock status register
     */
    union {
        cc2538_reg_t CLOCK_STA;              /**< Clock status register */
        struct {
            cc2538_reg_t SYS_DIV       : 3;  /**< Current functional frequency for system clock */
            cc2538_reg_t RESERVED6     : 5;  /**< Reserved bits */
            cc2538_reg_t IO_DIV        : 3;  /**< Current functional frequency for IO_CLK */
            cc2538_reg_t RESERVED7     : 5;  /**< Reserved bits */
            cc2538_reg_t OSC           : 1;  /**< Current clock source selected */
            cc2538_reg_t OSC_PD        : 1;  /**< Oscillator power-down */
            cc2538_reg_t HSOSC_STB     : 1;  /**< HSOSC stable status */
            cc2538_reg_t XOSC_STB      : 1;  /**< XOSC stable status */
            cc2538_reg_t SOURCE_CHANGE : 1;  /**< System clock source change */
            cc2538_reg_t RESERVED8     : 1;  /**< Reserved bits */
            cc2538_reg_t RST           : 2;  /**< Last source of reset */
            cc2538_reg_t OSC32K        : 1;  /**< Current 32-kHz clock oscillator selected */
            cc2538_reg_t OSC32K_CALDIS : 1;  /**< Disable calibration 32-kHz RC oscillator */
            cc2538_reg_t SYNC_32K      : 1;  /**< 32-kHz clock source synced to undivided system clock (16 or 32 MHz) */
            cc2538_reg_t RESERVED9     : 5;  /**< Reserved bits */
        } CLOCK_STAbits;
    } cc2538_sys_ctrl_clk_sta;

    cc2538_reg_t RCGCGPT;                    /**< Module clocks for GPT[3:0] when the CPU is in active (run) mode */
    cc2538_reg_t SCGCGPT;                    /**< Module clocks for GPT[3:0] when the CPU is in sleep mode */
    cc2538_reg_t DCGCGPT;                    /**< Module clocks for GPT[3:0] when the CPU is in PM0 */
    cc2538_reg_t SRGPT;                      /**< Reset for GPT[3:0]. */
    cc2538_reg_t RCGCSSI;                    /**< Module clocks for SSI[1:0] when the CPU is in active (run) mode */
    cc2538_reg_t SCGCSSI;                    /**< Module clocks for SSI[1:0] when the CPU is insSleep mode */
    cc2538_reg_t DCGCSSI;                    /**< Module clocks for SSI[1:0] when the CPU is in PM0 */
    cc2538_reg_t SRSSI;                      /**< Reset for SSI[1:0]. */

    /**
     * @brief UART module clock register - active mode
     */
    union {
        cc2538_reg_t RCGCUART;               /**< Module clocks for UART[1:0] when the CPU is in active (run) mode */
        struct {
            cc2538_reg_t UART0         :  1; /**< Enable UART0 clock in active (run) mode */
            cc2538_reg_t UART1         :  1; /**< Enable UART1 clock in active (run) mode */
            cc2538_reg_t RESERVED      : 30; /**< Reserved bits */
        } RCGCUARTbits;
    } cc2538_sys_ctrl_unnamed1;

    /**
     * @brief UART module clock register - sleep mode
     */
    union {
        cc2538_reg_t SCGCUART;               /**< Module clocks for UART[1:0] when the CPU is in sleep mode */
        struct {
            cc2538_reg_t UART0         :  1; /**< Enable UART0 clock in sleep mode */
            cc2538_reg_t UART1         :  1; /**< Enable UART1 clock in sleep mode */
            cc2538_reg_t RESERVED      : 30; /**< Reserved bits */
        } SCGCUARTbits;
    } cc2538_sys_ctrl_unnamed2;

    /**
     * @brief UART module clock register - PM0 mode
     */
    union {
        cc2538_reg_t DCGCUART;               /**< Module clocks for UART[1:0] when the CPU is in PM0 */
        struct {
            cc2538_reg_t UART0         :  1; /**< Enable UART0 clock in PM0 */
            cc2538_reg_t UART1         :  1; /**< Enable UART1 clock in PM0 */
            cc2538_reg_t RESERVED      : 30; /**< Reserved bits */
        } DCGCUARTbits;
    } cc2538_sys_ctrl_unnamed3;

    cc2538_reg_t SRUART;                     /**< Reset for UART[1:0]. */
    cc2538_reg_t RCGCI2C;                    /**< Module clocks for I2C when the CPU is in active (run) mode */
    cc2538_reg_t SCGCI2C;                    /**< Module clocks for I2C when the CPU is in sleep mode */
    cc2538_reg_t DCGCI2C;                    /**< Module clocks for I2C when the CPU is in PM0 */
    cc2538_reg_t SRI2C;                      /**< Reset for I2C. */
    cc2538_reg_t RCGCSEC;                    /**< Module clocks for the security module when the CPU is in active (run) mode */
    cc2538_reg_t SCGCSEC;                    /**< Module clocks for the security module when the CPU is in sleep mode */
    cc2538_reg_t DCGCSEC;                    /**< Module clocks for the security module when the CPU is in PM0 */
    cc2538_reg_t SRSEC;                      /**< Reset for the security module. */
    cc2538_reg_t PMCTL;                      /**< Power mode. */
    cc2538_reg_t SRCRC;                      /**< CRC on state retention. */
    cc2538_reg_t RESERVED10[5];              /**< Reserved bits */
    cc2538_reg_t PWRDBG;                     /**< Power debug register */
    cc2538_reg_t RESERVED11[2];              /**< Reserved bits */
    cc2538_reg_t CLD;                        /**< This register controls the clock loss detection feature. */
    cc2538_reg_t RESERVED12[4];              /**< Reserved bits */
    cc2538_reg_t IWE;                        /**< This register controls interrupt wake-up. */
    cc2538_reg_t I_MAP;                      /**< This register selects which interrupt map to be used. */
    cc2538_reg_t RESERVED13[3];              /**< Reserved bits */
    cc2538_reg_t RCGCRFC;                    /**< This register defines the module clocks for RF CORE when the CPU is in active (run) mode */
    cc2538_reg_t SCGCRFC;                    /**< This register defines the module clocks for RF CORE when the CPU is in sleep mode */
    cc2538_reg_t DCGCRFC;                    /**< This register defines the module clocks for RF CORE when the CPU is in PM0 */
    cc2538_reg_t EMUOVR;                     /**< This register defines the emulator override controls for power mode and peripheral clock gate. */
} cc2538_sys_ctrl_t;

#define SYS_CTRL ( (cc2538_sys_ctrl_t*)0x400d2000 ) /**< One and only instance of the System Control module */

/**
 * @brief Compute the current system clock frequency based on the SYS_CTRL register states
 */
#define sys_clock_freq() ((SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC ? \
                           RCOSC16M_FREQ : XOSC32M_FREQ) >> \
                                SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.SYS_DIV )

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_SYS_CTRL_H */

/** @} */
