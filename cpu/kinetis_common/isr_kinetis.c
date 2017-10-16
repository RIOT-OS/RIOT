/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
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

void pre_startup(void)
{
    /* disable the WDOG */
    wdog_disable();
#ifdef SIM_SCGC7_FLEXBUS_SHIFT
    /*
     * Workaround for hardware errata e4218: "SIM/FLEXBUS: SIM_SCGC7[FLEXBUS]
     * bit should be cleared when the FlexBus is not being used."
     */
    BITBAND_REG32(SIM->SCGC7, SIM_SCGC7_FLEXBUS_SHIFT) = 0;
#endif
}

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Cortex-M specific interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* Kinetis specific interrupt vector */
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_can0_bus_off(void);
WEAK_DEFAULT void isr_can0_error(void);
WEAK_DEFAULT void isr_can0_mb(void);
WEAK_DEFAULT void isr_can0_rx_warn(void);
WEAK_DEFAULT void isr_can0_tx_warn(void);
WEAK_DEFAULT void isr_can0_wake_up(void);
WEAK_DEFAULT void isr_can1_bus_off(void);
WEAK_DEFAULT void isr_can1_error(void);
WEAK_DEFAULT void isr_can1_mb(void);
WEAK_DEFAULT void isr_can1_rx_warn(void);
WEAK_DEFAULT void isr_can1_tx_warn(void);
WEAK_DEFAULT void isr_can1_wake_up(void);
WEAK_DEFAULT void isr_cmp0(void);
WEAK_DEFAULT void isr_cmp1(void);
WEAK_DEFAULT void isr_cmp2(void);
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
WEAK_DEFAULT void isr_dma_error(void);
WEAK_DEFAULT void isr_enet_1588_timer(void);
WEAK_DEFAULT void isr_enet_error(void);
WEAK_DEFAULT void isr_enet_rx(void);
WEAK_DEFAULT void isr_enet_tx(void);
WEAK_DEFAULT void isr_ftfl(void);
WEAK_DEFAULT void isr_ftfl_collision(void);
WEAK_DEFAULT void isr_ftm0(void);
WEAK_DEFAULT void isr_ftm1(void);
WEAK_DEFAULT void isr_ftm2(void);
WEAK_DEFAULT void isr_ftm3(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2s0_rx(void);
WEAK_DEFAULT void isr_i2s0_tx(void);
WEAK_DEFAULT void isr_llwu(void);
WEAK_DEFAULT void isr_lptmr0(void);
WEAK_DEFAULT void isr_lpuart0(void);
WEAK_DEFAULT void isr_mcg(void);
WEAK_DEFAULT void isr_mcm(void);
WEAK_DEFAULT void isr_pdb0(void);
WEAK_DEFAULT void isr_pit0(void);
WEAK_DEFAULT void isr_pit1(void);
WEAK_DEFAULT void isr_pit2(void);
WEAK_DEFAULT void isr_pit3(void);
WEAK_DEFAULT void isr_pmc(void);
WEAK_DEFAULT void isr_porta(void);
WEAK_DEFAULT void isr_portb(void);
WEAK_DEFAULT void isr_portc(void);
WEAK_DEFAULT void isr_portd(void);
WEAK_DEFAULT void isr_porte(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_seconds(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_sdhc(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_swi(void);
WEAK_DEFAULT void isr_tsi(void);
WEAK_DEFAULT void isr_uart0_error(void);
WEAK_DEFAULT void isr_uart0_lon(void);
WEAK_DEFAULT void isr_uart0_rx_tx(void);
WEAK_DEFAULT void isr_uart1_error(void);
WEAK_DEFAULT void isr_uart1_rx_tx(void);
WEAK_DEFAULT void isr_uart2_error(void);
WEAK_DEFAULT void isr_uart2_rx_tx(void);
WEAK_DEFAULT void isr_uart3_error(void);
WEAK_DEFAULT void isr_uart3_rx_tx(void);
WEAK_DEFAULT void isr_uart4_error(void);
WEAK_DEFAULT void isr_uart4_rx_tx(void);
WEAK_DEFAULT void isr_uart5_error(void);
WEAK_DEFAULT void isr_uart5_rx_tx(void);
WEAK_DEFAULT void isr_usb0(void);
WEAK_DEFAULT void isr_usbdcd(void);
WEAK_DEFAULT void isr_wdog_ewm(void);

/* Empty interrupt vector padding to ensure that all sanity checks in the
 * linking stage are fulfilled. These will be placed in the area between the
 * used vector table starting at memory address 0 and the flash configuration
 * field at 0x400-0x410 */
/* By using this padding we can let the linker script checks remain in place and
 * we will get a linking error if we accidentally link two interrupt vector
 * tables, or link the table from a different CPU, and catch many other mistakes. */
/* We subtract the expected number of used vectors, which are: The initial stack
 * pointer + the Cortex-M common IRQs + the Kinetis CPU specific IRQs */
ISR_VECTOR(99) const isr_t vector_padding[(0x400 / sizeof(isr_t)) - 1 - CPU_NONISR_EXCEPTIONS - CPU_IRQ_NUMOF];
