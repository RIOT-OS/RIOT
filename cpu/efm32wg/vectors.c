/*
 * vectors.c
 *
 *  Created on: 1 wrz 2015
 *      Author: kkk
 */

/**
 * @ingroup		cpu_efm32wg
 * @{
 *
 * @file
 * @file	Interrupt vector definitions.
 *
 * @author	Mateusz Kubaszek <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* EFM32WG specific interrupts vectors */
void DMA_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void GPIO_EVEN_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void TIMER0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART0_RX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART0_TX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USB_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void ACMP0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void ADC0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void DAC0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void I2C0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void I2C1_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void GPIO_ODD_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void TIMER1_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void TIMER2_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void TIMER3_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART1_RX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART1_TX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void LESENSE_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART2_RX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void USART2_TX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void UART0_RX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void UART0_TX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void UART1_RX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void UART1_TX_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void LEUART0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void LEUART1_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void LETIMER0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void PCNT0_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void PCNT1_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void PCNT2_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void RTC_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void BURTC_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void CMU_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void VCMP_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void LCD_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void MSC_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void AES_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void EBI_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void EMU_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));
void FPUEH_IRQHandler(void) __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
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
    /* External interrupts */
    (void*)	DMA_IRQHandler,                       /*  0 - DMA       */
    (void*)	GPIO_EVEN_IRQHandler,                 /*  1 - GPIO_EVEN       */
    (void*)	TIMER0_IRQHandler,                    /*  2 - TIMER0       */
    (void*)	USART0_RX_IRQHandler,                 /*  3 - USART0_RX       */
    (void*)	USART0_TX_IRQHandler,                 /*  4 - USART0_TX       */
    (void*)	USB_IRQHandler,                       /*  5 - USB       */
    (void*)	ACMP0_IRQHandler,                     /*  6 - ACMP0       */
    (void*)	ADC0_IRQHandler,                      /*  7 - ADC0       */
    (void*)	DAC0_IRQHandler,                      /*  8 - DAC0       */
    (void*)	I2C0_IRQHandler,                      /*  9 - I2C0       */
    (void*)	I2C1_IRQHandler,                      /*  10 - I2C1       */
    (void*)	GPIO_ODD_IRQHandler,                  /*  11 - GPIO_ODD       */
    (void*)	TIMER1_IRQHandler,                    /*  12 - TIMER1       */
    (void*)	TIMER2_IRQHandler,                    /*  13 - TIMER2       */
    (void*)	TIMER3_IRQHandler,                    /*  14 - TIMER3       */
    (void*)	USART1_RX_IRQHandler,                 /*  15 - USART1_RX       */
    (void*)	USART1_TX_IRQHandler,                 /*  16 - USART1_TX       */
    (void*)	LESENSE_IRQHandler,                   /*  17 - LESENSE       */
    (void*)	USART2_RX_IRQHandler,                 /*  18 - USART2_RX       */
    (void*)	USART2_TX_IRQHandler,                 /*  19 - USART2_TX       */
    (void*)	UART0_RX_IRQHandler,                  /*  20 - UART0_RX       */
    (void*)	UART0_TX_IRQHandler,                  /*  21 - UART0_TX       */
    (void*)	UART1_RX_IRQHandler,                  /*  22 - UART1_RX       */
    (void*)	UART1_TX_IRQHandler,                  /*  23 - UART1_TX       */
    (void*)	LEUART0_IRQHandler,                   /*  24 - LEUART0       */
    (void*)	LEUART1_IRQHandler,                   /*  25 - LEUART1       */
    (void*)	LETIMER0_IRQHandler,                  /*  26 - LETIMER0       */
    (void*)	PCNT0_IRQHandler,                     /*  27 - PCNT0       */
    (void*)	PCNT1_IRQHandler,                     /*  28 - PCNT1       */
    (void*)	PCNT2_IRQHandler,                     /*  29 - PCNT2       */
    (void*)	RTC_IRQHandler,                       /*  30 - RTC       */
    (void*)	BURTC_IRQHandler,                     /*  31 - BURTC       */
    (void*)	CMU_IRQHandler,                       /*  32 - CMU       */
    (void*)	VCMP_IRQHandler,                      /*  33 - VCMP       */
    (void*)	LCD_IRQHandler,                       /*  34 - LCD       */
    (void*)	MSC_IRQHandler,                       /*  35 - MSC       */
    (void*)	AES_IRQHandler,                       /*  36 - AES       */
    (void*)	EBI_IRQHandler,                       /*  37 - EBI       */
    (void*)	EMU_IRQHandler,                       /*  38 - EMU       */
    (void*)	FPUEH_IRQHandler,                     /*  39 - FPUEH       */
};
