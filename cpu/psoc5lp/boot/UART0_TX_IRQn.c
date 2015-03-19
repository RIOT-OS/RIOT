/*******************************************************************************
* File Name: UART0_TX_IRQn.c
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
#include <UART0_TX_IRQn.h>

#if !defined(UART0_TX_IRQn__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here
********************************************************************************/
/* `#START UART0_TX_IRQn_intc` */

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
* Function Name: UART0_TX_IRQn_Start
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
void UART0_TX_IRQn_Start(void)
{
    /* For all we know the interrupt is active. */
    UART0_TX_IRQn_Disable();

    /* Set the ISR to point to the UART0_TX_IRQn Interrupt. */
    UART0_TX_IRQn_SetVector(&UART0_TX_IRQn_Interrupt);

    /* Set the priority. */
    UART0_TX_IRQn_SetPriority((uint8)UART0_TX_IRQn_INTC_PRIOR_NUMBER);

    /* Enable it. */
    UART0_TX_IRQn_Enable();
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_StartEx
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
void UART0_TX_IRQn_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    UART0_TX_IRQn_Disable();

    /* Set the ISR to point to the UART0_TX_IRQn Interrupt. */
    UART0_TX_IRQn_SetVector(address);

    /* Set the priority. */
    UART0_TX_IRQn_SetPriority((uint8)UART0_TX_IRQn_INTC_PRIOR_NUMBER);

    /* Enable it. */
    UART0_TX_IRQn_Enable();
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_Stop
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
void UART0_TX_IRQn_Stop(void)
{
    /* Disable this interrupt. */
    UART0_TX_IRQn_Disable();

    /* Set the ISR to point to the passive one. */
    UART0_TX_IRQn_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for UART0_TX_IRQn.
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
CY_ISR(UART0_TX_IRQn_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START UART0_TX_IRQn_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling UART0_TX_IRQn_Start
*   will override any effect this method would have had. To set the vector
*   before the component has been started use UART0_TX_IRQn_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void UART0_TX_IRQn_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)UART0_TX_IRQn__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_GetVector
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
cyisraddress UART0_TX_IRQn_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)UART0_TX_IRQn__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling UART0_TX_IRQn_Start
*   or UART0_TX_IRQn_StartEx will override any effect this method
*   would have had. This method should only be called after
*   UART0_TX_IRQn_Start or UART0_TX_IRQn_StartEx has been called. To set
*   the initial priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 7, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void UART0_TX_IRQn_SetPriority(uint8 priority)
{
    *UART0_TX_IRQn_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_GetPriority
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
uint8 UART0_TX_IRQn_GetPriority(void)
{
    uint8 priority;


    priority = *UART0_TX_IRQn_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_Enable
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
void UART0_TX_IRQn_Enable(void)
{
    /* Enable the general interrupt. */
    *UART0_TX_IRQn_INTC_SET_EN = UART0_TX_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_GetState
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
uint8 UART0_TX_IRQn_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*UART0_TX_IRQn_INTC_SET_EN & (uint32)UART0_TX_IRQn__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_Disable
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
void UART0_TX_IRQn_Disable(void)
{
    /* Disable the general interrupt. */
    *UART0_TX_IRQn_INTC_CLR_EN = UART0_TX_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_SetPending
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
void UART0_TX_IRQn_SetPending(void)
{
    *UART0_TX_IRQn_INTC_SET_PD = UART0_TX_IRQn__INTC_MASK;
}


/*******************************************************************************
* Function Name: UART0_TX_IRQn_ClearPending
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
void UART0_TX_IRQn_ClearPending(void)
{
    *UART0_TX_IRQn_INTC_CLR_PD = UART0_TX_IRQn__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
