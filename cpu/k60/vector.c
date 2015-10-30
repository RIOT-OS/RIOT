/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for K60 MCU.
 *
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @note It is not necessary to modify this file to define custom interrupt
 * service routines. All symbols are defined weak, it is only necessary to
 * define a function with the same name in another file to override the default
 * interrupt handlers.
 */

/**
 * @name Interrupt vector definition
 * @{
 */

#include "cpu.h"
#include "vectors_cortexm.h"
#include "wdog.h"

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _estack;

void pre_startup(void)
{
    /* disable the WDOG */
    wdog_disable();

    /*
     * Workaround for hardware errata e4218: "SIM/FLEXBUS: SIM_SCGC7[FLEXBUS]
     * bit should be cleared when the FlexBus is not being used."
     */
    BITBAND_REG32(SIM->SCGC7, SIM_SCGC7_FLEXBUS_SHIFT) = 0;
}

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Cortex-M specific interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* K60 specific interrupt vector */
WEAK_DEFAULT void isr_dma0_complete(void);
WEAK_DEFAULT void isr_dma1_complete(void);
WEAK_DEFAULT void isr_dma2_complete(void);
WEAK_DEFAULT void isr_dma3_complete(void);
WEAK_DEFAULT void isr_dma4_complete(void);
WEAK_DEFAULT void isr_dma5_complete(void);
WEAK_DEFAULT void isr_dma6_complete(void);
WEAK_DEFAULT void isr_dma7_complete(void);
WEAK_DEFAULT void isr_dma8_complete(void);
WEAK_DEFAULT void isr_dma9_complete(void);
WEAK_DEFAULT void isr_dma10_complete(void);
WEAK_DEFAULT void isr_dma11_complete(void);
WEAK_DEFAULT void isr_dma12_complete(void);
WEAK_DEFAULT void isr_dma13_complete(void);
WEAK_DEFAULT void isr_dma14_complete(void);
WEAK_DEFAULT void isr_dma15_complete(void);
WEAK_DEFAULT void isr_dma_error(void);
WEAK_DEFAULT void isr_mcm(void);
WEAK_DEFAULT void isr_flash_command_complete(void);
WEAK_DEFAULT void isr_flash_read_collision(void);
WEAK_DEFAULT void isr_low_voltage(void);
WEAK_DEFAULT void isr_llwu(void);
WEAK_DEFAULT void isr_watchdog(void);
WEAK_DEFAULT void isr_random_number_generator(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_can0_ored_msg_buffer(void);
WEAK_DEFAULT void isr_can0_bus_off(void);
WEAK_DEFAULT void isr_can0_error(void);
WEAK_DEFAULT void isr_can0_tx_warn(void);
WEAK_DEFAULT void isr_can0_rx_warn(void);
WEAK_DEFAULT void isr_can0_wake_up(void);
WEAK_DEFAULT void isr_i2s0_tx(void);
WEAK_DEFAULT void isr_i2s0_rx(void);
WEAK_DEFAULT void isr_can1_ored_msg_buffer(void);
WEAK_DEFAULT void isr_can1_bus_off(void);
WEAK_DEFAULT void isr_can1_error(void);
WEAK_DEFAULT void isr_can1_tx_warn(void);
WEAK_DEFAULT void isr_can1_rx_warn(void);
WEAK_DEFAULT void isr_can1_wake_up(void);
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_uart0_lon(void);
WEAK_DEFAULT void isr_uart0_status(void);
WEAK_DEFAULT void isr_uart0_error(void);
WEAK_DEFAULT void isr_uart1_status(void);
WEAK_DEFAULT void isr_uart1_error(void);
WEAK_DEFAULT void isr_uart2_status(void);
WEAK_DEFAULT void isr_uart2_error(void);
WEAK_DEFAULT void isr_uart3_status(void);
WEAK_DEFAULT void isr_uart3_error(void);
WEAK_DEFAULT void isr_uart4_status(void);
WEAK_DEFAULT void isr_uart4_error(void);
/* void dummy_handler(void); */
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_cmp0(void);
WEAK_DEFAULT void isr_cmp1(void);
WEAK_DEFAULT void isr_cmp2(void);
WEAK_DEFAULT void isr_ftm0(void);
WEAK_DEFAULT void isr_ftm1(void);
WEAK_DEFAULT void isr_ftm2(void);
WEAK_DEFAULT void isr_cmt(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_seconds(void);
WEAK_DEFAULT void isr_pit0(void);
WEAK_DEFAULT void isr_pit1(void);
WEAK_DEFAULT void isr_pit2(void);
WEAK_DEFAULT void isr_pit3(void);
WEAK_DEFAULT void isr_pdb(void);
WEAK_DEFAULT void isr_usb_otg(void);
WEAK_DEFAULT void isr_usb_charger_detect(void);
WEAK_DEFAULT void isr_enet_1588_timer(void);
WEAK_DEFAULT void isr_enet_tx(void);
WEAK_DEFAULT void isr_enet_rx(void);
WEAK_DEFAULT void isr_enet_error_misc(void);
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_sdhc(void);
WEAK_DEFAULT void isr_dac0(void);
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_tsi(void);
WEAK_DEFAULT void isr_mcg(void);
WEAK_DEFAULT void isr_lptmr0(void);
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_porta(void);
WEAK_DEFAULT void isr_portb(void);
WEAK_DEFAULT void isr_portc(void);
WEAK_DEFAULT void isr_portd(void);
WEAK_DEFAULT void isr_porte(void);
/* void dummy_handler(void); */
/* void dummy_handler(void); */
WEAK_DEFAULT void isr_software(void);

/**
 * @brief Interrupt vector definition
 */
__attribute__((section(".vector_table")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void *)(&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* K60 specific peripheral handlers */
    (void*) isr_dma0_complete,
    (void*) isr_dma1_complete,
    (void*) isr_dma2_complete,
    (void*) isr_dma3_complete,
    (void*) isr_dma4_complete,
    (void*) isr_dma5_complete,
    (void*) isr_dma6_complete,
    (void*) isr_dma7_complete,
    (void*) isr_dma8_complete,
    (void*) isr_dma9_complete,
    (void*) isr_dma10_complete,
    (void*) isr_dma11_complete,
    (void*) isr_dma12_complete,
    (void*) isr_dma13_complete,
    (void*) isr_dma14_complete,
    (void*) isr_dma15_complete,
    (void*) isr_dma_error,
    (void*) isr_mcm,
    (void*) isr_flash_command_complete,
    (void*) isr_flash_read_collision,
    (void*) isr_low_voltage,
    (void*) isr_llwu,
    (void*) isr_watchdog,
    (void*) isr_random_number_generator,
    (void*) isr_i2c0,
    (void*) isr_i2c1,
    (void*) isr_spi0,
    (void*) isr_spi1,
    (void*) isr_spi2,
    (void*) isr_can0_ored_msg_buffer,
    (void*) isr_can0_bus_off,
    (void*) isr_can0_error,
    (void*) isr_can0_tx_warn,
    (void*) isr_can0_rx_warn,
    (void*) isr_can0_wake_up,
    (void*) isr_i2s0_tx,
    (void*) isr_i2s0_rx,
    (void*) isr_can1_ored_msg_buffer,
    (void*) isr_can1_bus_off,
    (void*) isr_can1_error,
    (void*) isr_can1_tx_warn,
    (void*) isr_can1_rx_warn,
    (void*) isr_can1_wake_up,
    (void*) dummy_handler,
    (void*) isr_uart0_lon,
    (void*) isr_uart0_status,
    (void*) isr_uart0_error,
    (void*) isr_uart1_status,
    (void*) isr_uart1_error,
    (void*) isr_uart2_status,
    (void*) isr_uart2_error,
    (void*) isr_uart3_status,
    (void*) isr_uart3_error,
    (void*) isr_uart4_status,
    (void*) isr_uart4_error,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) isr_adc0,
    (void*) isr_adc1,
    (void*) isr_cmp0,
    (void*) isr_cmp1,
    (void*) isr_cmp2,
    (void*) isr_ftm0,
    (void*) isr_ftm1,
    (void*) isr_ftm2,
    (void*) isr_cmt,
    (void*) isr_rtc_alarm,
    (void*) isr_rtc_seconds,
    (void*) isr_pit0,
    (void*) isr_pit1,
    (void*) isr_pit2,
    (void*) isr_pit3,
    (void*) isr_pdb,
    (void*) isr_usb_otg,
    (void*) isr_usb_charger_detect,
    (void*) isr_enet_1588_timer,
    (void*) isr_enet_tx,
    (void*) isr_enet_rx,
    (void*) isr_enet_error_misc,
    (void*) dummy_handler,
    (void*) isr_sdhc,
    (void*) isr_dac0,
    (void*) dummy_handler,
    (void*) isr_tsi,
    (void*) isr_mcg,
    (void*) isr_lptmr0,
    (void*) dummy_handler,
    (void*) isr_porta,
    (void*) isr_portb,
    (void*) isr_portc,
    (void*) isr_portd,
    (void*) isr_porte,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) isr_software, /* Vector 110 */
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler,
    (void*) dummy_handler /* vector 255 */
};

/** @} */
