#include "cpu.h"
#include "irq.h"

unsigned disableIRQ(void)
{
	return(irq_arch_disable());
}

unsigned enableIRQ(void)
{
	return(irq_arch_enable());
}

void restoreIRQ(unsigned state)
{
	irq_arch_restore(state);
}

int inISR(void)
{
	return(irq_arch_in());
}

