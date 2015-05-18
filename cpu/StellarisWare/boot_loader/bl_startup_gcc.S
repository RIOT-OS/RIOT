//*****************************************************************************
//
// bl_startup_gcc.S - Startup code for GNU.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

//*****************************************************************************
//
// Include the assember definitions used to make this code compiler
// independent.
//
//*****************************************************************************
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "bl_config.h"

//*****************************************************************************
//
// Put the assembler into the correct configuration.
//
//*****************************************************************************
    .syntax unified
    .thumb

//*****************************************************************************
//
// The stack gets placed into the zero-init section.
//
//*****************************************************************************
    .bss

//*****************************************************************************
//
// Allocate storage for the stack.
//
//*****************************************************************************
g_pulStack:
    .space  STACK_SIZE * 4

//*****************************************************************************
//
// This portion of the file goes into the text section.
//
//*****************************************************************************
    .section .isr_vector

//*****************************************************************************
//
// The minimal vector table for a Cortex-M3 processor.
//
//*****************************************************************************
Vectors:
    .word   g_pulStack + (STACK_SIZE * 4)   // Offset 00: Initial stack pointer
#ifdef FLASH_PATCH_COMPATIBLE
    .word   ResetISR - 0x1ffff000           // Offset 04: Reset handler
    .word   NmiSR - 0x1ffff000              // Offset 08: NMI handler
    .word   FaultISR - 0x1ffff000           // Offset 0C: Hard fault handler
#else
    .word   ResetISR - 0x20000000           // Offset 04: Reset handler
    .word   NmiSR - 0x20000000              // Offset 08: NMI handler
    .word   FaultISR - 0x20000000           // Offset 0C: Hard fault handler
#endif
    .word   IntDefaultHandler               // Offset 10: MPU fault handler
    .word   IntDefaultHandler               // Offset 14: Bus fault handler
    .word   IntDefaultHandler               // Offset 18: Usage fault handler
    .word   0                               // Offset 1C: Reserved
    .word   0                               // Offset 20: Reserved
    .word   0                               // Offset 24: Reserved
    .word   0                               // Offset 28: Reserved
#ifdef FLASH_PATCH_COMPATIBLE
    .word   UpdateHandler - 0x1ffff000      // Offset 2C: SVCall handler
#else
    .word   UpdateHandler - 0x20000000      // Offset 2C: SVCall handler
#endif
    .word   IntDefaultHandler               // Offset 30: Debug monitor handler
    .word   0                               // Offset 34: Reserved
    .word   IntDefaultHandler               // Offset 38: PendSV handler
#if defined(ENET_ENABLE_UPDATE)
    .extern SysTickIntHandler
    .word   SysTickIntHandler               // Offset 3C: SysTick handler
#else
    .word   IntDefaultHandler               // Offset 3C: SysTick handler
#endif
#if defined(UART_ENABLE_UPDATE) && defined(UART_AUTOBAUD)
    .extern GPIOIntHandler
    .word   GPIOIntHandler                  // Offset 40: GPIO port A handler
#else
    .word   IntDefaultHandler               // Offset 40: GPIO port A handler
#endif
#if (defined(USB_ENABLE_UPDATE) ||                                            \
     (APP_START_ADDRESS != VTABLE_START_ADDRESS) ||                           \
     defined(FLASH_PATCH_COMPATIBLE))
    .word   IntDefaultHandler               // Offset 44: GPIO Port B
    .word   IntDefaultHandler               // Offset 48: GPIO Port C
    .word   IntDefaultHandler               // Offset 4C: GPIO Port D
    .word   IntDefaultHandler               // Offset 50: GPIO Port E
    .word   IntDefaultHandler               // Offset 54: UART0 Rx and Tx
    .word   IntDefaultHandler               // Offset 58: UART1 Rx and Tx
    .word   IntDefaultHandler               // Offset 5C: SSI0 Rx and Tx
    .word   IntDefaultHandler               // Offset 60: I2C0 Master and Slave
    .word   IntDefaultHandler               // Offset 64: PWM Fault
    .word   IntDefaultHandler               // Offset 68: PWM Generator 0
    .word   IntDefaultHandler               // Offset 6C: PWM Generator 1
    .word   IntDefaultHandler               // Offset 70: PWM Generator 2
    .word   IntDefaultHandler               // Offset 74: Quadrature Encoder 0
    .word   IntDefaultHandler               // Offset 78: ADC Sequence 0
    .word   IntDefaultHandler               // Offset 7C: ADC Sequence 1
    .word   IntDefaultHandler               // Offset 80: ADC Sequence 2
    .word   IntDefaultHandler               // Offset 84: ADC Sequence 3
    .word   IntDefaultHandler               // Offset 88: Watchdog timer
    .word   IntDefaultHandler               // Offset 8C: Timer 0 subtimer A
    .word   IntDefaultHandler               // Offset 90: Timer 0 subtimer B
    .word   IntDefaultHandler               // Offset 94: Timer 1 subtimer A
    .word   IntDefaultHandler               // Offset 98: Timer 1 subtimer B
    .word   IntDefaultHandler               // Offset 9C: Timer 2 subtimer A
    .word   IntDefaultHandler               // Offset A0: Timer 2 subtimer B
    .word   IntDefaultHandler               // Offset A4: Analog Comparator 0
    .word   IntDefaultHandler               // Offset A8: Analog Comparator 1
    .word   IntDefaultHandler               // Offset AC: Analog Comparator 2
    .word   IntDefaultHandler               // Offset B0: System Control
#ifdef FLASH_PATCH_COMPATIBLE
    .word   0x00000881                      // Offset B4: FLASH Control
#else
    .word   IntDefaultHandler               // Offset B4: FLASH Control
#endif
#endif
#if (defined(USB_ENABLE_UPDATE) || (APP_START_ADDRESS != VTABLE_START_ADDRESS))
    .word   IntDefaultHandler               // Offset B8: GPIO Port F
    .word   IntDefaultHandler               // Offset BC: GPIO Port G
    .word   IntDefaultHandler               // Offset C0: GPIO Port H
    .word   IntDefaultHandler               // Offset C4: UART2 Rx and Tx
    .word   IntDefaultHandler               // Offset C8: SSI1 Rx and Tx
    .word   IntDefaultHandler               // Offset CC: Timer 3 subtimer A
    .word   IntDefaultHandler               // Offset D0: Timer 3 subtimer B
    .word   IntDefaultHandler               // Offset D4: I2C1 Master and Slave
    .word   IntDefaultHandler               // Offset D8: Quadrature Encoder 1
    .word   IntDefaultHandler               // Offset DC: CAN0
    .word   IntDefaultHandler               // Offset E0: CAN1
    .word   IntDefaultHandler               // Offset E4: CAN2
    .word   IntDefaultHandler               // Offset E8: Ethernet
    .word   IntDefaultHandler               // Offset EC: Hibernation module
#if defined(USB_ENABLE_UPDATE)
    .extern USB0DeviceIntHandler
    .word   USB0DeviceIntHandler            // Offset F0: USB 0 Controller
#else
    .word   IntDefaultHandler               // Offset F0: USB 0 Controller
#endif
#endif

//*****************************************************************************
//
// This portion of the file goes into the text section.
//
//*****************************************************************************
    .text

//*****************************************************************************
//
// Initialize the processor by copying the boot loader from flash to SRAM, zero
// filling the .bss section, and moving the vector table to the beginning of
// SRAM.  The return address is modified to point to the SRAM copy of the boot
// loader instead of the flash copy, resulting in a branch to the copy now in
// SRAM.
//
//*****************************************************************************
    .thumb_func
ProcessorInit:
    //
    // Copy the code image from flash to SRAM.
    //
#ifdef FLASH_PATCH_COMPATIBLE
    movs    r0, #0x1000
#else
    movs    r0, #0x0000
#endif
    movs    r1, #0x0000
    movt    r1, #0x2000
    .extern _bss
    ldr     r2, =_bss
copy_loop:
        ldr     r3, [r0], #4
        str     r3, [r1], #4
        cmp     r1, r2
        blt     copy_loop

    //
    // Zero fill the .bss section.
    //
    movs    r0, #0x0000
    .extern _ebss
    ldr     r2, =_ebss
zero_loop:
        str     r0, [r1], #4
        cmp     r1, r2
        blt     zero_loop

    //
    // Set the vector table pointer to the beginning of SRAM.
    //
    movw    r0, #(NVIC_VTABLE & 0xffff)
    movt    r0, #(NVIC_VTABLE >> 16)
    movs    r1, #0x0000
    movt    r1, #0x2000
    str     r1, [r0]

    //
    // Set the return address to the code just copied into SRAM.
    //
    orr     lr, lr, #0x20000000
#ifdef FLASH_PATCH_COMPATIBLE
    bic     lr, lr, #0x00001000
#endif

    //
    // Return to the caller.
    //
    bx      lr

//*****************************************************************************
//
// The reset handler, which gets called when the processor starts.
//
//*****************************************************************************
    .globl  ResetISR
    .thumb_func
ResetISR:
    //
    // Initialize the processor.
    //
    bl      ProcessorInit

    //
    // Call the user-supplied low level hardware initialization function
    // if provided.
    //
#ifdef BL_HW_INIT_FN_HOOK
    .extern BL_HW_INIT_FN_HOOK
    bl      BL_HW_INIT_FN_HOOK
#endif

    //
    // See if an update should be performed.
    //
    .extern CheckForceUpdate
    bl      CheckForceUpdate
    cbz     r0, CallApplication

    //
    // Configure the microcontroller.
    //
    .thumb_func
EnterBootLoader:
#ifdef ENET_ENABLE_UPDATE
    .extern ConfigureEnet
    bl      ConfigureEnet
#elif defined(CAN_ENABLE_UPDATE)
    .extern ConfigureCAN
    bl      ConfigureCAN
#elif defined(USB_ENABLE_UPDATE)
    .extern ConfigureUSB
    bl      ConfigureUSB
#else
    .extern ConfigureDevice
    bl      ConfigureDevice
#endif

    //
    // Call the user-supplied initialization function if provided.
    //
#ifdef BL_INIT_FN_HOOK
    .extern BL_INIT_FN_HOOK
    bl      BL_INIT_FN_HOOK
#endif

    //
    // Branch to the update handler.
    //
#ifdef ENET_ENABLE_UPDATE
    .extern UpdateBOOTP
    b       UpdateBOOTP
#elif defined(CAN_ENABLE_UPDATE)
    .extern UpdaterCAN
    b       UpdaterCAN
#elif defined(USB_ENABLE_UPDATE)
    .extern UpdaterUSB
    b       UpdaterUSB
#else
    .extern Updater
    b       Updater
#endif

    //
    // This is a second symbol to allow starting the application from the boot
    // loader the linker may not like the perceived jump.
    //
    .globl  StartApplication
    .thumb_func
StartApplication:
    //
    // Call the application via the reset handler in its vector table.  Load
    // the address of the application vector table.
    //
    .thumb_func
CallApplication:
    //
    // Copy the application's vector table to the target address if necessary.
    // Note that incorrect boot loader configuration could cause this to
    // corrupt the code!  Setting VTABLE_START_ADDRESS to 0x20000000 (the start
    // of SRAM) is safe since this will use the same memory that the boot loader
    // already uses for its vector table.  Great care will have to be taken if
    // other addresses are to be used.
    //
#if (APP_START_ADDRESS != VTABLE_START_ADDRESS)
    movw    r0, #(VTABLE_START_ADDRESS & 0xffff)
#if (VTABLE_START_ADDRESS > 0xffff)
    movt    r0, #(VTABLE_START_ADDRESS >> 16)
#endif
    movw    r1, #(APP_START_ADDRESS & 0xffff)
#if (APP_START_ADDRESS > 0xffff)
    movt    r1, #(APP_START_ADDRESS >> 16)
#endif

    //
    // Calculate the end address of the vector table assuming that it has the
    // maximum possible number of vectors.  We don't know how many the app has
    // populated so this is the safest approach though it may copy some non
    // vector data if the app table is smaller than the maximum.
    //
    movw    r2, #(70 * 4)
    adds    r2, r2, r0
VectorCopyLoop:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        cmp     r0, r2
        blt     VectorCopyLoop
#endif

    //
    // Set the application's vector table start address.  Typically this is the
    // application start address but in some cases an application may relocate
    // this so we can't assume that these two addresses are equal.
    //
    movw    r0, #(VTABLE_START_ADDRESS & 0xffff)
#if (VTABLE_START_ADDRESS > 0xffff)
    movt    r0, #(VTABLE_START_ADDRESS >> 16)
#endif
    movw    r1, #(NVIC_VTABLE & 0xffff)
    movt    r1, #(NVIC_VTABLE >> 16)
    str     r0, [r1]

    //
    // Load the stack pointer from the application's vector table.
    //
#if (APP_START_ADDRESS != VTABLE_START_ADDRESS)
    movw    r0, #(APP_START_ADDRESS & 0xffff)
#if (APP_START_ADDRESS > 0xffff)
    movt    r0, #(APP_START_ADDRESS >> 16)
#endif
#endif
    ldr     sp, [r0]

    //
    // Load the initial PC from the application's vector table and branch to
    // the application's entry point.
    //
    ldr     r0, [r0, #4]
    bx      r0

//*****************************************************************************
//
// The update handler, which gets called when the application would like to
// start an update.
//
//*****************************************************************************
    .thumb_func
UpdateHandler:
    //
    // Initialize the processor.
    //
    bl      ProcessorInit

    //
    // Load the stack pointer from the vector table.
    //
#ifdef FLASH_PATCH_COMPATIBLE
    movs    r0, #0x1000
#else
    movs    r0, #0x0000
#endif
    ldr     sp, [r0]

    //
    // Call the user-supplied low level hardware initialization function
    // if provided.
    //
#ifdef BL_HW_INIT_FN_HOOK
    bl      BL_HW_INIT_FN_HOOK
#endif

    //
    // Call the user-supplied re-initialization function if provided.
    //
#ifdef BL_REINIT_FN_HOOK
    .extern BL_REINIT_FN_HOOK
    bl      BL_REINIT_FN_HOOK
#endif

    //
    // Branch to the update handler.
    //
#ifdef ENET_ENABLE_UPDATE
    b       UpdateBOOTP
#elif defined(CAN_ENABLE_UPDATE)
    .extern AppUpdaterCAN
    b       AppUpdaterCAN
#elif defined(USB_ENABLE_UPDATE)
    .extern AppUpdaterUSB
    b       AppUpdaterUSB
#else
    b       Updater
#endif

//*****************************************************************************
//
// The NMI handler.
//
//*****************************************************************************
    .thumb_func
NmiSR:
#ifdef ENABLE_MOSCFAIL_HANDLER
    //
    // Grab the fault frame from the stack (the stack will be cleared by the
    // processor initialization that follows).
    //
    ldm     sp, {r4-r11}
    mov     r12, lr

    //
    // Initialize the processor.
    //
    bl      ProcessorInit

    //
    // Restore the stack frame.
    //
    mov     lr, r12
    stm     sp, {r4-r11}

    //
    // Save the link register.
    //
    mov     r9, lr

    //
    // Call the user-supplied low level hardware initialization function
    // if provided.
    //
#ifdef BL_HW_INIT_FN_HOOK
    bl      BL_HW_INIT_FN_HOOK
#endif

    //
    // See if an update should be performed.
    //
    bl      CheckForceUpdate
    cbz     r0, EnterApplication

        //
        // Clear the MOSCFAIL bit in RESC.
        //
        movw    r0, #(SYSCTL_RESC & 0xffff)
        movt    r0, #(SYSCTL_RESC >> 16)
        ldr     r1, [r0]
        bic     r1, r1, #SYSCTL_RESC_MOSCFAIL
        str     r1, [r0]

        //
        // Fix up the PC on the stack so that the boot pin check is bypassed
        // (since it has already been performed).
        //
        ldr     r0, =EnterBootLoader
        bic     r0, #0x00000001
        str     r0, [sp, #0x18]
        
        //
        // Return from the NMI handler.  This will then start execution of the
        // boot loader.
        //
        bx      r9

    //
    // Restore the link register.
    //
    .thumb_func
EnterApplication:
    mov     lr, r9

    //
    // Copy the application's vector table to the target address if necessary.
    // Note that incorrect boot loader configuration could cause this to
    // corrupt the code!  Setting VTABLE_START_ADDRESS to 0x20000000 (the start
    // of SRAM) is safe since this will use the same memory that the boot loader
    // already uses for its vector table.  Great care will have to be taken if
    // other addresses are to be used.
    //
#if (APP_START_ADDRESS != VTABLE_START_ADDRESS)
    movw    r0, #(VTABLE_START_ADDRESS & 0xffff)
#if (VTABLE_START_ADDRESS > 0xffff)
    movt    r0, #(VTABLE_START_ADDRESS >> 16)
#endif
    movw    r1, #(APP_START_ADDRESS & 0xffff)
#if (APP_START_ADDRESS > 0xffff)
    movt    r1, #(APP_START_ADDRESS >> 16)
#endif

    //
    // Calculate the end address of the vector table assuming that it has the
    // maximum possible number of vectors.  We don't know how many the app has
    // populated so this is the safest approach though it may copy some non
    // vector data if the app table is smaller than the maximum.
    //
    movw    r2, #(70 * 4)
    adds    r2, r2, r0
VectorCopyLoop2:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        cmp     r0, r2
        blt     VectorCopyLoop2
#endif

    //
    // Set the application's vector table start address.  Typically this is the
    // application start address but in some cases an application may relocate
    // this so we can't assume that these two addresses are equal.
    //
    movw    r0, #(VTABLE_START_ADDRESS & 0xffff)
#if (VTABLE_START_ADDRESS > 0xffff)
    movt    r0, #(VTABLE_START_ADDRESS >> 16)
#endif
    movw    r1, #(NVIC_VTABLE & 0xffff)
    movt    r1, #(NVIC_VTABLE >> 16)
    str     r0, [r1]

    //
    // Remove the NMI stack frame from the boot loader's stack.
    //
    ldmia   sp, {r4-r11}

    //
    // Get the application's stack pointer.
    //
#if (APP_START_ADDRESS != VTABLE_START_ADDRESS)
    movw    r0, #(APP_START_ADDRESS & 0xffff)
#if (APP_START_ADDRESS > 0xffff)
    movt    r0, #(APP_START_ADDRESS >> 16)
#endif
#endif
    ldr     sp, [r0, #0x00]

    //
    // Fix up the NMI stack frame's return address to be the reset handler of
    // the application.
    //
    ldr     r10, [r0, #0x04]
    bic     r10, #0x00000001

    //
    // Store the NMI stack frame onto the application's stack.
    //
    stmdb   sp!, {r4-r11}

    //
    // Branch to the application's NMI handler.
    //
    ldr     r0, [r0, #0x08]
    bx      r0
#else
    //
    // Loop forever since there is nothing that we can do about a NMI.
    //
    b       .
#endif

//*****************************************************************************
//
// The hard fault handler.
//
//*****************************************************************************
    .thumb_func
FaultISR:
    //
    // Loop forever since there is nothing that we can do about a hard fault.
    //
    b       .

//*****************************************************************************
//
// The default interrupt handler.
//
//*****************************************************************************
    .thumb_func
IntDefaultHandler:
    //
    // Loop forever since there is nothing that we can do about an unexpected
    // interrupt.
    //
    b       .

//*****************************************************************************
//
// Provides a small delay.  The loop below takes 3 cycles/loop.
//
//*****************************************************************************
    .globl  Delay
    .thumb_func
Delay:
    subs    r0, #1
    bne     Delay
    bx      lr

//*****************************************************************************
//
// This is the end of the file.
//
//*****************************************************************************
    .end
