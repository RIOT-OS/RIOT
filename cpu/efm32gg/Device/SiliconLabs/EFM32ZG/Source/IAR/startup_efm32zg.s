;/**************************************************************************//**
; * @file startup_efm32zg.s
; * @brief    CMSIS Core Device Startup File
; *           Silicon Labs EFM32ZG Device Series
; * @version 3.20.6
; * @date     30. January 2012
; *
; * @note
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
;
; When debugging in RAM, it can be located in RAM with at least a 128 byte
; alignment, 256 byte alignment is requied if all interrupt vectors are in use.
;
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;
        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(8)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     0
        DCD     0
        DCD     0
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     0
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts
        DCD DMA_IRQHandler  ; 0: DMA Interrupt
        DCD GPIO_EVEN_IRQHandler  ; 1: GPIO_EVEN Interrupt
        DCD TIMER0_IRQHandler  ; 2: TIMER0 Interrupt
        DCD ACMP0_IRQHandler  ; 3: ACMP0 Interrupt
        DCD ADC0_IRQHandler  ; 4: ADC0 Interrupt
        DCD I2C0_IRQHandler  ; 5: I2C0 Interrupt
        DCD GPIO_ODD_IRQHandler  ; 6: GPIO_ODD Interrupt
        DCD TIMER1_IRQHandler  ; 7: TIMER1 Interrupt
        DCD USART1_RX_IRQHandler  ; 8: USART1_RX Interrupt
        DCD USART1_TX_IRQHandler  ; 9: USART1_TX Interrupt
        DCD LEUART0_IRQHandler  ; 10: LEUART0 Interrupt
        DCD PCNT0_IRQHandler  ; 11: PCNT0 Interrupt
        DCD RTC_IRQHandler  ; 12: RTC Interrupt
        DCD CMU_IRQHandler  ; 13: CMU Interrupt
        DCD VCMP_IRQHandler  ; 14: VCMP Interrupt
        DCD MSC_IRQHandler  ; 15: MSC Interrupt
        DCD AES_IRQHandler  ; 16: AES Interrupt


__Vectors_End
__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        ; Device specific interrupt handlers

        PUBWEAK DMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_IRQHandler
        B DMA_IRQHandler

        PUBWEAK GPIO_EVEN_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO_EVEN_IRQHandler
        B GPIO_EVEN_IRQHandler

        PUBWEAK TIMER0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER0_IRQHandler
        B TIMER0_IRQHandler

        PUBWEAK ACMP0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ACMP0_IRQHandler
        B ACMP0_IRQHandler

        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC0_IRQHandler
        B ADC0_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK GPIO_ODD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO_ODD_IRQHandler
        B GPIO_ODD_IRQHandler

        PUBWEAK TIMER1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER1_IRQHandler
        B TIMER1_IRQHandler

        PUBWEAK USART1_RX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_RX_IRQHandler
        B USART1_RX_IRQHandler

        PUBWEAK USART1_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_TX_IRQHandler
        B USART1_TX_IRQHandler

        PUBWEAK LEUART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LEUART0_IRQHandler
        B LEUART0_IRQHandler

        PUBWEAK PCNT0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PCNT0_IRQHandler
        B PCNT0_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK CMU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CMU_IRQHandler
        B CMU_IRQHandler

        PUBWEAK VCMP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VCMP_IRQHandler
        B VCMP_IRQHandler

        PUBWEAK MSC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MSC_IRQHandler
        B MSC_IRQHandler

        PUBWEAK AES_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
AES_IRQHandler
        B AES_IRQHandler


        END
