/* ****************************************************************************************************** */
/*   demo2148_blink_flash.cmd				LINKER  SCRIPT                                                */
/*                                                                                                        */
/*                                                                                                        */
/*   The Linker Script defines how the code and data emitted by the GNU C compiler and assembler are  	  */
/*   to be loaded into memory (code goes into FLASH, variables go into RAM).                 			  */
/*                                                                                                        */
/*   Any symbols defined in the Linker Script are automatically global and available to the rest of the   */
/*   program.                                                                                             */
/*                                                                                                        */
/*   To force the linker to use this LINKER SCRIPT, just add the -T demo2148_blink_flash.cmd directive    */
/*   to the linker flags in the makefile.                                                                 */
/*                                                                                                        */
/*   			LFLAGS  =  -Map main.map -nostartfiles -T demo2148_blink_flash.cmd                        */
/*                                                                                                        */
/*                                                                                                        */
/*   The Philips boot loader supports the ISP (In System Programming) via the serial port and the IAP     */
/*   (In Application Programming) for flash programming from within your application.                     */
/*                                                                                                        */
/*   The boot loader uses RAM memory and we MUST NOT load variables or code in these areas.               */
/*                                                                                                        */
/*   RAM used by boot loader:  0x40000120 - 0x400001FF  (223 bytes) for ISP variables                     */
/*                             0x40007FE0 - 0x4000FFFF  (32 bytes)  for ISP and IAP variables             */
/*                             0x40007EE0 - 0x40007FE0  (256 bytes) stack for ISP and IAP                 */
/*                                                                                                        */
/*                                                                                                        */
/*                              MEMORY MAP                                                                */
/*                      |                                 |0x40008000                                     */
/*            .-------->|---------------------------------|                                               */
/*            .         |     variables and stack         |0x40007FFF                                     */
/*         ram_isp_high |     for Philips boot loader     |                                               */
/*            .         |     32 + 256 = 288 bytes        |                                               */
/*            .         |                                 |                                     		  */
/*            .         |   Do not put anything here      |0x40007EE0                                     */
/*            .-------->|---------------------------------|                                               */
/*                      |    UDF Stack  4 bytes           |0x40007EDC  <---------- _stack_end             */
/*            .-------->|---------------------------------|                                               */
/*                      |    ABT Stack  4 bytes           |0x40007ED8                                     */
/*            .-------->|---------------------------------|                                               */
/*                      |    FIQ Stack  4 bytes           |0x40007ED4                                     */
/*            .-------->|---------------------------------|                                               */
/*                      |    IRQ Stack  4 bytes           |0x40007ED0                                     */
/*            .-------->|---------------------------------|                                               */
/*                      |    SVC Stack  4 bytes           |0x40007ECC                                     */
/*            .-------->|---------------------------------|                                               */
/*            .         |                                 |0x40007EC8 			                          */
/*            .         |     stack area for user program |                                               */
/*            .         |               |                 |                                               */
/*            .         |               |                 |                                               */
/*            .         |               |                 |                                               */
/*            .         |               V                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |          free ram               |                                               */
/*           ram        |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |                                 |                                               */
/*            .         |.................................|0x40000234 <---------- _bss_end                */
/*            .         |                                 |                                               */
/*            .         |  .bss   uninitialized variables |                                               */
/*            .         |.................................|0x40000218 <---------- _bss_start, _edata      */
/*            .         |                                 |                                               */
/*            .         |  .data  initialized variables   |                                               */
/*            .         |                                 |0x40000200 <---------- _data                   */
/*            .-------->|---------------------------------|                                               */
/*            .         |     variables used by           |0x400001FF                                     */
/*         ram_isp_low  |     Philips boot loader         |                                               */
/*            .         |           223 bytes             |0x40000120                                     */
/*            .-------->|---------------------------------|                                               */
/*            .         |                                 |0x4000011F                                     */
/*         ram_vectors  |          free ram               |                                               */
/*            .         |---------------------------------|0x40000040                                     */
/*            .         |                                 |0x4000003F                                     */
/*            .         |  Interrupt Vectors (re-mapped)  |                                               */
/*            .         |          64 bytes               |0x40000000                                     */
/*            .-------->|---------------------------------|                                               */
/*                      |                                 |                                               */
/*                                                                                                        */
/*                                                                                                        */
/*                                                                                                        */
/*                      |                                 |                                               */
/*           .--------> |---------------------------------|                                               */
/*           .          |                                 |0x0001FFFF                                     */
/*           .          |                                 |                                               */
/*           .          |                                 |                                               */
/*           .          |                                 |                                               */
/*           .          |                                 |                                               */
/*           .          |                                 |                                               */
/*           .          |       unused flash eprom        |                                               */
/*           .          |                                 |                                               */
/*           .          |.................................|0x0000032c                                     */
/*           .          |                                 |                                               */
/*           .          |      copy of .data area         |                                               */
/*         flash        |                                 |                                               */
/*           .          |---------------------------------|0x00000314 <----------- _etext                 */
/*           .          |                                 |                                               */
/*           .          |                                 |0x00000180  main                               */
/*           .          |                                 |0x00000278  feed                               */
/*           .          |         main()                  |0x000002c4  FIQ_Routine                        */
/*           .          |                                 |0x000002d8  SWI_Routine                        */
/*           .          |                                 |0x000002ec  UNDEF_Routine	                  */
/*           .          |                                 |0x000002b0  IRQ_routine                        */
/*           .          |---------------------------------|0x000001cc  initialize                         */
/*           .          |                                 |0x000000D4                                     */
/*           .          |         Startup Code            |                                               */
/*           .          |         (assembler)             |                                               */
/*           .          |                                 |                                               */
/*           .          |---------------------------------|0x00000040 Reset_Handler                       */
/*           .          |                                 |0x0000003F                                     */
/*           .          | Interrupt Vector Table (unused) |                                               */
/*           .          |          64 bytes               |                                               */
/*           .--------->|---------------------------------|0x00000000 _startup                            *
/*                                                                                                        */
/*                                                                                                        */
/*    The easy way to prevent the linker from loading anything into a memory area is to define            */
/*    a MEMORY region for it and then avoid assigning any .text, .data or .bss sections into it.          */
/*                                                                                                        */
/*                                                                                                        */
/*             MEMORY                                                                                     */
/*             {                                                                                          */
/*                ram_isp_low(A)  : ORIGIN = 0x40000120, LENGTH = 223                                     */
/*                                                                                                        */
/*             }                                                                                          */
/*                                                                                                        */
/*                                                                                                        */
/*  Author:  James P. Lynch                                                                               */
/*                                                                                                        */
/* ****************************************************************************************************** */


/* identify the Entry Point  */

ENTRY(_startup)
/* STARTUP(bin/startup.o)*/


/* specify the LPC2148 memory areas  */

MEMORY 
{
	flash     			: ORIGIN = 0,          LENGTH = 512K	/* FLASH ROM                            	*/	
	ram_isp_low(A)		: ORIGIN = 0x40000120, LENGTH = 223		/* variables used by Philips ISP bootloader	*/		 
	ram   				: ORIGIN = 0x40000200, LENGTH = 32513   /* free RAM area							*/
	ram_isp_high(A)		: ORIGIN = 0x40007FE0, LENGTH = 32		/* variables used by Philips ISP bootloader	*/
	ram_usb_dma			: ORIGIN = 0x7FD00000, LENGTH = 8192    /* on-chip USB DMA RAM area (not used)      */
}



/* define a global symbol _stack_end  */

_stack_end = 0x40007EDC;



/* now define the output sections  */

SECTIONS 
{
	. = 0;								/* set location counter to address zero  */
 
	.text :								/* collect all sections that should go into FLASH after startup  */ 
	{
		*(.vectors)						/* Exception Vectors and branch table >= 64 bytes */
		. = ALIGN(64);
	    *(.init)
		*(.text)						/* all .text sections (code)  */
		*(.rodata)						/* all .rodata sections (constants, strings, etc.)  */
		*(.rodata*)						/* all .rodata* sections (constants, strings, etc.)  */
		*(.glue_7)						/* all .glue_7 sections  (no idea what these are) */
		*(.glue_7t)						/* all .glue_7t sections (no idea what these are) */
		_etext = .;						/* define a global symbol _etext just after the last code byte */
	} >flash							/* put all the above into FLASH */
	

	

	.data :								/* collect all initialized .data sections that go into RAM  */ 
	{
		_data = .;						/* create a global symbol marking the start of the .data section  */
		*(.data)						/* all .data sections  */
		_edata = .;						/* define a global symbol marking the end of the .data section  */
	} >ram AT >flash					/* put all the above into RAM (but load the LMA copy into FLASH) */

	.bss :								/* collect all uninitialized .bss sections that go into RAM  */
	{
		_bss_start = .;					/* define a global symbol marking the start of the .bss section */
		*(.bss)							/* all .bss sections  */
	} >ram								/* put all the above in RAM (it will be cleared in the startup code */

	. = ALIGN(4);						/* advance location counter to the next 32-bit boundary */
	_bss_end = . ;						/* define a global symbol marking the end of the .bss section */
}
	_end = .;							/* define a global symbol marking the end of application RAM */
	PROVIDE(end = .); 
	
