/*******************************************************************************
* File Name: TIMER0.c
* Version 2.40
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "TIMER0.h"

uint8 TIMER0_initVar = 0u;


/*******************************************************************************
* Function Name: TIMER0_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_Init(void)
{
        #if (!TIMER0_UsingFixedFunction && !TIMER0_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!TIMER0_UsingFixedFunction && !TIMER0_ControlRegRemoved) */

        #if(!TIMER0_UsingFixedFunction)
            /* Interrupt State Backup for Critical Region*/
            uint8 TIMER0_interruptState;
        #endif /* (!TIMER0_UsingFixedFunction) */

        #if (TIMER0_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            TIMER0_CONTROL &= TIMER0_CTRL_ENABLE;

            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                TIMER0_CONTROL2 &= ((uint8)(~TIMER0_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                TIMER0_CONTROL3 &= ((uint8)(~TIMER0_CTRL_MODE_MASK));
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (TIMER0_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                TIMER0_CONTROL |= TIMER0_ONESHOT;
            #endif /* (TIMER0_RunModeUsed != 0x0u) */

            /* Set the IRQ to use the status register interrupts */
            TIMER0_CONTROL2 |= TIMER0_CTRL2_IRQ_SEL;

            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            TIMER0_RT1 &= ((uint8)(~TIMER0_RT1_MASK));
            TIMER0_RT1 |= TIMER0_SYNC;

            /*Enable DSI Sync all all inputs of the Timer*/
            TIMER0_RT1 &= ((uint8)(~TIMER0_SYNCDSI_MASK));
            TIMER0_RT1 |= TIMER0_SYNCDSI_EN;

        #else
            #if(!TIMER0_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = TIMER0_CONTROL & ((uint8)(~TIMER0_CTRL_CMPMODE_MASK));
            TIMER0_CONTROL = ctrl | TIMER0_DEFAULT_COMPARE_MODE;

            /* Set the default capture mode defined in the parameter */
            ctrl = TIMER0_CONTROL & ((uint8)(~TIMER0_CTRL_CAPMODE_MASK));

            #if( 0 != TIMER0_CAPTURE_MODE_CONF)
                TIMER0_CONTROL = ctrl | TIMER0_DEFAULT_CAPTURE_MODE;
            #else
                TIMER0_CONTROL = ctrl;
            #endif /* 0 != TIMER0_CAPTURE_MODE */

            #endif /* (!TIMER0_ControlRegRemoved) */
        #endif /* (TIMER0_UsingFixedFunction) */

        /* Clear all data in the FIFO's */
        #if (!TIMER0_UsingFixedFunction)
            TIMER0_ClearFIFO();
        #endif /* (!TIMER0_UsingFixedFunction) */

        /* Set Initial values from Configuration */
        TIMER0_WritePeriod(TIMER0_INIT_PERIOD_VALUE);
        #if (!(TIMER0_UsingFixedFunction && (CY_PSOC5A)))
            TIMER0_WriteCounter(TIMER0_INIT_COUNTER_VALUE);
        #endif /* (!(TIMER0_UsingFixedFunction && (CY_PSOC5A))) */
        TIMER0_SetInterruptMode(TIMER0_INIT_INTERRUPTS_MASK);

        #if (!TIMER0_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)TIMER0_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            TIMER0_WriteCompare(TIMER0_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            TIMER0_interruptState = CyEnterCriticalSection();

            TIMER0_STATUS_AUX_CTRL |= TIMER0_STATUS_ACTL_INT_EN_MASK;

            /* Exit Critical Region*/
            CyExitCriticalSection(TIMER0_interruptState);

        #endif /* (!TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_Enable
********************************************************************************
* Summary:
*     Enable the Counter
*
* Parameters:
*  void
*
* Return:
*  void
*
* Side Effects:
*   If the Enable mode is set to Hardware only then this function has no effect
*   on the operation of the counter.
*
*******************************************************************************/
void TIMER0_Enable(void)
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (TIMER0_UsingFixedFunction)
        TIMER0_GLOBAL_ENABLE |= TIMER0_BLOCK_EN_MASK;
        TIMER0_GLOBAL_STBY_ENABLE |= TIMER0_BLOCK_STBY_EN_MASK;
    #endif /* (TIMER0_UsingFixedFunction) */

    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!TIMER0_ControlRegRemoved || TIMER0_UsingFixedFunction)
        TIMER0_CONTROL |= TIMER0_CTRL_ENABLE;
    #endif /* (!TIMER0_ControlRegRemoved || TIMER0_UsingFixedFunction) */

}


/*******************************************************************************
* Function Name: TIMER0_Start
********************************************************************************
* Summary:
*  Enables the counter for operation
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TIMER0_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void TIMER0_Start(void)
{
    if(TIMER0_initVar == 0u)
    {
        TIMER0_Init();

        TIMER0_initVar = 1u; /* Clear this bit for Initialization */
    }

    /* Enable the Counter */
    TIMER0_Enable();
}


/*******************************************************************************
* Function Name: TIMER0_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void TIMER0_Stop(void)
{
    /* Disable Counter */
    #if(!TIMER0_ControlRegRemoved || TIMER0_UsingFixedFunction)
        TIMER0_CONTROL &= ((uint8)(~TIMER0_CTRL_ENABLE));
    #endif /* (!TIMER0_ControlRegRemoved || TIMER0_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (TIMER0_UsingFixedFunction)
        TIMER0_GLOBAL_ENABLE &= ((uint8)(~TIMER0_BLOCK_EN_MASK));
        TIMER0_GLOBAL_STBY_ENABLE &= ((uint8)(~TIMER0_BLOCK_STBY_EN_MASK));
    #endif /* (TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_SetInterruptMode(uint8 interruptsMask)
{
    TIMER0_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: TIMER0_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:
*  void
*
* Return:
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read.
*
*******************************************************************************/
uint8   TIMER0_ReadStatusRegister(void)
{
    return TIMER0_STATUS;
}


#if(!TIMER0_ControlRegRemoved)
/*******************************************************************************
* Function Name: TIMER0_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:
*  void
*
* Return:
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   TIMER0_ReadControlRegister(void)
{
    return TIMER0_CONTROL;
}


/*******************************************************************************
* Function Name: TIMER0_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:
*  void
*
* Return:
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    TIMER0_WriteControlRegister(uint8 control)
{
    TIMER0_CONTROL = control;
}

#endif  /* (!TIMER0_ControlRegRemoved) */


#if (!(TIMER0_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: TIMER0_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:
*  counter:  New counter value.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_WriteCounter(uint32 counter) \

{
    #if(TIMER0_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (TIMER0_GLOBAL_ENABLE & TIMER0_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        TIMER0_GLOBAL_ENABLE |= TIMER0_BLOCK_EN_MASK;
        CY_SET_REG16(TIMER0_COUNTER_LSB_PTR, (uint16)counter);
        TIMER0_GLOBAL_ENABLE &= ((uint8)(~TIMER0_BLOCK_EN_MASK));
    #else
        CY_SET_REG32(TIMER0_COUNTER_LSB_PTR, counter);
    #endif /* (TIMER0_UsingFixedFunction) */
}
#endif /* (!(TIMER0_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: TIMER0_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:
*  void:
*
* Return:
*  (uint32) The present value of the counter.
*
*******************************************************************************/
uint32 TIMER0_ReadCounter(void)
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(TIMER0_UsingFixedFunction)
		(void)CY_GET_REG16(TIMER0_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(TIMER0_COUNTER_LSB_PTR_8BIT);
	#endif/* (TIMER0_UsingFixedFunction) */

    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(TIMER0_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(TIMER0_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG32(TIMER0_STATICCOUNT_LSB_PTR));
    #endif /* (TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:
*  void
*
* Return:
*  (uint32) Present Capture value.
*
*******************************************************************************/
uint32 TIMER0_ReadCapture(void)
{
    #if(TIMER0_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(TIMER0_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG32(TIMER0_STATICCOUNT_LSB_PTR));
    #endif /* (TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period
* will be loaded the next time terminal count is detected.
*
* Parameters:
*  period: (uint32) A value of 0 will result in
*         the counter remaining at zero.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_WritePeriod(uint32 period)
{
    #if(TIMER0_UsingFixedFunction)
        CY_SET_REG16(TIMER0_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG32(TIMER0_PERIOD_LSB_PTR, period);
    #endif /* (TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:
*  void:
*
* Return:
*  (uint32) Present period value.
*
*******************************************************************************/
uint32 TIMER0_ReadPeriod(void)
{
    #if(TIMER0_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(TIMER0_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG32(TIMER0_PERIOD_LSB_PTR));
    #endif /* (TIMER0_UsingFixedFunction) */
}


#if (!TIMER0_UsingFixedFunction)
/*******************************************************************************
* Function Name: TIMER0_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will
* reflect the new value on the next UDB clock.  The compare output will be
* driven high when the present counter value compares true based on the
* configured compare mode setting.
*
* Parameters:
*  Compare:  New compare value.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_WriteCompare(uint32 compare) \

{
    #if(TIMER0_UsingFixedFunction)
        CY_SET_REG16(TIMER0_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG32(TIMER0_COMPARE_LSB_PTR, compare);
    #endif /* (TIMER0_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: TIMER0_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:
*  void:
*
* Return:
*  (uint32) Present compare value.
*
*******************************************************************************/
uint32 TIMER0_ReadCompare(void)
{
    return (CY_GET_REG32(TIMER0_COMPARE_LSB_PTR));
}


#if (TIMER0_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: TIMER0_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_SetCompareMode(uint8 compareMode)
{
    /* Clear the compare mode bits in the control register */
    TIMER0_CONTROL &= ((uint8)(~TIMER0_CTRL_CMPMODE_MASK));

    /* Write the new setting */
    TIMER0_CONTROL |= compareMode;
}
#endif  /* (TIMER0_COMPARE_MODE_SOFTWARE) */


#if (TIMER0_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: TIMER0_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void TIMER0_SetCaptureMode(uint8 captureMode)
{
    /* Clear the capture mode bits in the control register */
    TIMER0_CONTROL &= ((uint8)(~TIMER0_CTRL_CAPMODE_MASK));

    /* Write the new setting */
    TIMER0_CONTROL |= ((uint8)((uint8)captureMode << TIMER0_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (TIMER0_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: TIMER0_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:
*  void:
*
* Return:
*  None
*
*******************************************************************************/
void TIMER0_ClearFIFO(void)
{

    while(0u != (TIMER0_ReadStatusRegister() & TIMER0_STATUS_FIFONEMP))
    {
        (void)TIMER0_ReadCapture();
    }

}
#endif  /* (!TIMER0_UsingFixedFunction) */


/* [] END OF FILE */

