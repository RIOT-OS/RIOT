/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include <stdint.h>

#include "cc2538-gpio.h"
#include "board.h"
#include "periph_conf.h"

/**
 * @brief Convert an interrupt number to an interrupt vector offset
 *
 * @param[in] inum Interrupt number as defined in the "IRQn" enum, may be negative.
 *
 * @return Corresponding nonnegative vector offset
*/
#define INUM_TO_IVEC(inum) ( (inum) + 16 )

void isr_uart0(void)           __attribute__((weak, alias("dummy_handler")));
void isr_uart1(void)           __attribute__((weak, alias("dummy_handler")));
void default_handler(void)     __attribute__((weak, alias("dummy_handler")));

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;


/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate;) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero;) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
__attribute__((naked))
void dummy_handler(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_nmi(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_mem_manage(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_debug_mon(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_hard_fault(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_bus_fault(void)
{
    while (1) {
        asm("nop");
    }
}

__attribute__((naked))
void isr_usage_fault(void)
{
    while (1) {
        asm("nop");
    }
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    [0]                                   = &_estack,        /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    [INUM_TO_IVEC(ResetHandler_IRQn)]     = reset_handler,   /* entry point of the program */
    [INUM_TO_IVEC(NonMaskableInt_IRQn)]   = isr_nmi,         /* non maskable interrupt handler */
    [INUM_TO_IVEC(HardFault_IRQn)]        = isr_hard_fault,  /* if you end up here its not good */
    [INUM_TO_IVEC(MemoryManagement_IRQn)] = isr_mem_manage,  /* memory controller interrupt */
    [INUM_TO_IVEC(BusFault_IRQn)]         = isr_bus_fault,   /* also not good to end up here */
    [INUM_TO_IVEC(UsageFault_IRQn)]       = isr_usage_fault, /* autsch */
    [INUM_TO_IVEC(SVCall_IRQn)]           = isr_svc,         /* system call interrupt */
    [INUM_TO_IVEC(DebugMonitor_IRQn)]     = isr_debug_mon,   /* debug interrupt */
    [INUM_TO_IVEC(PendSV_IRQn)]           = isr_pendsv,      /* pendSV interrupt, used for task switching in RIOT */
    [INUM_TO_IVEC(SysTick_IRQn)]          = isr_systick,     /* SysTick interrupt, not used in RIOT */

    /* CC2538 specific peripheral handlers */
    [INUM_TO_IVEC(GPIO_PORT_A_IRQn)]      = gpio_port_a_isr,  /**<  GPIO port A */
    [INUM_TO_IVEC(GPIO_PORT_B_IRQn)]      = gpio_port_b_isr,  /**<  GPIO port B */
    [INUM_TO_IVEC(GPIO_PORT_C_IRQn)]      = gpio_port_c_isr,  /**<  GPIO port C */
    [INUM_TO_IVEC(GPIO_PORT_D_IRQn)]      = gpio_port_d_isr,  /**<  GPIO port D */
    [INUM_TO_IVEC(UART_0_IRQ)]            = isr_uart0,        /**<  RIOT UART 0 */
    [INUM_TO_IVEC(UART_1_IRQ)]            = isr_uart1,        /**<  RIOT UART 1 */
    [INUM_TO_IVEC(SSI0_IRQn)]             = default_handler,  /**<  SSI0 */
    [INUM_TO_IVEC(I2C_IRQn)]              = default_handler,  /**<  I2C */
    [INUM_TO_IVEC(ADC_IRQn)]              = default_handler,  /**<  ADC */
    [INUM_TO_IVEC(WDT_IRQn)]              = default_handler,  /**<  Watchdog Timer */

#if TIMER_0_EN
    [INUM_TO_IVEC(TIMER_0_IRQn_1)]        = isr_timer0_chan0, /**<  RIOT Timer 0 Channel 0 */
    [INUM_TO_IVEC(TIMER_0_IRQn_2)]        = isr_timer0_chan1, /**<  RIOT Timer 0 Channel 1 */
#endif

#if TIMER_1_EN
    [INUM_TO_IVEC(TIMER_1_IRQn_1)]        = isr_timer1_chan0, /**<  RIOT Timer 1 Channel 0 */
    [INUM_TO_IVEC(TIMER_1_IRQn_2)]        = isr_timer1_chan1, /**<  RIOT Timer 1 Channel 1 */
#endif

#if TIMER_2_EN
    [INUM_TO_IVEC(TIMER_2_IRQn_1)]        = isr_timer2_chan0, /**<  RIOT Timer 2 Channel 0 */
    [INUM_TO_IVEC(TIMER_2_IRQn_2)]        = isr_timer2_chan1, /**<  RIOT Timer 2 Channel 1 */
#endif

#if TIMER_3_EN
    [INUM_TO_IVEC(TIMER_3_IRQn_1)]        = isr_timer3_chan0, /**<  RIOT Timer 3 Channel 0 */
    [INUM_TO_IVEC(TIMER_3_IRQn_2)]        = isr_timer3_chan1, /**<  RIOT Timer 3 Channel 1 */
#endif

    [INUM_TO_IVEC(ADC_CMP_IRQn)]          = default_handler,  /**<  Analog Comparator */
    [INUM_TO_IVEC(RF_RXTX_ALT_IRQn)]      = default_handler,  /**<  RF TX/RX (Alternate) */
    [INUM_TO_IVEC(RF_ERR_ALT_IRQn)]       = default_handler,  /**<  RF Error (Alternate) */
    [INUM_TO_IVEC(SYS_CTRL_IRQn)]         = default_handler,  /**<  System Control */
    [INUM_TO_IVEC(FLASH_CTRL_IRQn)]       = default_handler,  /**<  Flash memory control */
    [INUM_TO_IVEC(AES_ALT_IRQn)]          = default_handler,  /**<  AES (Alternate) */
    [INUM_TO_IVEC(PKA_ALT_IRQn)]          = default_handler,  /**<  PKA (Alternate) */
    [INUM_TO_IVEC(SM_TIMER_ALT_IRQn)]     = default_handler,  /**<  SM Timer (Alternate) */
    [INUM_TO_IVEC(MAC_TIMER_ALT_IRQn)]    = default_handler,  /**<  MAC Timer (Alternate) */
    [INUM_TO_IVEC(SSI1_IRQn)]             = default_handler,  /**<  SSI1 */
    [INUM_TO_IVEC(UDMA_IRQn)]             = default_handler,  /**<  uDMA software */
    [INUM_TO_IVEC(UDMA_ERR_IRQn)]         = default_handler,  /**<  uDMA error */
    [INUM_TO_IVEC(USB_IRQn)]              = default_handler,  /**<  USB */
    [INUM_TO_IVEC(RF_RXTX_IRQn)]          = default_handler,  /**<  RF Core Rx/Tx */
    [INUM_TO_IVEC(RF_ERR_IRQn)]           = default_handler,  /**<  RF Core Error */
    [INUM_TO_IVEC(AES_IRQn)]              = default_handler,  /**<  AES */
    [INUM_TO_IVEC(PKA_IRQn)]              = default_handler,  /**<  PKA */
    [INUM_TO_IVEC(SM_TIMER_IRQn)]         = default_handler,  /**<  SM Timer */
    [INUM_TO_IVEC(MACTIMER_IRQn)]         = default_handler,  /**<  MAC Timer */
};

#if UPDATE_CCA
/**
 * @brief Flash Customer Configuration Area (CCA)
 *
 * Defines bootloader backdoor configuration, boot image validity and base address, and flash page lock bits.
 */
__attribute__((section(".flashcca"), used))
const uint32_t cca[] = {
    /* Bootloader Backdoor Configuration: */
    0xe0ffffff | (CCA_BACKDOOR_ENABLE << 28) | (CCA_BACKDOOR_ACTIVE_LEVEL << 27) | (CCA_BACKDOOR_PORT_A_PIN << 24),
    0x00000000,                  /**< Image Valid */
    (uintptr_t)interrupt_vector, /**< Application Entry Point */

    /* Unlock all pages and debug: */
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
#endif

/** @} */
