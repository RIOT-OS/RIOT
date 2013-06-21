#include "cpu.h"
#include "bits.h"
#include "VIC.h"

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale)
{
    *prescale = source / PCLK_DIV / target;
}

/******************************************************************************
** Function name:		install_irq
**
** Descriptions:		Install interrupt handler
** parameters:			Interrupt number, interrupt handler address,
**						interrupt priority
** Returned value:		true or false, return false if IntNum is out of range
**
******************************************************************************/
#define VIC_BASE_ADDR   0xFFFFF000

bool cpu_install_irq(int IntNumber, void *HandlerAddr, int Priority)
{
    int *vect_addr;
    int *vect_cntl;

    VICIntEnClear = 1 << IntNumber;	/* Disable Interrupt */

    if(IntNumber >= VIC_SIZE) {
        return (false);
    }
    else {
        /* find first un-assigned VIC address for the handler */
        vect_addr = (int *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + Priority * 4);
        vect_cntl = (int *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + Priority * 4);

        *vect_addr = (int)HandlerAddr;	/* set interrupt vector */
        *vect_cntl = IntNumber + BIT5;
        VICIntEnable = 1 << IntNumber;	/* Enable Interrupt */
        return(true);
    }
}
