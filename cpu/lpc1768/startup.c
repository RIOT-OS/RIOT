/******************************************************************************
 * @file:    startup.c
 * @purpose: CMSIS Cortex-M3 Core Device Startup File
 * @version: V1.02
 * @date:    9. November 2013
 *
 * @note: Integrated and adopted for RIOT by Oliver Hahm.
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

/**
 * @ingroup lpc1768
 * @{
 */

/**
 * @file
 * @brief       lpc1768 startup code
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include "LPC17xx.h"
#include "kernel_internal.h"

#define WEAK __attribute__ ((weak))

/*****************************************************************************
 *
 * Forward declaration of the default fault handlers.
 *
 *****************************************************************************/
/* System exception vector handler */
void WEAK       Reset_Handler(void);             /* Reset Handler */
void WEAK       NMI_Handler(void);               /* NMI Handler */
void WEAK       HardFault_Handler(void);         /* Hard Fault Handler */
void WEAK       MemManage_Handler(void);         /* MPU Fault Handler */
void WEAK       BusFault_Handler(void);          /* Bus Fault Handler */
void WEAK       UsageFault_Handler(void);        /* Usage Fault Handler */
void WEAK       SVC_Handler(void);               /* SVCall Handler */
void WEAK       DebugMon_Handler(void);          /* Debug Monitor Handler */
void WEAK       PendSV_Handler(void);            /* PendSV Handler */
void WEAK       SysTick_Handler(void);           /* SysTick Handler */

/* External interrupt vector handler */
void WEAK       WDT_IRQHandler(void);            /* Watchdog Timer */
void WEAK       TIMER0_IRQHandler(void);         /* Timer0 */
void WEAK       TIMER1_IRQHandler(void);         /* Timer1 */
void WEAK       TIMER2_IRQHandler(void);         /* Timer2 */
void WEAK       TIMER3_IRQHandler(void);         /* Timer3 */
void WEAK       UART0_IRQHandler(void);          /* UART0 */
void WEAK       UART1_IRQHandler(void);          /* UART1 */
void WEAK       UART2_IRQHandler(void);          /* UART2 */
void WEAK       UART3_IRQHandler(void);          /* UART3 */
void WEAK       PWM1_IRQHandler(void);           /* PWM1 */
void WEAK       I2C0_IRQHandler(void);           /* I2C0 */
void WEAK       I2C1_IRQHandler(void);           /* I2C1 */
void WEAK       I2C2_IRQHandler(void);           /* I2C2 */
void WEAK       SPI_IRQHandler(void);            /* SPI */
void WEAK       SSP0_IRQHandler(void);           /* SSP0 */
void WEAK       SSP1_IRQHandler(void);           /* SSP1 */
void WEAK       PLL0_IRQHandler(void);           /* PLL0 (Main PLL) */
void WEAK       RTC_IRQHandler(void);            /* Real Time Clock */
void WEAK       EINT0_IRQHandler(void);          /* External Interrupt 0 */
void WEAK       EINT1_IRQHandler(void);          /* External Interrupt 1 */
void WEAK       EINT2_IRQHandler(void);          /* External Interrupt 2 */
void WEAK       EINT3_IRQHandler(void);          /* External Interrupt 3 */
void WEAK       ADC_IRQHandler(void);            /* A/D Converter */
void WEAK       BOD_IRQHandler(void);            /* Brown Out Detect */
void WEAK       USB_IRQHandler(void);            /* USB */
void WEAK       CAN_IRQHandler(void);            /* CAN */
void WEAK       DMA_IRQHandler(void);            /* GP DMA */
void WEAK       I2S_IRQHandler(void);            /* I2S */
void WEAK       ENET_IRQHandler(void);           /* Ethernet */
void WEAK       RIT_IRQHandler(void);            /* Repetitive Interrupt Timer */
void WEAK       MCPWM_IRQHandler(void);          /* Motor Control PWM */
void WEAK       QEI_IRQHandler(void);            /* Quadrature Encoder Interface */
void WEAK       PLL1_IRQHandler(void);           /* PLL1 (USB PLL) */


/* Exported constants --------------------------------------------------------*/
extern unsigned long __etext;
extern unsigned long __sidata;      /* start address for the initialization values of the .data section. defined in linker script */
extern unsigned long __data_start__;        /* start address for the .data section. defined in linker script */
extern unsigned long __data_end__;      /* end address for the .data section. defined in linker script */

extern unsigned long __bss_start__;         /* start address for the .bss section. defined in linker script */
extern unsigned long __bss_end__;           /* end address for the .bss section. defined in linker script */

extern void _estack;        /* init value for the stack pointer. defined in linker script */


/* function prototypes ------------------------------------------------------*/
void Reset_Handler(void) __attribute__((__interrupt__));
extern void _CPUregTestPOST (void);

/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
#define STACK_SIZE                              0x00000200

__attribute__ ((section(".stackarea")))
/* static */ unsigned long pulStack[STACK_SIZE];


__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
        /* &_estack,                   // The initial stack pointer */
        (void (*)(void))((unsigned long)pulStack + sizeof(pulStack)),  // The initial stack pointer
        Reset_Handler,             /* Reset Handler */
        NMI_Handler,               /* NMI Handler */
        HardFault_Handler,         /* Hard Fault Handler */
        MemManage_Handler,         /* MPU Fault Handler */
        BusFault_Handler,          /* Bus Fault Handler */
        UsageFault_Handler,        /* Usage Fault Handler */
        0,                         /* Reserved */
        0,                         /* Reserved */
        0,                         /* Reserved */
        0,                         /* Reserved */
        SVC_Handler,               /* SVCall Handler */
        DebugMon_Handler,          /* Debug Monitor Handler */
        0,                         /* Reserved */
        PendSV_Handler,            /* PendSV Handler */
        SysTick_Handler,           /* SysTick Handler */

        // External Interrupts
        WDT_IRQHandler,            /* Watchdog Timer */
        TIMER0_IRQHandler,         /* Timer0 */
        TIMER1_IRQHandler,         /* Timer1 */
        TIMER2_IRQHandler,         /* Timer2 */
        TIMER3_IRQHandler,         /* Timer3 */
        UART0_IRQHandler,          /* UART0 */
        UART1_IRQHandler,          /* UART1 */
        UART2_IRQHandler,          /* UART2 */
        UART3_IRQHandler,          /* UART3 */
        PWM1_IRQHandler,           /* PWM1 */
        I2C0_IRQHandler,           /* I2C0 */
        I2C1_IRQHandler,           /* I2C1 */
        I2C2_IRQHandler,           /* I2C2 */
        SPI_IRQHandler,            /* SPI */
        SSP0_IRQHandler,           /* SSP0 */
        SSP1_IRQHandler,           /* SSP1 */
        PLL0_IRQHandler,           /* PLL0 (Main PLL) */
        RTC_IRQHandler,            /* Real Time Clock */
        EINT0_IRQHandler,          /* External Interrupt 0 */
        EINT1_IRQHandler,          /* External Interrupt 1 */
        EINT2_IRQHandler,          /* External Interrupt 2 */
        EINT3_IRQHandler,          /* External Interrupt 3 */
        ADC_IRQHandler,            /* A/D Converter */
        BOD_IRQHandler,            /* Brown Out Detect */
        USB_IRQHandler,            /* USB */
        CAN_IRQHandler,            /* CAN */
        DMA_IRQHandler,            /* GP DMA */
        I2S_IRQHandler,            /* I2S */
        ENET_IRQHandler,           /* Ethernet */
        RIT_IRQHandler,            /* Repetitive Interrupt Timer */
        MCPWM_IRQHandler,          /* Motor Control PWM */
        QEI_IRQHandler,            /* Quadrature Encoder Interface */
        PLL1_IRQHandler,           /* PLL1 (USB PLL) */
};

/*******************************************************************************
* Function Name  : Reset_Handler
* Description    : This is the code that gets called when the processor first starts execution
*              following a reset event.  Only the absolutely necessary set is performed,
*              after which the application supplied main() routine is called.
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void Reset_Handler(void)
{
    unsigned long *pulDest;

    /*
     * This used for cleaning AHBRAM0 section
     */
#if 0
    for (pulDest = ((unsigned long *)AHBRAM0_BASE); \
                    pulDest < ((unsigned long *)(AHBRAM0_BASE + AHBRAM0_SIZE)); \
                    pulDest++){
        *(pulDest++) = 0;
    }
#endif

    /*
     * This used for cleaning AHBRAM1 section
     */
#if 0
    for (pulDest = ((unsigned long *)AHBRAM1_BASE); \
                    pulDest < ((unsigned long *)(AHBRAM1_BASE + AHBRAM1_SIZE)); \
                    pulDest++){
        *(pulDest++) = 0;
    }
#endif

    /*
     * Copy the data segment initializers from flash to SRAM in ROM mode
     */
#if (__RAM_MODE__==0)
    unsigned long *pulSrc = &__sidata;
    for(pulDest = &__data_start__; pulDest < &__data_end__; )
    {
        *(pulDest++) = *(pulSrc++);
    }
#endif


    /*
     * Zero fill the bss segment.
     */
    for(pulDest = &__bss_start__; pulDest < &__bss_end__; )
    {
        *(pulDest++) = 0;
    }

    /*
     *  Call IEC60335 CPU register tests POST
     */
    //    __ASM volatile ("bl _CPUregTestPOST \t\n");

    /*
     * Call the application's entry point.
     */
    board_init();
    kernel_init();
}

/*****************************************************************************
 *
 * Provide weak aliases for each Exception handler to the default_handler.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 *
 ****************************************************************************/
#pragma weak MemManage_Handler = default_handler          /* MPU Fault Handler */
#pragma weak BusFault_Handler = default_handler           /* Bus Fault Handler */
#pragma weak UsageFault_Handler = default_handler         /* Usage Fault Handler */
#pragma weak SVC_Handler = default_handler                /* SVCall Handler */
#pragma weak DebugMon_Handler = default_handler           /* Debug Monitor Handler */
#pragma weak PendSV_Handler = default_handler             /* PendSV Handler */
#pragma weak SysTick_Handler = default_handler            /* SysTick Handler */

/* External interrupt vector handler */
#pragma weak WDT_IRQHandler = default_handler            /* Watchdog Timer */
#pragma weak TIMER0_IRQHandler = default_handler         /* Timer0 */
#pragma weak TIMER1_IRQHandler = default_handler         /* Timer1 */
#pragma weak TIMER2_IRQHandler = default_handler         /* Timer2 */
#pragma weak TIMER3_IRQHandler = default_handler         /* Timer3 */
#pragma weak UART0_IRQHandler = default_handler          /* UART0 */
#pragma weak UART1_IRQHandler = default_handler          /* UART1 */
#pragma weak UART2_IRQHandler = default_handler          /* UART2 */
#pragma weak UART3_IRQHandler = default_handler          /* UART3 */
#pragma weak PWM1_IRQHandler = default_handler           /* PWM1 */
#pragma weak I2C0_IRQHandler = default_handler           /* I2C0 */
#pragma weak I2C1_IRQHandler = default_handler           /* I2C1 */
#pragma weak I2C2_IRQHandler = default_handler           /* I2C2 */
#pragma weak SPI_IRQHandler = default_handler            /* SPI */
#pragma weak SSP0_IRQHandler = default_handler           /* SSP0 */
#pragma weak SSP1_IRQHandler = default_handler           /* SSP1 */
#pragma weak PLL0_IRQHandler = default_handler           /* PLL0 (Main PLL) */
#pragma weak RTC_IRQHandler = default_handler            /* Real Time Clock */
#pragma weak EINT0_IRQHandler = default_handler          /* External Interrupt 0 */
#pragma weak EINT1_IRQHandler = default_handler          /* External Interrupt 1 */
#pragma weak EINT2_IRQHandler = default_handler          /* External Interrupt 2 */
#pragma weak EINT3_IRQHandler = default_handler          /* External Interrupt 3 */
#pragma weak ADC_IRQHandler = default_handler            /* A/D Converter */
#pragma weak BOD_IRQHandler = default_handler            /* Brown Out Detect */
#pragma weak USB_IRQHandler = default_handler            /* USB */
#pragma weak CAN_IRQHandler = default_handler            /* CAN */
#pragma weak DMA_IRQHandler = default_handler            /* GP DMA */
#pragma weak I2S_IRQHandler = default_handler            /* I2S */
#pragma weak ENET_IRQHandler = default_handler           /* Ethernet */
#pragma weak RIT_IRQHandler = default_handler            /* Repetitive Interrupt Timer */
#pragma weak MCPWM_IRQHandler = default_handler          /* Motor Control PWM */
#pragma weak QEI_IRQHandler = default_handler            /* Quadrature Encoder Interface */
#pragma weak PLL1_IRQHandler = default_handler           /* PLL1 (USB PLL) */

/*****************************************************************************
 *
 * This is the code that gets called when the processor receives an unexpected
 * interrupt.  This simply enters an infinite loop, preserving the system state
 * for examination by a debugger.
 *
 ****************************************************************************/
void default_handler(void) {
    /* Go into an infinite loop. */
    while (1) {
    }
}
