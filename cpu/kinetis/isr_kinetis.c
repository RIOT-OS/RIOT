/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis
 * @{
 *
 * @file
 * @brief       Default interrupt service routine definitions for Kinetis CPUs
 *
 * This file defines weak defaults for all available ISRs in Kinetis CPUs, these
 * weak defaults will act as fallback definitions if no driver defines a
 * specific handler for any interrupt.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "vectors_cortexm.h"
#include "vectors_kinetis.h"
#include "wdog.h"
#include "bit.h"

/**
 * @brief Enable workarounds for some known CPU errata
 */
static inline void cpu_errata_fixes(void)
{
#ifdef SIM_SCGC7_FLEXBUS_SHIFT
    /* K series errata
     * e4218: SIM/FLEXBUS: SIM_SCGC7[FLEXBUS] bit should be cleared when the
     * FlexBus is not being used.
     *
     * Description: The SIM_SCGC7[FLEXBUS] bit is set by default. This means
     * that the FlexBus will be enabled and come up in global chip select mode.
     * With some code sequence and register value combinations the core could
     * attempt to prefetch from the FlexBus even though it might not actually
     * use the value it prefetched. In the case where the FlexBus is
     * unconfigured, this can result in a hung bus cycle on the FlexBus.
     *
     * Workaround: If the FlexBus is not being used, disabled the clock to the
     * FlexBus during chip initialization by clearing the SIM_SCGC7[FLEXBUS] bit.
     * If the FlexBus will be used, then enable at least one chip select as
     * early in the chip initialization process as possible.
     */
    bit_clear32(&SIM->SCGC7, SIM_SCGC7_FLEXBUS_SHIFT);
#endif
#ifdef RSIM
    /* KW41Z errata
     * e10224: RSIM: XTAL_OUT_EN signal from the pin is enabled by default
     *
     * Description: The XTAL_OUT_EN signal from the default XTAL_OUT_EN pin,
     * PTB0, is enabled out of reset. This will result in the reference
     * oscillator being enabled when this pin is asserted high regardless of the
     * port control multiplexor setting.
     *
     * Workaround: To prevent the pin from enabling the XTAL out feature
     * unintentionally, set RSIM_RF_OSC_CTRL[RADIO_EXT_OSC_OVRD_EN]=1.
     */
    bit_set32(&RSIM->RF_OSC_CTRL, RSIM_RF_OSC_CTRL_RADIO_EXT_OSC_OVRD_EN_SHIFT);
#endif
}

void pre_startup(void)
{
    /* disable the WDOG */
    wdog_disable();
    cpu_errata_fixes();
}

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Kinetis specific interrupt service routines */
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_adc2(void);
WEAK_DEFAULT void isr_can0_bus_off(void);
WEAK_DEFAULT void isr_can0_error(void);
WEAK_DEFAULT void isr_can0_ored_message_buffer(void);
WEAK_DEFAULT void isr_can0_rx_warning(void);
WEAK_DEFAULT void isr_can0_tx_warning(void);
WEAK_DEFAULT void isr_can0_wake_up(void);
WEAK_DEFAULT void isr_can1_bus_off(void);
WEAK_DEFAULT void isr_can1_error(void);
WEAK_DEFAULT void isr_can1_ored_message_buffer(void);
WEAK_DEFAULT void isr_can1_rx_warning(void);
WEAK_DEFAULT void isr_can1_tx_warning(void);
WEAK_DEFAULT void isr_can1_wake_up(void);
WEAK_DEFAULT void isr_cmp0(void);
WEAK_DEFAULT void isr_cmp1(void);
WEAK_DEFAULT void isr_cmp2(void);
WEAK_DEFAULT void isr_cmp3(void);
WEAK_DEFAULT void isr_cmt(void);
WEAK_DEFAULT void isr_dac0(void);
WEAK_DEFAULT void isr_dac1(void);
WEAK_DEFAULT void isr_dma0(void);
WEAK_DEFAULT void isr_dma1(void);
WEAK_DEFAULT void isr_dma2(void);
WEAK_DEFAULT void isr_dma3(void);
WEAK_DEFAULT void isr_dma4(void);
WEAK_DEFAULT void isr_dma5(void);
WEAK_DEFAULT void isr_dma6(void);
WEAK_DEFAULT void isr_dma7(void);
WEAK_DEFAULT void isr_dma8(void);
WEAK_DEFAULT void isr_dma9(void);
WEAK_DEFAULT void isr_dma10(void);
WEAK_DEFAULT void isr_dma11(void);
WEAK_DEFAULT void isr_dma12(void);
WEAK_DEFAULT void isr_dma13(void);
WEAK_DEFAULT void isr_dma14(void);
WEAK_DEFAULT void isr_dma15(void);
WEAK_DEFAULT void isr_dma0_dma16(void);
WEAK_DEFAULT void isr_dma1_dma17(void);
WEAK_DEFAULT void isr_dma2_dma18(void);
WEAK_DEFAULT void isr_dma3_dma19(void);
WEAK_DEFAULT void isr_dma4_dma20(void);
WEAK_DEFAULT void isr_dma5_dma21(void);
WEAK_DEFAULT void isr_dma6_dma22(void);
WEAK_DEFAULT void isr_dma7_dma23(void);
WEAK_DEFAULT void isr_dma8_dma24(void);
WEAK_DEFAULT void isr_dma9_dma25(void);
WEAK_DEFAULT void isr_dma10_dma26(void);
WEAK_DEFAULT void isr_dma11_dma27(void);
WEAK_DEFAULT void isr_dma12_dma28(void);
WEAK_DEFAULT void isr_dma13_dma29(void);
WEAK_DEFAULT void isr_dma14_dma30(void);
WEAK_DEFAULT void isr_dma15_dma31(void);
WEAK_DEFAULT void isr_dma_error(void);
WEAK_DEFAULT void isr_enet_1588_timer(void);
WEAK_DEFAULT void isr_enet_error(void);
WEAK_DEFAULT void isr_enet_receive(void);
WEAK_DEFAULT void isr_enet_transmit(void);
WEAK_DEFAULT void isr_ftfa(void);
WEAK_DEFAULT void isr_ftfa_collision(void);
WEAK_DEFAULT void isr_ftfe(void);
WEAK_DEFAULT void isr_ftfe_collision(void);
WEAK_DEFAULT void isr_ftfl(void);
WEAK_DEFAULT void isr_ftfl_collision(void);
WEAK_DEFAULT void isr_ftm0(void);
WEAK_DEFAULT void isr_ftm1(void);
WEAK_DEFAULT void isr_ftm2(void);
WEAK_DEFAULT void isr_ftm3(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2c3(void);
WEAK_DEFAULT void isr_i2s0(void);
WEAK_DEFAULT void isr_i2s0_rx(void);
WEAK_DEFAULT void isr_i2s0_tx(void);
WEAK_DEFAULT void isr_llwu(void);
WEAK_DEFAULT void isr_lptmr0(void);
WEAK_DEFAULT void isr_lpuart0(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_lpuart2(void);
WEAK_DEFAULT void isr_lpuart3(void);
WEAK_DEFAULT void isr_lpuart4(void);
WEAK_DEFAULT void isr_lpuart5(void);
WEAK_DEFAULT void isr_lvd_lvw(void);
WEAK_DEFAULT void isr_mcg(void);
WEAK_DEFAULT void isr_mcm(void);
WEAK_DEFAULT void isr_pdb0(void);
WEAK_DEFAULT void isr_pit(void);
WEAK_DEFAULT void isr_pit0(void);
WEAK_DEFAULT void isr_pit1(void);
WEAK_DEFAULT void isr_pit2(void);
WEAK_DEFAULT void isr_pit3(void);
WEAK_DEFAULT void isr_porta(void);
WEAK_DEFAULT void isr_portb(void);
WEAK_DEFAULT void isr_portc(void);
WEAK_DEFAULT void isr_portd(void);
WEAK_DEFAULT void isr_porte(void);
WEAK_DEFAULT void isr_portb_portc(void);
WEAK_DEFAULT void isr_portc_portd(void);
WEAK_DEFAULT void isr_radio_0(void);
WEAK_DEFAULT void isr_radio_1(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_rtc_seconds(void);
WEAK_DEFAULT void isr_sdhc(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_swi(void);
WEAK_DEFAULT void isr_tpm0(void);
WEAK_DEFAULT void isr_tpm1(void);
WEAK_DEFAULT void isr_tpm2(void);
WEAK_DEFAULT void isr_tsi0(void);
WEAK_DEFAULT void isr_trng0(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart0_err(void);
WEAK_DEFAULT void isr_uart0_lon(void);
WEAK_DEFAULT void isr_uart0_rx_tx(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_uart1_err(void);
WEAK_DEFAULT void isr_uart1_rx_tx(void);
WEAK_DEFAULT void isr_uart2(void);
WEAK_DEFAULT void isr_uart2_err(void);
WEAK_DEFAULT void isr_uart2_flexio(void);
WEAK_DEFAULT void isr_uart2_rx_tx(void);
WEAK_DEFAULT void isr_uart3(void);
WEAK_DEFAULT void isr_uart3_err(void);
WEAK_DEFAULT void isr_uart3_rx_tx(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart4_err(void);
WEAK_DEFAULT void isr_uart4_rx_tx(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_uart5_err(void);
WEAK_DEFAULT void isr_uart5_rx_tx(void);
WEAK_DEFAULT void isr_usb0(void);
WEAK_DEFAULT void isr_usbdcd(void);
WEAK_DEFAULT void isr_usbhs(void);
WEAK_DEFAULT void isr_usbhsdcd(void);
WEAK_DEFAULT void isr_wdog_ewm(void);
WEAK_DEFAULT void isr_mscan_rx(void);
WEAK_DEFAULT void isr_mscan_tx(void);

/* Empty interrupt vector padding to ensure that all sanity checks in the
 * linking stage are fulfilled. These will be placed in the area between the
 * used vector table starting at memory address 0 and the flash configuration
 * field at 0x400-0x410 */
/* By using this padding we can let the linker script checks remain in place and
 * we will get a linking error if we accidentally link two interrupt vector
 * tables, or link the table from a different CPU, and catch many other mistakes. */
/* We subtract the expected number of used vectors, which are: The initial stack
 * pointer + the Cortex-M common IRQs + the Kinetis CPU specific IRQs */
ISR_VECTOR(99) const isr_t vector_padding[(0x400 / sizeof(isr_t)) - 1 - CPU_NONISR_EXCEPTIONS - CPU_IRQ_NUMOF] = { 0 };
