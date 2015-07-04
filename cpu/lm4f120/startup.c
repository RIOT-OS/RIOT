/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file		startup.c - Startup code
 * @brief       LM4F120H5QR startup code and interrupt vector definition.
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"

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


//-----------------------------------------------------------------------------
// 							 Functions declarations
//-----------------------------------------------------------------------------
void reset_handler(void);
static void NmiSR(void);
static void FaultISR(void);
extern void empty_def_handler(void);


extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * Required by g++ cross compiler
 */
void *__dso_handle;


// External declaration for the interrupt handler used by the application
extern void UARTIntHandler(void);  		// UART 0							21
extern void UART1IntHandler(void);  		// UART 1							21
extern void	TIMER0IntHandler(void);		// 16 bit timer 0 A
extern void	WTIMER0IntHandler(void);		// 32 bit timer 0 A
extern void	TIMER1IntHandler(void);		// 16 bit timer 1 A
extern void isr_svc(void);				// SV call
extern void isr_pendsv(void);     		// PendSV
extern void isr_bus_fault(void);    	// Bus Fault
extern void isr_usage_fault(void);		// Usage Fault

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /* entry point of the program */
    NmiSR,					// NMI handler.						2
    FaultISR,				// hard fault handler.				3
    // Configurable priority interruts handler start here.
    empty_def_handler,		// Memory Management Fault			4
    isr_bus_fault,	    	// Bus Fault						5
    isr_usage_fault,		// Usage Fault 						6
    0,						// Reserved							7
    0,						// Reserved							8
    0,						// Reserved							9
    0,						// Reserved							10
    isr_svc,        		// SV call							11
    empty_def_handler,		// Debug monitor					12
    0,						// Reserved							13
    isr_pendsv,     		// PendSV							14
    empty_def_handler,		// SysTick							15
    // Peripherial interrupts start here.
	empty_def_handler,		// GPIO Port A						16
	empty_def_handler,		// GPIO Port B						17
	empty_def_handler,		// GPIO Port C						18
	empty_def_handler,		// GPIO Port D						19
	empty_def_handler,		// GPIO Port E						20
	UARTIntHandler,  		// UART 0							21
	UART1IntHandler,		// UART 1							22
	empty_def_handler,		// SSI 0							23
	empty_def_handler,		// I2C 0							24
	0,						// Reserved							25
	0,						// Reserved							26
	0,						// Reserved							27
	0,						// Reserved							28
	0,						// Reserved							29
	empty_def_handler,		// ADC 0 Seq 0						30
	empty_def_handler,		// ADC 0 Seq 1						31
	empty_def_handler,		// ADC 0 Seq 2						32
	empty_def_handler,		// ADC 0 Seq 3						33
	empty_def_handler,		// WDT 0 and 1						34
	TIMER0IntHandler,		// 16/32 bit timer 0 A				35
	empty_def_handler,		// 16/32 bit timer 0 B				36
	TIMER1IntHandler,		// 16/32 bit timer 1 A				37
	empty_def_handler,		// 16/32 bit timer 1 B				38
	empty_def_handler,		// 16/32 bit timer 2 A				39
	empty_def_handler,		// 16/32 bit timer 2 B				40
	empty_def_handler,		// Analog comparator 0				41
	empty_def_handler,		// Analog comparator 1				42
	0,						// Reserved							43
	empty_def_handler,		// System control					44
	empty_def_handler,		// Flash + EEPROM control			45
	empty_def_handler,		// GPIO Port F						46
	0,						// Reserved							47
	0,						// Reserved							48
	empty_def_handler,		// UART 2							49
	empty_def_handler,		// SSI 1							50
	empty_def_handler,		// 16/32 bit timer 3 A				51
	empty_def_handler,		// 16/32 bit timer 3 B				52
	empty_def_handler,		// I2C 1							53
	0,						// Reserved							54
	empty_def_handler,		// CAN 0							55
	0,						// Reserved							56
	0,						// Reserved							57
	0,						// Reserved							58
	empty_def_handler,		// Hibernation module				59
	empty_def_handler,		// USB								60
	0,						// Reserved							61
	empty_def_handler,		// UDMA SW							62
	empty_def_handler,		// UDMA Error						63
	empty_def_handler,		// ADC 1 Seq 0						64
	empty_def_handler,		// ADC 1 Seq 1						65
	empty_def_handler,		// ADC 1 Seq 2						66
	empty_def_handler,		// ADC 1 Seq 3						67
	0,						// Reserved							68
	0,						// Reserved							69
	0,						// Reserved							70
	0,						// Reserved							71
	0,						// Reserved							72
	empty_def_handler,		// SSI 2							73
	empty_def_handler,		// SSI 2							74
	empty_def_handler,		// UART 3							75
	empty_def_handler,		// UART 4							76
	empty_def_handler,		// UART 5							77
	empty_def_handler,		// UART 6							78
	empty_def_handler,		// UART 7							79
	0,						// Reserved							80
	0,						// Reserved							81
	0,						// Reserved							82
	0,						// Reserved							83
	empty_def_handler,		// I2C 2							84
	empty_def_handler,		// I2C 4							85
	empty_def_handler,		// 16/32 bit timer 4 A				86
	empty_def_handler,		// 16/32 bit timer 4 B				87
	0,						// Reserved							88
	0,						// Reserved							89
	0,						// Reserved							90
	0,						// Reserved							91
	0,						// Reserved							92
	0,						// Reserved							93
	0,						// Reserved							94
	0,						// Reserved							95
	0,						// Reserved							96
	0,						// Reserved							97
	0,						// Reserved							98
	0,						// Reserved							99
	0,						// Reserved							100
	0,						// Reserved							101
	0,						// Reserved							102
	0,						// Reserved							103
	0,						// Reserved							104
	0,						// Reserved							105
	0,						// Reserved							106
	0,						// Reserved							107
	empty_def_handler,		// 16/32 bit timer 5 A				108
	empty_def_handler,		// 16/32 bit timer 5 B				109
	WTIMER0IntHandler,		// 32/64 bit timer 0 A				110
	empty_def_handler,		// 32/64 bit timer 0 B				111
	empty_def_handler,		// 32/64 bit timer 1 A				112
	empty_def_handler,		// 32/64 bit timer 1 B				113
	empty_def_handler,		// 32/64 bit timer 2 A				114
	empty_def_handler,		// 32/64 bit timer 2 B				115
	empty_def_handler,		// 32/64 bit timer 3 A				116
	empty_def_handler,		// 32/64 bit timer 3 B				117
	empty_def_handler,		// 32/64 bit timer 4 A				118
	empty_def_handler,		// 32/64 bit timer 4 B				119
	empty_def_handler,		// 32/64 bit timer 5 A				120
	empty_def_handler,		// 32/64 bit timer 5 B				121
	empty_def_handler,		// System Exception					122
	0,						// Reserved							123
	0,						// Reserved							124
	0,						// Reserved							125
	0,						// Reserved							126
	0,						// Reserved							127
	0,						// Reserved							128
	0,						// Reserved							129
	0,						// Reserved							130
	0,						// Reserved							131
	0,						// Reserved							132
	0,						// Reserved							133
	0,						// Reserved							134
	0,						// Reserved							135
	0,						// Reserved							136
	0,						// Reserved							137
	0,						// Reserved							138
	0,						// Reserved							139
	0,						// Reserved							140
	0,						// Reserved							141
	0,						// Reserved							142
	0,						// Reserved							143
	0,						// Reserved							144
	0,						// Reserved							145
	0,						// Reserved							146
	0,						// Reserved							147
	0,						// Reserved							148
	0,						// Reserved							149
	0,						// Reserved							150
	0,						// Reserved							151
	0,						// Reserved							152
	0,						// Reserved							153
	0						// Reserved							154
};
//
//-----------------------------------------------------------------------------
// 							Function implementations.
//-----------------------------------------------------------------------------
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
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

// NMI Exception handler code NVIC 2
static
void NmiSR(void){
	// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

// Hard fault handler code NVIC 3
static
void FaultISR(void){
	// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

void isr_bus_fault(void){    	// Bus Fault
// Bus fault handler code
	// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

void isr_usage_fault(void){		// Usage Fault
// Usage fault handler code
// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

// Empty handler used as default.
void empty_def_handler(void){
	// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

void	TIMER1IntHandler(void){
// Just loop forever, so if you want to debug the processor it's running.
    while(1){
    }
}

void UART1IntHandler(void){
    while(1){
	}
}
