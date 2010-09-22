/* ***************************************************************************************************************

	crt.s						STARTUP  ASSEMBLY  CODE 
								-----------------------


	Module includes the interrupt vectors and start-up code.

  *************************************************************************************************************** */

/* Stack Sizes */
.set  UND_STACK_SIZE, 0x00000040		/* stack for "undefined instruction" interrupts				*/
.set  ABT_STACK_SIZE, 0x00000040		/* stack for "abort" interrupts								*/
.set  FIQ_STACK_SIZE, 0x00000040		/* stack for "FIQ" interrupts								*/
.set  IRQ_STACK_SIZE, 0X00000100		/* stack for "IRQ" normal interrupts						*/
.set  SVC_STACK_SIZE, 0x00000400		/* stack for "SVC" supervisor mode							*/



/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers) */
.set  MODE_USR, 0x10            		/* Normal User Mode 										*/
.set  MODE_FIQ, 0x11            		/* FIQ Processing Fast Interrupts Mode 						*/
.set  MODE_IRQ, 0x12            		/* IRQ Processing Standard Interrupts Mode 					*/
.set  MODE_SVC, 0x13            		/* Supervisor Processing Software Interrupts Mode 			*/
.set  MODE_ABT, 0x17            		/* Abort Processing memory Faults Mode 						*/
.set  MODE_UND, 0x1B            		/* Undefined Processing Undefined Instructions Mode 		*/
.set  MODE_SYS, 0x1F            		/* System Running Priviledged Operating System Tasks  Mode	*/

.set  I_BIT, 0x80               		/* when I bit is set, IRQ is disabled (program status registers) */
.set  F_BIT, 0x40               		/* when F bit is set, FIQ is disabled (program status registers) */


.text
.arm

.global	Reset_Handler
.global _startup
.func   _startup

_startup:

# Exception Vectors

_vectors:       ldr     PC, Reset_Addr         
                ldr     PC, Undef_Addr
                ldr     PC, SWI_Addr
                ldr     PC, PAbt_Addr
                ldr     PC, DAbt_Addr
                nop							/* Reserved Vector (holds Philips ISP checksum) */
                ldr     PC, IRQ_Addr    	/* see page 71 of "Insiders Guide to the Philips ARM7-Based Microcontrollers" by Trevor Martin  */
                ldr     PC, FIQ_Addr

Reset_Addr:     .word   Reset_Handler		/* defined in this module below  */
Undef_Addr:     .word   UNDEF_Routine		/* defined in main.c  */
SWI_Addr:       .word   ctx_switch 			/* defined in main.c  */
PAbt_Addr:      .word   UNDEF_Routine		/* defined in main.c  */
DAbt_Addr:      .word   UNDEF_Routine		/* defined in main.c  */
IRQ_Addr:       .word   fk_cpu_irq_isr  	/* defined in main.c  */
FIQ_Addr:       .word   FIQ_Routine			/* defined in main.c  */
                .word   0					/* rounds the vectors and ISR addresses to 64 bytes total  */


# Reset Handler

Reset_Handler:  

				/* Setup a stack for each mode - note that this only sets up a usable stack
				for User mode.   Also each mode is setup with interrupts initially disabled. */
    			  
    			ldr   r0, =_stack_end
    			msr   CPSR_c, #MODE_UND|I_BIT|F_BIT 	/* Undefined Instruction Mode  */
    			mov   sp, r0
    			sub   r0, r0, #UND_STACK_SIZE
    			msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT 	/* Abort Mode */
    			mov   sp, r0
    			sub   r0, r0, #ABT_STACK_SIZE
    			msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT 	/* FIQ Mode */
    			mov   sp, r0	
   				sub   r0, r0, #FIQ_STACK_SIZE
    			msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT 	/* IRQ Mode */
    			mov   sp, r0
    			sub   r0, r0, #IRQ_STACK_SIZE
    			msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT 	/* Supervisor Mode */
    			mov   sp, r0
    			sub   r0, r0, #SVC_STACK_SIZE
    			msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT 	/* User Mode */
    			mov   sp, r0

				/* copy .data section (Copy from ROM to RAM) */
                ldr     R1, =_etext
                ldr     R2, =_data
                ldr     R3, =_edata
1:        		cmp     R2, R3
                ldrlo   R0, [R1], #4
                strlo   R0, [R2], #4
                blo     1b

				/* Clear .bss section (Zero init)  */
                mov     R0, #0
                ldr     R1, =_bss_start
                ldr     R2, =_bss_end
2:				cmp     R1, R2
                strlo   R0, [R1], #4
                blo     2b

				/* Enter the C code  */
                bl       bootloader
                b        kernel_init


.endfunc
.end
