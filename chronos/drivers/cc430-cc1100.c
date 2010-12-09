#include <stdint.h>

#include <cpu.h>
#include <irq.h>
#include <cc1100_ng.h>
#include <arch_cc1100.h>

#include <cc430_.h>
#include <msp430/rf1a.h>

#define CC1100_GDO0     IOCFG0
#define CC1100_GDO1     IOCFG1
#define CC1100_GDO2     IOCFG2

int cc1100_get_gdo0(void) {
	return 	CC1100_GDO0;
}

int cc1100_get_gdo1(void) {
	return 	CC1100_GDO1;
}

int cc1100_get_gdo2(void) {
	return 	CC1100_GDO2;
}

void cc1100_before_send(void)
{
	// Disable GDO2 interrupt before sending packet
	cc1100_gdo2_disable();
}

void cc1100_after_send(void)
{
	// Enable GDO2 interrupt after sending packet
	cc1100_gdo2_enable();
}

void cc1100_gdo0_enable(void) {
    RF1AIFG &= ~RF1AIV_RFIFG0;
    RF1AIE  |= RF1AIV_RFIFG0;
}

void cc1100_gdo0_disable(void) {
    RF1AIE  &= ~RF1AIV_RFIFG0;
    RF1AIFG &= ~RF1AIV_RFIFG0;
}

void cc1100_gdo2_disable(void) {
    RF1AIFG &= ~RF1AIV_RFIFG2;
    RF1AIE  &= ~RF1AIV_RFIFG2;
}

void cc1100_gdo2_enable(void) {
    RF1AIE  &= ~RF1AIV_RFIFG2;
    RF1AIFG |= RF1AIV_RFIFG2;
}

void cc1100_init_interrupts(void) {
	uint8_t state = disableIRQ(); /* Disable all interrupts */
	restoreIRQ(state);  /* Enable all interrupts */
}

interrupt (CC1101_VECTOR) __attribute__ ((naked)) cc1100_isr(void){
    __enter_isr();
 	/* Check IFG */
	if (RF1AIFG & RF1AIV_RFIFG2) {
        RF1AIFG &= ~RF1AIV_RFIFG2;
		cc1100_gdo2_irq();
    }
	if (RF1AIFG & RF1AIV_RFIFG0) {
        RF1AIFG &= ~RF1AIV_RFIFG0;
        RF1AIE  &= ~RF1AIV_RFIFG0;
        cc1100_gdo0_irq();
	}
	__exit_isr();
}
