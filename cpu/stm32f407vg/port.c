#include "cpu.h"
#include "core_cmFunc.h"

#define PRIV_USER_MODE  	0x2
#define PRIV_THREAD_MODE    0x0

extern void fk_task_exit(void);

unsigned int atomic_set_return(unsigned int* p, unsigned int uiVal) {
	//unsigned int cspr = disableIRQ();		//crashes
	dINT();
	unsigned int uiOldVal = *p;
	*p = uiVal;
	//restoreIRQ(cspr);						//crashes
	eINT();
	return uiOldVal;
}

void cpu_switch_context_exit(void){
    sched_run();
    sched_task_return();
}


void thread_yield(void) {
	//asm("svc 0x01\n");
	// set PendSV Bit
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

__attribute__((naked))void PendSV_Handler(void)
{
	save_context();
	/* call scheduler update fk_thread variable with pdc of next thread	 */
	asm("bl sched_run");
	/* the thread that has higest priority and is in PENDING state */
	restore_context();
}


// /* kernel functions */
//void ctx_switch(void)
//{
//	/* Save return address on stack */
//	/* stmfd   sp!, {lr} */
//
//	/* disable interrupts */
//	/* mov     lr, #NOINT|SVCMODE */
//	/* msr     CPSR_c, lr */
//	/* cpsid 	i */
//
//	ctx_switch2:
//	/* save other register */
//	asm("nop");
//
//	asm("mov r12, sp");
//	asm("stmfd r12!, {r4-r11}");
//
//	/* save user mode stack pointer in *fk_thread */
//	asm("ldr     r1, =active_thread"); /* r1 = &fk_thread */
//	asm("ldr     r1, [r1]"); /* r1 = *r1 = fk_thread */
//	asm("str     r12, [r1]"); /* store stack pointer in tasks pdc*/
//
//	sched_task_return();
//}

/* call scheduler so fk_thread points to the next task */
void sched_task_return(void)
{
	/* switch to user mode use PSP insteat of MSP in ISR Mode*/

//	unsigned int mode = PRIV_USER_MODE;
//
//	asm("msr CONTROL,%0 ": : "r" (mode):);
	CONTROL_Type mode;
	mode.w = __get_CONTROL();

	mode.b.SPSEL = 1; // select PSP
	mode.b.nPRIV = 0; // privilege

	__set_CONTROL(mode.w);

	/* load pdc->stackpointer in r0 */
	asm("ldr     r0, =active_thread"); /* r0 = &fk_thread */
	asm("ldr     r0, [r0]"); /* r0 = *r0 = fk_thread */
	asm("ldr     sp, [r0]"); /* sp = r0  restore stack pointer*/
	asm("pop		{r4}"); /* skip exception return */
	asm(" pop		{r4-r11}");
	asm(" pop		{r0-r3,r12,lr}"); /* simulate register restor from stack */
//	asm("pop 		{r4}"); /*foo*/
	asm("pop		{pc}");
}
/*
 * cortex m4 knows stacks and handles register backups
 *
 * so use different stack frame layout
 *
 *
 * with float storage
 * ------------------------------------------------------------------------------------------------------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR | S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S13 | S14 | S15 | FPSCR |
 * ------------------------------------------------------------------------------------------------------------------------------------
 *
 * without
 *
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 *
 */
char *thread_stack_init(void (*task_func)(void), void *stack_start, int stack_size)
{
	unsigned int * stk;
	stk = (unsigned int *)(stack_start + stack_size);

	/* marker */
	stk--;
	*stk = 0x77777777;

	//FIXME FPSCR
	stk--;
	*stk = (unsigned int) 0;

	//S0 - S15
	for (int i = 15; i >= 0; i--) {
		stk--;
		*stk = i;
	}

	//FIXME xPSR
	stk--;
	*stk = (unsigned int) 0x01000200;

	//program counter
	stk--;
	*stk = (unsigned int) task_func;

	/* link register */
	stk--;
	*stk = (unsigned int) 0x0;

	/* r12 */
	stk--;
	*stk = (unsigned int) 0;

	/* r0 - r3 */
	for (int i = 3; i >= 0; i--) {
		stk--;
		*stk = i;
	}

	/* r11 - r4 */
	for (int i = 11; i >= 4; i--) {
		stk--;
		*stk = i;
	}

	/* lr means exception return code  */
	stk--;
	*stk = (unsigned int) 0xfffffffd; // return to taskmode main stack pointer

	return (char*) stk;
}
