/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Anton Gerasimov <tossel@gmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* CC26xx_CC13xx specific interrupt vectors */
WEAK_DEFAULT void isr_edge(void);
WEAK_DEFAULT void isr_i2c(void);
WEAK_DEFAULT void isr_rfc_cpe1(void);
WEAK_DEFAULT void isr_pka(void);
WEAK_DEFAULT void isr_aon_rtc(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_aux0_aon(void);
WEAK_DEFAULT void isr_ssi0(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_rfc_cpe0(void);
WEAK_DEFAULT void isr_rfc_hw(void);
WEAK_DEFAULT void isr_rfc_cmd_ack(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_aux1_aon(void);
WEAK_DEFAULT void isr_watchdog(void);
WEAK_DEFAULT void isr_timer0_chan0(void);
WEAK_DEFAULT void isr_timer0_chan1(void);
WEAK_DEFAULT void isr_timer1_chan0(void);
WEAK_DEFAULT void isr_timer1_chan1(void);
WEAK_DEFAULT void isr_timer2_chan0(void);
WEAK_DEFAULT void isr_timer2_chan1(void);
WEAK_DEFAULT void isr_timer3_chan0(void);
WEAK_DEFAULT void isr_timer3_chan1(void);
WEAK_DEFAULT void isr_crypto_res(void);
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_dmaerr(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_se0(void);
WEAK_DEFAULT void isr_aux_ce(void);
WEAK_DEFAULT void isr_aon_prog(void);
WEAK_DEFAULT void isr_dyn_prog(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_trng(void);
#ifdef CPU_VARIANT_X2
WEAK_DEFAULT void isr_osc(void);
WEAK_DEFAULT void isr_aux_timer2(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_batmon(void);
#endif // CPU_VARIANT_X2

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[] = {
    isr_edge,              /* 16 AON edge detect                        */
    isr_i2c,               /* 17 I2C                                    */
    isr_rfc_cpe1,          /* 18 RF Command and Packet Engine 1         */
    isr_pka,               /* 19 PKA interrupt                          */
    isr_aon_rtc,           /* 20 AON RTC                                */
    isr_uart0,             /* 21 UART0 Rx and Tx                        */
    isr_aux0_aon,          /* 22 AUX event 0, through AON domain */
    isr_ssi0,              /* 23 SSI0 Rx and Tx                         */
    isr_ssi1,              /* 24 SSI1 Rx and Tx                         */
    isr_rfc_cpe0,          /* 25 RF Command and Packet Engine 0         */
    isr_rfc_hw,            /* 26 RF Core Hardware                       */
    isr_rfc_cmd_ack,       /* 27 RF Core Command Acknowledge            */
    isr_i2s,               /* 28 I2S                                    */
    isr_aux1_aon,          /* 29 AUX event 1, through AON domain */
    isr_watchdog,          /* 30 Watchdog timer                         */
    isr_timer0_chan0,      /* 31 Timer 0 subtimer A                     */
    isr_timer0_chan1,      /* 32 Timer 0 subtimer B                     */
    isr_timer1_chan0,      /* 33 Timer 1 subtimer A                     */
    isr_timer1_chan1,      /* 34 Timer 1 subtimer B                     */
    isr_timer2_chan0,      /* 35 Timer 2 subtimer A                     */
    isr_timer2_chan1,      /* 36 Timer 2 subtimer B                     */
    isr_timer3_chan0,      /* 37 Timer 3 subtimer A                     */
    isr_timer3_chan1,      /* 38 Timer 3 subtimer B                     */
    isr_crypto_res,        /* 39 Crypto Core Result available           */
    isr_dma,               /* 40 uDMA Software                          */
    isr_dmaerr,            /* 41 uDMA Error                             */
    isr_flash,             /* 42 Flash controller                       */
    isr_se0,               /* 43 Software Event 0                       */
    isr_aux_ce,            /* 44 AUX combined event, directly to MCU domain */
    isr_aon_prog,          /* 45 AON programmable 0                     */
    isr_dyn_prog,          /* 46 Dynamic Programmable interrupt (default source: PRCM) */
    isr_comp,              /* 47 AUX Comparator A                       */
    isr_adc,               /* 48 AUX ADC IRQ                            */
    isr_trng,              /* 49 TRNG event                             */
#ifdef CPU_VARIANT_X2
    isr_osc,               /* 50 Combined event from oscillator control */
    isr_aux_timer2,        /* 51 AUX Timer 2 event 0                    */
    isr_uart1,             /* 52 UART 1 RX and TX                       */
    isr_batmon,            /* 53 BATMON interrupt                       */
#endif // CPU_VARIANT_X2
};

#if IS_USED(CONFIG_CC26XX_CC13XX_UPDATE_CCFG)
/**
 * @brief   Set the bits of a CCFG bit field.
 *
 * Allows safe concatenation of values and protects with the mask
 * from setting unwanted bits.
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * uint32_t value = CCFG_SET_BITS(0xCA, 24, 0xFF000000)
                  & CCFG_SET_BITS(0xFE, 16, 0x00FF0000);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @param value        The bit value to set.
 * @param shift_amount The (to left) shift amount.
 * @param mask         The bit mask.
 */
#define CCFG_SET_BITS(value, shift_amount, mask) \
    (((uint32_t)(value)) << (shift_amount) | ~(mask))

extern cortexm_base_t cortex_vector_base;

__attribute__((section(".ccfg"), used))
ccfg_regs_t cc26xx_cc13xx_ccfg = {
    /* external LF clock configuration */
    .EXT_LF_CLK         = CCFG_SET_BITS(SET_EXT_LF_CLK_DIO,
                                        CCFG_EXT_LF_CLK_DIO_s,
                                        CCFG_EXT_LF_CLK_DIO_m)
                        & CCFG_SET_BITS(SET_EXT_LF_CLK_RTC_INCREMENT,
                                        CCFG_EXT_LF_CLK_RTC_INCREMENT_s,
                                        CCFG_EXT_LF_CLK_RTC_INCREMENT_m),
    /* misc */
    .MODE_CONF_1        =
#ifdef CPU_VARIANT_X2
                        /* only x2 CPUs allow TCXO settings */
                          CCFG_SET_BITS(SET_MODE_CONF_1_TCXO_TYPE,
                                        CCFG_MODE_CONF_1_TCXO_TYPE_s,
                                        CCFG_MODE_CONF_1_TCXO_TYPE_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_TCXO_MAX_START,
                                        CCFG_MODE_CONF_1_TCXO_MAX_START_s,
                                        CCFG_MODE_CONF_1_TCXO_MAX_START_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_ALT_DCDC_VMIN,
                                        CCFG_MODE_CONF_1_ALT_DCDC_VMIN_s,
                                        CCFG_MODE_CONF_1_ALT_DCDC_VMIN_m)
#else
                          CCFG_SET_BITS(SET_MODE_CONF_1_ALT_DCDC_VMIN,
                                        CCFG_MODE_CONF_1_ALT_DCDC_VMIN_s,
                                        CCFG_MODE_CONF_1_ALT_DCDC_VMIN_m)
#endif /* CPU_VARIANT_X2 */
                        & CCFG_SET_BITS(SET_MODE_CONF_1_ALT_DCDC_DITHER_EN,
                                        CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_s,
                                        CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_ALT_DCDC_IPEAK,
                                        CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_s,
                                        CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_DELTA_IBIAS_INIT,
                                        CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_s,
                                        CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_DELTA_IBIAS_OFFSET,
                                        CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_s,
                                        CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_1_XOSC_MAX_START,
                                        CCFG_MODE_CONF_1_XOSC_MAX_START_s,
                                        CCFG_MODE_CONF_1_XOSC_MAX_START_m),
    /* misc */
    .SIZE_AND_DIS_FLAGS = CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG,
                                        CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_m)
                        & CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS,
                                        CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_m)
                        & CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_DIS_TCXO,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_m)
                        & CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_DIS_GPRAM,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_m)
                        & CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_m)
                        & CCFG_SET_BITS(SET_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_s,
                                        CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_m),
    /* misc */
    .MODE_CONF          = CCFG_SET_BITS(SET_MODE_CONF_VDDR_TRIM_SLEEP_DELTA,
                                        CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_s,
                                        CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_DCDC_RECHARGE,
                                        CCFG_MODE_CONF_DCDC_RECHARGE_s,
                                        CCFG_MODE_CONF_DCDC_RECHARGE_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_DCDC_ACTIVE,
                                        CCFG_MODE_CONF_DCDC_ACTIVE_s,
                                        CCFG_MODE_CONF_DCDC_ACTIVE_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_VDDR_EXT_LOAD,
                                        CCFG_MODE_CONF_VDDR_EXT_LOAD_s,
                                        CCFG_MODE_CONF_VDDR_EXT_LOAD_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_VDDS_BOD_LEVEL,
                                        CCFG_MODE_CONF_VDDS_BOD_LEVEL_s,
                                        CCFG_MODE_CONF_VDDS_BOD_LEVEL_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_SCLK_LF_OPTION,
                                        CCFG_MODE_CONF_SCLK_LF_OPTION_s,
                                        CCFG_MODE_CONF_SCLK_LF_OPTION_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_VDDR_TRIM_SLEEP_TC,
                                        CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_s,
                                        CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_RTC_COMP,
                                        CCFG_MODE_CONF_RTC_COMP_s,
                                        CCFG_MODE_CONF_RTC_COMP_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_XOSC_FREQ,
                                        CCFG_MODE_CONF_XOSC_FREQ_s,
                                        CCFG_MODE_CONF_XOSC_FREQ_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_XOSC_CAP_MOD,
                                        CCFG_MODE_CONF_XOSC_CAP_MOD_s,
                                        CCFG_MODE_CONF_XOSC_CAP_MOD_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_HF_COMP,
                                        CCFG_MODE_CONF_HF_COMP_s,
                                        CCFG_MODE_CONF_HF_COMP_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_XOSC_CAPARRAY_DELTA,
                                        CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_s,
                                        CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_m)
                        & CCFG_SET_BITS(SET_MODE_CONF_VDDR_CAP,
                                        CCFG_MODE_CONF_VDDR_CAP_s,
                                        CCFG_MODE_CONF_VDDR_CAP_m),
    /* reserved for "future use" by Texas Instruments */
    .VOLT_LOAD_0        = 0xFFFFFFFF,
    .VOLT_LOAD_1        = 0xFFFFFFFF,
    .RTC_OFFSET         = 0xFFFFFFFF,
    .FREQ_OFFSET        = 0xFFFFFFFF,
    /* IEEE 802.15.4g MAC address */
    .IEEE_MAC_0         = 0xFFFFFFFF,
    .IEEE_MAC_1         = 0xFFFFFFFF,
    /* BLE MAC address */
    .IEEE_BLE_0         = 0xFFFFFFFF,
    .IEEE_BLE_1         = 0xFFFFFFFF,
    /* bootloader */
    .BL_CONFIG          = CCFG_SET_BITS(SET_BL_CONFIG_BOOTLOADER_ENABLE,
                                        CCFG_BL_CONFIG_BOOTLOADER_ENABLE_s,
                                        CCFG_BL_CONFIG_BOOTLOADER_ENABLE_m)
                        & CCFG_SET_BITS(SET_BL_CONFIG_BL_LEVEL,
                                        CCFG_BL_CONFIG_BL_LEVEL_s,
                                        CCFG_BL_CONFIG_BL_LEVEL_m)
                        & CCFG_SET_BITS(SET_BL_CONFIG_BL_PIN_NUMBER,
                                        CCFG_BL_CONFIG_BL_PIN_NUMBER_s,
                                        CCFG_BL_CONFIG_BL_PIN_NUMBER_m)
                        & CCFG_SET_BITS(SET_BL_CONFIG_BL_ENABLE,
                                        CCFG_BL_CONFIG_BL_ENABLE_s,
                                        CCFG_BL_CONFIG_BL_ENABLE_m),
    .ERASE_CONF         = 0xFFFFFFFF,
    /* debugging */
    .CCFG_TI_OPTIONS    = 0xFFFFFF00,
    .CCFG_TAP_DAP_0     = CCFG_SET_BITS(SET_CCFG_TAP_DAP_0_CPU_DAP_ENABLE,
                                        CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_s,
                                        CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_m)
                        & CCFG_SET_BITS(SET_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE,
                                        CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_s,
                                        CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_m)
                        & CCFG_SET_BITS(SET_CCFG_TAP_DAP_0_TEST_TAP_ENABLE,
                                        CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_s,
                                        CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_m),
    /* PBIST2, PBIST1 and AON disabled by default */
    .CCFG_TAP_DAP_1     = 0xFF000000,
    /* flash image vector table */
    .IMAGE_VALID_CONF   = (reg32_t)&cortex_vector_base,
    /* flash sector write protections */
    .CCFG_PROT_31_0     = 0xFFFFFFFF,
    .CCFG_PROT_63_32    = 0xFFFFFFFF,
    .CCFG_PROT_95_64    = 0xFFFFFFFF,
    .CCFG_PROT_127_96   = 0xFFFFFFFF,
};
#endif /* IS_USED(CONFIG_CC26XX_CC13XX_UPDATE_CCFG) */

/** @} */
