//*****************************************************************************
// startup_gcc.c
//
// Startup code for use with GCC.
//
// Original work Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
// Modified work Copyright 2015 Attilio Dona'
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <stdint.h>
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "kernel_internal.h"
#include "periph_conf.h"

#if UART_0_EN
void UART_0_ISR(void);
#else

static void IntDefaultHandler10(void)
{
    while(1) {}
}

#endif


//*****************************************************************************
//
// Heap block pointers defined by linker script
//
//*****************************************************************************
static char *heap_end = 0;
extern unsigned long _heap;
extern unsigned long _eheap;

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void) __attribute__(( naked ));
static void IntDefaultHandler(void);
static void BusFaultHandler(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t pui32Stack[1024];

void isr_svc(void);
void isr_pendsv(void);

static void isr_systick(void)
{
    while(1) {}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void IntDefaultHandler(void)
{
    while(1) {}
}
static void IntDefaultHandler1(void)
{
    while(1) {}
}
static void IntDefaultHandler3(void)
{
    while(1) {}
}
static void IntDefaultHandler6(void)
{
    while(1) {}
}
static void IntDefaultHandler7(void)
{
    while(1) {}
}
static void IntDefaultHandler8(void)
{
    while(1) {}
}
static void IntDefaultHandler9(void)
{
    while(1) {}
}
static void IntDefaultHandler11(void)
{
    while(1) {}
}
static void IntDefaultHandler12(void)
{
    while(1) {}
}
static void IntDefaultHandler13(void)
{
    while(1) {}
}
static void IntDefaultHandler14(void)
{
    while(1) {}
}
static void IntDefaultHandler15(void)
{
    while(1) {}
}
static void IntDefaultHandler16(void)
{
    while(1) {}
}
static void IntDefaultHandler17(void)
{
    while(1) {}
}
static void IntDefaultHandler18(void)
{
    while(1) {}
}
static void IntDefaultHandler19(void)
{
    while(1) {}
}
static void IntDefaultHandler20(void)
{
    while(1) {}
}
static void IntDefaultHandler21(void)
{
    while(1) {}
}
static void IntDefaultHandler22(void)
{
    while(1) {}
}
static void IntDefaultHandler23(void)
{
    while(1) {}
}
static void IntDefaultHandler24(void)
{
    while(1) {}
}
static void IntDefaultHandler25(void)
{
    while(1) {}
}
static void IntDefaultHandler26(void)
{
    while(1) {}
}
static void IntDefaultHandler27(void)
{
    while(1) {}
}
static void IntDefaultHandler28(void)
{
    while(1) {}
}


//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".intvecs")))
void (* const g_pfnVectors[256])(void) =
{
    (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NmiSR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    BusFaultHandler,                        // The bus fault handler
    IntDefaultHandler1,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
	isr_svc,                      /* system call interrupt, in RIOT used for
     	 	 	 	 	 	 	   * switching into thread context on boot */
    IntDefaultHandler3,                      // Debug monitor handler
    0,                                      // Reserved
	isr_pendsv,                              /* pendSV interrupt, in RIOT the actual
     	 	 	 	 	 	 	 	 	 	  * context switching is happening here */
	isr_systick,                             /* SysTick interrupt, not used in RIOT */

	IntDefaultHandler6,                      // GPIO Port A0
    IntDefaultHandler7,                      // GPIO Port A1
    IntDefaultHandler8,                      // GPIO Port A2
    IntDefaultHandler9,                      // GPIO Port A3
    0,                                       // Reserved
	UART_0_ISR,                              // UART0 Rx and Tx
    IntDefaultHandler11,                     // UART1 Rx and Tx
    0,                                       // Reserved
    IntDefaultHandler12,                      // I2C0 Master and Slave
    0,0,0,0,0,                                // Reserved
    IntDefaultHandler13,                      // ADC Channel 0
    IntDefaultHandler14,                      // ADC Channel 1
    IntDefaultHandler15,                      // ADC Channel 2
    IntDefaultHandler16,                      // ADC Channel 3
    IntDefaultHandler17,                      // Watchdog Timer
    IntDefaultHandler18,                      // Timer 0 subtimer A  35*4 = 0x8C
    IntDefaultHandler19,                      // Timer 0 subtimer B
    IntDefaultHandler20,                      // Timer 1 subtimer A
    IntDefaultHandler21,                      // Timer 1 subtimer B
    IntDefaultHandler22,                      // Timer 2 subtimer A
    IntDefaultHandler23,                      // Timer 2 subtimer B
    0,0,0,0,                                // Reserved
    IntDefaultHandler24,                      // Flash
    0,0,0,0,0,                              // Reserved
    IntDefaultHandler25,                      // Timer 3 subtimer A
    IntDefaultHandler26,                      // Timer 3 subtimer B
    0,0,0,0,0,0,0,0,0,                      // Reserved
    IntDefaultHandler27,                      // uDMA Software Transfer
    IntDefaultHandler28,                      // uDMA Error
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    IntDefaultHandler,                      // SHA
    0,0,                                    // Reserved
    IntDefaultHandler,                      // AES
    0,                                      // Reserved
    IntDefaultHandler,                      // DES
    0,0,0,0,0,                              // Reserved
    IntDefaultHandler,                      // SDHost
    0,                                      // Reserved
    IntDefaultHandler,                      // I2S
    0,                                      // Reserved
    IntDefaultHandler,                      // Camera
    0,0,0,0,0,0,0,                          // Reserved
    IntDefaultHandler,                      // NWP to APPS Interrupt
    IntDefaultHandler,                      // Power, Reset and Clock module
    0,0,                                    // Reserved
    IntDefaultHandler,                      // Shared SPI
    IntDefaultHandler,                      // Generic SPI
    IntDefaultHandler,                      // Link SPI
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0                                     // Reserved
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t __init_data;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    uint32_t *pui32Src, *pui32Dest;

    //
    // Copy the data segment initializers
    //
    pui32Src = &__init_data;
    for(pui32Dest = &_data; pui32Dest < &_edata; )
    {
        *pui32Dest++ = *pui32Src++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");


    /* initialize the board (which also initiates CPU initialization) */
    board_init();

#ifdef MODULE_NEWLIB
    /* initialize std-c library (this must be done after board_init) */
    extern void __libc_init_array(void);
    __libc_init_array();
#endif

    /* startup the kernel */
    kernel_init();

}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
NmiSR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}


//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
FaultISR(void)
{
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler2_address_const                            \n"
			" bx r2                                                     \n"
			" handler2_address_const: .word prvGetRegistersFromStack    \n"
	);
}


#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];

	/* When the following line is hit, the variables contain the register values. */
	for( ;; );
}
#pragma GCC diagnostic pop







//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************

static void
BusFaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}


//*****************************************************************************
//
// This function is used by dynamic memory allocation API(s) in newlib 
// library
//
//*****************************************************************************
void * _sbrk(unsigned int incr)
{

    char *prev_heap_end;

    //
    // Check if this function is calld for the
    // first time and the heap end pointer
    //
    if (heap_end == 0)
    {
        heap_end = (char *)&_heap; 
    }

    //
    // Check if we have enough heap memory available
    //
    prev_heap_end = heap_end;
    if (heap_end + incr > (char *)&_eheap)
    {
    //
    // Return error
    //
        return 0;
    }

    //
    // Set the new heap end pointer
    //
    heap_end += incr;
    
    //
    // Return the pointer to the newly allocated memory
    //
    return prev_heap_end;

}
