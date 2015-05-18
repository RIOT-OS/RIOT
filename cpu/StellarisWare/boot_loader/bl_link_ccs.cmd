/******************************************************************************
 *
 * bl_link_ccs.cmd - CCS linker configuration file for boot loader.
 *
 * Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
 * Software License Agreement
 * 
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI's microcontroller products. The software is owned by
 * TI and/or its suppliers, and is protected under applicable copyright
 * laws. You may not combine this software with "viral" open-source
 * software in order to form a larger program.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 * 
 * This is part of revision 9453 of the Stellaris Firmware Development Package.
 *
 *****************************************************************************/

--retain=Vectors

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* System memory map */

MEMORY
{
    FLASH (RX) : origin = 0x00000000, length = 0x00010000
    SRAM (RWX) : origin = 0x20000000, length = 0x00010000
}

/* Section allocation in memory */

SECTIONS
{
    GROUP
    {
        .intvecs
        .text
        .const
        .data
    } load = FLASH, run = SRAM, LOAD_START(init_load), RUN_START(init_run), SIZE(init_size)

    GROUP
    {
        .bss
        .stack
    } run = SRAM, RUN_START(bss_run), RUN_END(bss_end), SIZE(bss_size), RUN_END(__STACK_TOP)

}
