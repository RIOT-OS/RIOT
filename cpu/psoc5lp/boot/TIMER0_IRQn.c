/*******************************************************************************
* File Name: TIMER0_IRQn.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <TIMER0_IRQn.h>

#if !defined(TIMER0_IRQn__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START TIMER0_IRQn_intc` */

/* `#END` */

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: TIMER0_IRQn_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_Start(void)
{
    /* For all we know the interrupt is active. */
    TIMER0_IRQn_Disable();

    /* Set the ISR to point to the TIMER0_IRQn Interrupt. */
    TIMER0_IRQn_SetVector(&TIMER0_IRQn_Interrupt);

    /* Set the priority. */
    TIMER0_IRQn_SetPriority((uint8)TIMER0_IRQn_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TIMER0_IRQn_Enable();
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_StartEx
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    TIMER0_IRQn_Disable();

    /* Set the ISR to point to the TIMER0_IRQn Interrupt. */
    TIMER0_IRQn_SetVector(address);

    /* Set the priority. */
    TIMER0_IRQn_SetPriority((uint8)TIMER0_IRQn_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TIMER0_IRQn_Enable();
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_Stop(void)
{
    /* Disable this interrupt. */
    TIMER0_IRQn_Disable();

    /* Set the ISR to point to the passive one. */
    TIMER0_IRQn_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for TIMER0_IRQn.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(TIMER0_IRQn_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START TIMER0_IRQn_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling TIMER0_IRQn_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use TIMER0_IRQn_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)TIMER0_IRQn__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress TIMER0_IRQn_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)TIMER0_IRQn__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling TIMER0_IRQn_Start
*   or TIMER0_IRQn_StartEx will override any effect this method 
*   would have had. This method should only be called after 
*   TIMER0_IRQn_Start or TIMER0_IRQn_StartEx has been called. To set 
*   the initial priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 7, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_SetPriority(uint8 priority)
{
    *TIMER0_IRQn_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt. 0 - 7, 0 being the highest.
*
*******************************************************************************/
uint8 TIMER0_IRQn_GetPriority(void)
{
    uint8 priority;


    priority = *TIMER0_IRQn_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_Enable(void)
{
    /* Enable the general interrupt. */
    *TIMER0_IRQn_INTC_SET_EN = TIMER0_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 TIMER0_IRQn_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*TIMER0_IRQn_INTC_SET_EN & (uint32)TIMER0_IRQn__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_Disable(void)
{
    /* Disable the general interrupt. */
    *TIMER0_IRQn_INTC_CLR_EN = TIMER0_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_SetPending(void)
{
    *TIMER0_IRQn_INTC_SET_PD = TIMER0_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: TIMER0_IRQn_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TIMER0_IRQn_ClearPending(void)
{
    *TIMER0_IRQn_INTC_CLR_PD = TIMER0_IRQn__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
