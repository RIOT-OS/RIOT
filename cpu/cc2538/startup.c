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
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include <stdint.h>

#include "board.h"

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
void dummy_handler(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_nmi(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_mem_manage(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_debug_mon(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_hard_fault(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_bus_fault(void)
{
    while (1) {
        asm("nop");
    }
}

void isr_usage_fault(void)
{
    while (1) {
        asm("nop");
    }
}


/* Cortex-M specific interrupt vectors */
void isr_svc(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)       __attribute__ ((weak, alias("dummy_handler")));
/* CC2538 specific interrupt vector */
void isr_gpioa(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_gpiob(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_gpioc(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_gpiod(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_ssi0(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_watchdog(void)      __attribute__ ((weak, alias("dummy_handler")));
void isr_timer0_chan0(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer0_chan1(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer1_chan0(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer1_chan1(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer2_chan0(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer2_chan1(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_comp(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_rfcoretx(void)      __attribute__ ((weak, alias("dummy_handler")));
void isr_rfcoreerr(void)     __attribute__ ((weak, alias("dummy_handler")));
void isr_icepick(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_flash(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_aes(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_pka(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_sleepmode(void)     __attribute__ ((weak, alias("dummy_handler")));
void isr_mactimer(void)      __attribute__ ((weak, alias("dummy_handler")));
void isr_ssi1(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_timer3_chan0(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer3_chan1(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_usb(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_dma(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_dmaerr(void)        __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors"), aligned(CC2538_VTOR_ALIGN)))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) isr_mem_manage,         /* memory controller interrupt */
    (void*) isr_bus_fault,          /* also not good to end up here */
    (void*) isr_usage_fault,        /* autsch */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) isr_debug_mon,          /* debug interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* CC2538 specific peripheral handlers */
    (void*) isr_gpioa,              /* 16 GPIO Port A */
    (void*) isr_gpiob,              /* 17 GPIO Port B */
    (void*) isr_gpioc,              /* 18 GPIO Port C */
    (void*) isr_gpiod,              /* 19 GPIO Port D */
    (void*) (0UL),                  /* 20 none */
    (void*) isr_uart0,              /* 21 UART0 Rx and Tx */
    (void*) isr_uart1,              /* 22 UART1 Rx and Tx */
    (void*) isr_ssi0,               /* 23 SSI0 Rx and Tx */
    (void*) isr_i2c,                /* 24 I2C Master and Slave */
    (void*) (0UL),                  /* 25 Reserved */
    (void*) (0UL),                  /* 26 Reserved */
    (void*) (0UL),                  /* 27 Reserved */
    (void*) (0UL),                  /* 28 Reserved */
    (void*) (0UL),                  /* 29 Reserved */
    (void*) isr_adc,                /* 30 ADC Sequence 0 */
    (void*) (0UL),                  /* 31 Reserved */
    (void*) (0UL),                  /* 32 Reserved */
    (void*) (0UL),                  /* 33 Reserved */
    (void*) isr_watchdog,           /* 34 Watchdog timer, timer 0 */
    (void*) isr_timer0_chan0,       /* 35 Timer 0 subtimer A */
    (void*) isr_timer0_chan1,       /* 36 Timer 0 subtimer B */
    (void*) isr_timer1_chan0,       /* 37 Timer 1 subtimer A */
    (void*) isr_timer1_chan1,       /* 38 Timer 1 subtimer B */
    (void*) isr_timer2_chan0,       /* 39 Timer 2 subtimer A */
    (void*) isr_timer2_chan1,       /* 40 Timer 2 subtimer B */
    (void*) isr_comp,               /* 41 Analog Comparator 0 */
    (void*) isr_rfcoretx,           /* 42 RFCore Rx/Tx */
    (void*) isr_rfcoreerr,          /* 43 RFCore Error */
    (void*) isr_icepick,            /* 44 IcePick */
    (void*) isr_flash,              /* 45 FLASH Control */
    (void*) isr_aes,                /* 46 AES */
    (void*) isr_pka,                /* 47 PKA */
    (void*) isr_sleepmode,          /* 48 Sleep Timer */
    (void*) isr_mactimer,           /* 49 MacTimer */
    (void*) isr_ssi1,               /* 50 SSI1 Rx and Tx */
    (void*) isr_timer3_chan0,       /* 51 Timer 3 subtimer A */
    (void*) isr_timer3_chan1,       /* 52 Timer 3 subtimer B */
    (void*) (0UL),                  /* 53 Reserved */
    (void*) (0UL),                  /* 54 Reserved */
    (void*) (0UL),                  /* 55 Reserved */
    (void*) (0UL),                  /* 56 Reserved */
    (void*) (0UL),                  /* 57 Reserved */
    (void*) (0UL),                  /* 58 Reserved */
    (void*) (0UL),                  /* 59 Reserved */
    (void*) isr_usb,                /* 60 USB 2538 */
    (void*) (0UL),                  /* 61 Reserved */
    (void*) isr_dma,                /* 62 uDMA */
    (void*) isr_dmaerr,             /* 63 uDMA Error */
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
