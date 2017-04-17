/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  Copyright (C) 2017                                                         */
/*                                                                             */
/*  Module      : cpu_s5p4418                                                  */
/*  File        : s5p4418.ld                                                   */
/*  Description : s5p4418 linker script                                        */
/*  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)                      */
/*  History     :                                                              */
/*-----------------------------------------------------------------------------*/

STACK_FIQ_SIZE = 0x1000;
STACK_IRQ_SIZE = 0x1000;
STACK_ABT_SIZE = 0x1000;
STACK_UND_SIZE = 0x1000;
STACK_SRV_SIZE = 0x1000;
STACK_SYS_SIZE = 0x1000;

HEAP_SIZE      = 0x1000;

/* Memory Spaces Definitions */
MEMORY
{
    ROM (rx)  : ORIGIN = 0x48000000, LENGTH = 0x02000000    /* 32 MB */
    RAM (rwx) : ORIGIN = 0x4A000000, LENGTH = 0x02000000    /* 32 MB */
}
