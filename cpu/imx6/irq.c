#include <stdbool.h>
#include "gic.h"

/*
 * TODO from sdk/core/interrupt.h
//! @brief Disable an interrupt on the specified CPU.
//!
//! @param irq_id The interrupt number to disabled.
//! @param cpu_id The index of the CPU for which the interrupt will be disabled.
void disable_interrupt(uint32_t irq_id, uint32_t cpu_id);

//! @brief Set the interrupt service routine for the specified interrupt.
//!
//! @param irq_id The interrupt number.
//! @param isr Function that will be called to handle the interrupt.
void register_interrupt_routine(uint32_t irq_id, irq_hdlr_t isr);
 * TODO this means
 * maybe gic_enable() is the right way?
 * I'll just leave it here
 */

unsigned lastState = 1;

unsigned enableIRQ(void)
{
    unsigned result = lastState;
    gic_enable(true);
    lastState = true;
    return result;
}

unsigned disableIRQ(void)
{
    unsigned result = lastState;
    gic_enable(false);
    lastState = false;
    return result;
}

void restoreIRQ(unsigned irqstate)
{
    gic_enable(irqstate);
    lastState = irqstate;
}

int inISR(void)
{
    /*
     * TODO ok. To be honest, i can't even think of a concept for handling this...
     * Maybe there is a way to access the ICDISPR-register of the PL390?
     * But even that won't give a proper result with multiple cores.
     * Until then we can only say we are not in an interrupt if they are disabled globally
     */

    /*
     * this might work without having to modify the SDK:
    //! @brief GIC CPU interface registers.
    //!
    //! Uses the GICv2 register names. Does not include GICv2 registers.
    struct _gicc_registers
    {
    uint32_t CTLR;  //!< CPU Interface Control Register.
    uint32_t PMR;   //!< Interrupt Priority Mask Register.
    uint32_t BPR;   //!< Binary Point Register.
    uint32_t IAR;   //!< Interrupt Acknowledge Register.
    uint32_t EOIR;  //!< End of Interrupt Register.
    uint32_t RPR;   //!< Running Priority Register.
    uint32_t HPPIR; //!< Highest Priority Pending Interrupt Register.
    uint32_t ABPR;  //!< Aliased Binary Point Register. (only visible with a secure access)
    uint32_t _reserved[56];
    uint32_t IIDR;  //!< CPU Interface Identification Register.
    };
     * TODO maybe the "Running Priority Register" is useful somehow. (still : mulit-core?)
     */
    printf("WARNING: You are using an unsafe method! (inISR)\n");
    return (lastState == false);
}
