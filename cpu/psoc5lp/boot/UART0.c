/*******************************************************************************
* File Name: UART0.c
* Version 2.30
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART0.h"
#include "CyLib.h"
#if(UART0_INTERNAL_CLOCK_USED)
    #include "UART0_IntClock.h"
#endif /* End UART0_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 UART0_initVar = 0u;
#if( UART0_TX_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
    volatile uint8 UART0_txBuffer[UART0_TXBUFFERSIZE];
    volatile uint8 UART0_txBufferRead = 0u;
    uint8 UART0_txBufferWrite = 0u;
#endif /* End UART0_TX_ENABLED */
#if( ( UART0_RX_ENABLED || UART0_HD_ENABLED ) && \
     (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) )
    volatile uint8 UART0_rxBuffer[UART0_RXBUFFERSIZE];
    volatile uint8 UART0_rxBufferRead = 0u;
    volatile uint8 UART0_rxBufferWrite = 0u;
    volatile uint8 UART0_rxBufferLoopDetect = 0u;
    volatile uint8 UART0_rxBufferOverflow = 0u;
    #if (UART0_RXHW_ADDRESS_ENABLED)
        volatile uint8 UART0_rxAddressMode = UART0_RXADDRESSMODE;
        volatile uint8 UART0_rxAddressDetected = 0u;
    #endif /* End EnableHWAddress */
#endif /* End UART0_RX_ENABLED */


/*******************************************************************************
* Function Name: UART0_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the UART component.
*  Enable the clock input to enable operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The UART0_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time UART_Start() is called. This allows for
*  component initialization without re-initialization in all subsequent calls
*  to the UART0_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART0_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(UART0_initVar == 0u)
    {
        UART0_Init();
        UART0_initVar = 1u;
    }
    UART0_Enable();
}


/*******************************************************************************
* Function Name: UART0_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  UART0_Start().
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void UART0_Init(void) 
{
    #if(UART0_RX_ENABLED || UART0_HD_ENABLED)

        #if(UART0_RX_INTERRUPT_ENABLED && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))
            /* Set the RX Interrupt. */
            (void)CyIntSetVector(UART0_RX_VECT_NUM, &UART0_RXISR);
            CyIntSetPriority(UART0_RX_VECT_NUM, UART0_RX_PRIOR_NUM);
        #endif /* End UART0_RX_INTERRUPT_ENABLED */

        #if (UART0_RXHW_ADDRESS_ENABLED)
            UART0_SetRxAddressMode(UART0_RXAddressMode);
            UART0_SetRxAddress1(UART0_RXHWADDRESS1);
            UART0_SetRxAddress2(UART0_RXHWADDRESS2);
        #endif /* End UART0_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        UART0_RXBITCTR_PERIOD_REG = UART0_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        UART0_RXSTATUS_MASK_REG  = UART0_INIT_RX_INTERRUPTS_MASK;
    #endif /* End UART0_RX_ENABLED || UART0_HD_ENABLED*/

    #if(UART0_TX_ENABLED)
        #if(UART0_TX_INTERRUPT_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
            /* Set the TX Interrupt. */
            (void)CyIntSetVector(UART0_TX_VECT_NUM, &UART0_TXISR);
            CyIntSetPriority(UART0_TX_VECT_NUM, UART0_TX_PRIOR_NUM);
        #endif /* End UART0_TX_INTERRUPT_ENABLED */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if(UART0_TXCLKGEN_DP)
            UART0_TXBITCLKGEN_CTR_REG = UART0_BIT_CENTER;
            UART0_TXBITCLKTX_COMPLETE_REG = (UART0_NUMBER_OF_DATA_BITS +
                        UART0_NUMBER_OF_START_BIT) * UART0_OVER_SAMPLE_COUNT;
        #else
            UART0_TXBITCTR_PERIOD_REG = ((UART0_NUMBER_OF_DATA_BITS +
                        UART0_NUMBER_OF_START_BIT) * UART0_OVER_SAMPLE_8) - 1u;
        #endif /* End UART0_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if(UART0_TX_INTERRUPT_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
            UART0_TXSTATUS_MASK_REG = UART0_TX_STS_FIFO_EMPTY;
        #else
            UART0_TXSTATUS_MASK_REG = UART0_INIT_TX_INTERRUPTS_MASK;
        #endif /*End UART0_TX_INTERRUPT_ENABLED*/

    #endif /* End UART0_TX_ENABLED */

    #if(UART0_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        UART0_WriteControlRegister( \
            (UART0_ReadControlRegister() & (uint8)~UART0_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(UART0_PARITY_TYPE << UART0_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End UART0_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: UART0_Enable
********************************************************************************
*
* Summary:
*  Enables the UART block operation
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART0_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void UART0_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if(UART0_RX_ENABLED || UART0_HD_ENABLED)
        /*RX Counter (Count7) Enable */
        UART0_RXBITCTR_CONTROL_REG |= UART0_CNTR_ENABLE;
        /* Enable the RX Interrupt. */
        UART0_RXSTATUS_ACTL_REG  |= UART0_INT_ENABLE;
        #if(UART0_RX_INTERRUPT_ENABLED && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))
            CyIntEnable(UART0_RX_VECT_NUM);
            #if (UART0_RXHW_ADDRESS_ENABLED)
                UART0_rxAddressDetected = 0u;
            #endif /* End UART0_RXHW_ADDRESS_ENABLED */
        #endif /* End UART0_RX_INTERRUPT_ENABLED */
    #endif /* End UART0_RX_ENABLED || UART0_HD_ENABLED*/

    #if(UART0_TX_ENABLED)
        /*TX Counter (DP/Count7) Enable */
        #if(!UART0_TXCLKGEN_DP)
            UART0_TXBITCTR_CONTROL_REG |= UART0_CNTR_ENABLE;
        #endif /* End UART0_TXCLKGEN_DP */
        /* Enable the TX Interrupt. */
        UART0_TXSTATUS_ACTL_REG |= UART0_INT_ENABLE;
        #if(UART0_TX_INTERRUPT_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
            CyIntEnable(UART0_TX_VECT_NUM);
        #endif /* End UART0_TX_INTERRUPT_ENABLED*/
     #endif /* End UART0_TX_ENABLED */

    #if(UART0_INTERNAL_CLOCK_USED)
        /* Enable the clock. */
        UART0_IntClock_Start();
    #endif /* End UART0_INTERNAL_CLOCK_USED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: UART0_Stop
********************************************************************************
*
* Summary:
*  Disable the UART component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void UART0_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if(UART0_RX_ENABLED || UART0_HD_ENABLED)
        UART0_RXBITCTR_CONTROL_REG &= (uint8)~UART0_CNTR_ENABLE;
    #endif /* End UART0_RX_ENABLED */

    #if(UART0_TX_ENABLED)
        #if(!UART0_TXCLKGEN_DP)
            UART0_TXBITCTR_CONTROL_REG &= (uint8)~UART0_CNTR_ENABLE;
        #endif /* End UART0_TXCLKGEN_DP */
    #endif /* UART0_TX_ENABLED */

    #if(UART0_INTERNAL_CLOCK_USED)
        /* Disable the clock. */
        UART0_IntClock_Stop();
    #endif /* End UART0_INTERNAL_CLOCK_USED */

    /* Disable internal interrupt component */
    #if(UART0_RX_ENABLED || UART0_HD_ENABLED)
        UART0_RXSTATUS_ACTL_REG  &= (uint8)~UART0_INT_ENABLE;
        #if(UART0_RX_INTERRUPT_ENABLED && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))
            UART0_DisableRxInt();
        #endif /* End UART0_RX_INTERRUPT_ENABLED */
    #endif /* End UART0_RX_ENABLED */

    #if(UART0_TX_ENABLED)
        UART0_TXSTATUS_ACTL_REG &= (uint8)~UART0_INT_ENABLE;
        #if(UART0_TX_INTERRUPT_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
            UART0_DisableTxInt();
        #endif /* End UART0_TX_INTERRUPT_ENABLED */
    #endif /* End UART0_TX_ENABLED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: UART0_ReadControlRegister
********************************************************************************
*
* Summary:
*  Read the current state of the control register
*
* Parameters:
*  None.
*
* Return:
*  Current state of the control register.
*
*******************************************************************************/
uint8 UART0_ReadControlRegister(void) 
{
    #if( UART0_CONTROL_REG_REMOVED )
        return(0u);
    #else
        return(UART0_CONTROL_REG);
    #endif /* End UART0_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: UART0_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  UART0_WriteControlRegister(uint8 control) 
{
    #if( UART0_CONTROL_REG_REMOVED )
        if(control != 0u) { }      /* release compiler warning */
    #else
       UART0_CONTROL_REG = control;
    #endif /* End UART0_CONTROL_REG_REMOVED */
}


#if(UART0_RX_ENABLED || UART0_HD_ENABLED)

    #if(UART0_RX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: UART0_EnableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UART0_EnableRxInt(void) 
        {
            CyIntEnable(UART0_RX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: UART0_DisableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UART0_DisableRxInt(void) 
        {
            CyIntDisable(UART0_RX_VECT_NUM);
        }

    #endif /* UART0_RX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: UART0_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  IntSrc:  An or'd combination of the desired status bit masks (defined in
    *           the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void UART0_SetRxInterruptMode(uint8 intSrc) 
    {
        UART0_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: UART0_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns data in RX Data register without checking status register to
    *  determine if data is valid
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  UART0_rxBuffer - RAM buffer pointer for save received data.
    *  UART0_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  UART0_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  UART0_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 UART0_ReadRxData(void) 
    {
        uint8 rxData;

        #if(UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_DisableRxInt();
            #endif /* UART0_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = UART0_rxBufferRead;
            loc_rxBufferWrite = UART0_rxBufferWrite;

            if( (UART0_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = UART0_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;

                if(loc_rxBufferRead >= UART0_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                UART0_rxBufferRead = loc_rxBufferRead;

                if(UART0_rxBufferLoopDetect != 0u )
                {
                    UART0_rxBufferLoopDetect = 0u;
                    #if( (UART0_RX_INTERRUPT_ENABLED) && (UART0_FLOW_CONTROL != 0u) && \
                         (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( UART0_HD_ENABLED )
                            if((UART0_CONTROL_REG & UART0_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only in RX
                                *  configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                UART0_RXSTATUS_MASK_REG  |= UART0_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            UART0_RXSTATUS_MASK_REG  |= UART0_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end UART0_HD_ENABLED */
                    #endif /* UART0_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }
            }
            else
            {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
                rxData = UART0_RXDATA_REG;
            }

            /* Enable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_EnableRxInt();
            #endif /* End UART0_RX_INTERRUPT_ENABLED */

        #else /* UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

            /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
            rxData = UART0_RXDATA_REG;

        #endif /* UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: UART0_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the current state of the status register
    *  And detect software buffer overflow.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Global Variables:
    *  UART0_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn?t free space in
    *   UART0_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   UART0_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 UART0_ReadRxStatus(void) 
    {
        uint8 status;

        status = UART0_RXSTATUS_REG & UART0_RX_HW_MASK;

        #if(UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH)
            if( UART0_rxBufferOverflow != 0u )
            {
                status |= UART0_RX_STS_SOFT_BUFF_OVER;
                UART0_rxBufferOverflow = 0u;
            }
        #endif /* UART0_RXBUFFERSIZE */

        return(status);
    }


    /*******************************************************************************
    * Function Name: UART0_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, if data is not available or an error
    *  condition exists, zero is returned; otherwise, character is read and
    *  returned.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  UART0_rxBuffer - RAM buffer pointer for save received data.
    *  UART0_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  UART0_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  UART0_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 UART0_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

        #if(UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_DisableRxInt();
            #endif /* UART0_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = UART0_rxBufferRead;
            loc_rxBufferWrite = UART0_rxBufferWrite;

            if( (UART0_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = UART0_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;
                if(loc_rxBufferRead >= UART0_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                UART0_rxBufferRead = loc_rxBufferRead;

                if(UART0_rxBufferLoopDetect > 0u )
                {
                    UART0_rxBufferLoopDetect = 0u;
                    #if( (UART0_RX_INTERRUPT_ENABLED) && (UART0_FLOW_CONTROL != 0u) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( UART0_HD_ENABLED )
                            if((UART0_CONTROL_REG & UART0_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only if
                                *  RX configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                UART0_RXSTATUS_MASK_REG  |= UART0_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            UART0_RXSTATUS_MASK_REG  |= UART0_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end UART0_HD_ENABLED */
                    #endif /* UART0_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }

            }
            else
            {   rxStatus = UART0_RXSTATUS_REG;
                if((rxStatus & UART0_RX_STS_FIFO_NOTEMPTY) != 0u)
                {   /* Read received data from FIFO*/
                    rxData = UART0_RXDATA_REG;
                    /*Check status on error*/
                    if((rxStatus & (UART0_RX_STS_BREAK | UART0_RX_STS_PAR_ERROR |
                                   UART0_RX_STS_STOP_ERROR | UART0_RX_STS_OVERRUN)) != 0u)
                    {
                        rxData = 0u;
                    }
                }
            }

            /* Enable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_EnableRxInt();
            #endif /* UART0_RX_INTERRUPT_ENABLED */

        #else /* UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

            rxStatus =UART0_RXSTATUS_REG;
            if((rxStatus & UART0_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO*/
                rxData = UART0_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (UART0_RX_STS_BREAK | UART0_RX_STS_PAR_ERROR |
                               UART0_RX_STS_STOP_ERROR | UART0_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        #endif /* UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: UART0_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Grab the next available byte of data from the recieve FIFO
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains Status Register and LSB contains UART RX data
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 UART0_GetByte(void) 
    {
        return ( ((uint16)UART0_ReadRxStatus() << 8u) | UART0_ReadRxData() );
    }


    /*******************************************************************************
    * Function Name: UART0_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of bytes left in the RX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Integer count of the number of bytes left
    *  in the RX buffer
    *
    * Global Variables:
    *  UART0_rxBufferWrite - used to calculate left bytes.
    *  UART0_rxBufferRead - used to calculate left bytes.
    *  UART0_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 UART0_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH)

            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_DisableRxInt();
            #endif /* UART0_RX_INTERRUPT_ENABLED */

            if(UART0_rxBufferRead == UART0_rxBufferWrite)
            {
                if(UART0_rxBufferLoopDetect > 0u)
                {
                    size = UART0_RXBUFFERSIZE;
                }
                else
                {
                    size = 0u;
                }
            }
            else if(UART0_rxBufferRead < UART0_rxBufferWrite)
            {
                size = (UART0_rxBufferWrite - UART0_rxBufferRead);
            }
            else
            {
                size = (UART0_RXBUFFERSIZE - UART0_rxBufferRead) + UART0_rxBufferWrite;
            }

            /* Enable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_EnableRxInt();
            #endif /* End UART0_RX_INTERRUPT_ENABLED */

        #else /* UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

            /* We can only know if there is data in the fifo. */
            size = ((UART0_RXSTATUS_REG & UART0_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

        #endif /* End UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: UART0_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the RX RAM buffer by setting the read and write pointers both to zero.
    *  Clears hardware RX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_rxBufferWrite - cleared to zero.
    *  UART0_rxBufferRead - cleared to zero.
    *  UART0_rxBufferLoopDetect - cleared to zero.
    *  UART0_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *******************************************************************************/
    void UART0_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* clear the HW FIFO */
        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        UART0_RXDATA_AUX_CTL_REG |=  UART0_RX_FIFO_CLR;
        UART0_RXDATA_AUX_CTL_REG &= (uint8)~UART0_RX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH)
            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_DisableRxInt();
            #endif /* End UART0_RX_INTERRUPT_ENABLED */

            UART0_rxBufferRead = 0u;
            UART0_rxBufferWrite = 0u;
            UART0_rxBufferLoopDetect = 0u;
            UART0_rxBufferOverflow = 0u;

            /* Enable Rx interrupt. */
            #if(UART0_RX_INTERRUPT_ENABLED)
                UART0_EnableRxInt();
            #endif /* End UART0_RX_INTERRUPT_ENABLED */
        #endif /* End UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH */

    }


    /*******************************************************************************
    * Function Name: UART0_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the receive addressing mode
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  UART0__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  UART0__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  UART0__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  UART0__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  UART0__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  UART0_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void UART0_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(UART0_RXHW_ADDRESS_ENABLED)
            #if(UART0_CONTROL_REG_REMOVED)
                if(addressMode != 0u) { }     /* release compiler warning */
            #else /* UART0_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = UART0_CONTROL_REG & (uint8)~UART0_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << UART0_CTRL_RXADDR_MODE0_SHIFT);
                UART0_CONTROL_REG = tmpCtrl;
                #if(UART0_RX_INTERRUPT_ENABLED && \
                   (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) )
                    UART0_rxAddressMode = addressMode;
                    UART0_rxAddressDetected = 0u;
                #endif /* End UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH*/
            #endif /* End UART0_CONTROL_REG_REMOVED */
        #else /* UART0_RXHW_ADDRESS_ENABLED */
            if(addressMode != 0u) { }     /* release compiler warning */
        #endif /* End UART0_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: UART0_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Set the first hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UART0_SetRxAddress1(uint8 address) 

    {
        UART0_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: UART0_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Set the second hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UART0_SetRxAddress2(uint8 address) 
    {
        UART0_RXADDRESS2_REG = address;
    }

#endif  /* UART0_RX_ENABLED || UART0_HD_ENABLED*/


#if( (UART0_TX_ENABLED) || (UART0_HD_ENABLED) )

    #if(UART0_TX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: UART0_EnableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UART0_EnableTxInt(void) 
        {
            CyIntEnable(UART0_TX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: UART0_DisableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UART0_DisableTxInt(void) 
        {
            CyIntDisable(UART0_TX_VECT_NUM);
        }

    #endif /* UART0_TX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: UART0_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  intSrc: An or'd combination of the desired status bit masks (defined in
    *          the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void UART0_SetTxInterruptMode(uint8 intSrc) 
    {
        UART0_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: UART0_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Write a byte of data to the Transmit FIFO or TX buffer to be sent when the
    *  bus is available. WriteTxData sends a byte without checking for buffer room
    *  or status. It is up to the user to separately check status.
    *
    * Parameters:
    *  TXDataByte: byte of data to place in the transmit FIFO
    *
    * Return:
    * void
    *
    * Global Variables:
    *  UART0_txBuffer - RAM buffer pointer for save data for transmission
    *  UART0_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  UART0_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  UART0_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UART0_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(UART0_initVar != 0u)
        {
            #if(UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH)

                /* Disable Tx interrupt. */
                /* Protect variables that could change on interrupt. */
                #if(UART0_TX_INTERRUPT_ENABLED)
                    UART0_DisableTxInt();
                #endif /* End UART0_TX_INTERRUPT_ENABLED */

                if( (UART0_txBufferRead == UART0_txBufferWrite) &&
                    ((UART0_TXSTATUS_REG & UART0_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    UART0_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(UART0_txBufferWrite >= UART0_TXBUFFERSIZE)
                    {
                        UART0_txBufferWrite = 0u;
                    }

                    UART0_txBuffer[UART0_txBufferWrite] = txDataByte;

                    /* Add to the software buffer. */
                    UART0_txBufferWrite++;

                }

                /* Enable Tx interrupt. */
                #if(UART0_TX_INTERRUPT_ENABLED)
                    UART0_EnableTxInt();
                #endif /* End UART0_TX_INTERRUPT_ENABLED */

            #else /* UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */

                /* Add directly to the FIFO. */
                UART0_TXDATA_REG = txDataByte;

            #endif /* End UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */
        }
    }


    /*******************************************************************************
    * Function Name: UART0_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the status register for the component
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the status register which is clear on read. It is up to
    *  the user to handle all bits in this return value accordingly, even if the bit
    *  was not enabled as an interrupt source the event happened and must be handled
    *  accordingly.
    *
    *******************************************************************************/
    uint8 UART0_ReadTxStatus(void) 
    {
        return(UART0_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: UART0_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Wait to send byte until TX register or buffer has room.
    *
    * Parameters:
    *  txDataByte: The 8-bit data value to send across the UART.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_txBuffer - RAM buffer pointer for save data for transmission
    *  UART0_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  UART0_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  UART0_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void UART0_PutChar(uint8 txDataByte) 
    {
            #if(UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH)
                /* The temporary output pointer is used since it takes two instructions
                *  to increment with a wrap, and we can't risk doing that with the real
                *  pointer and getting an interrupt in between instructions.
                */
                uint8 loc_txBufferWrite;
                uint8 loc_txBufferRead;

                do{
                    /* Block if software buffer is full, so we don't overwrite. */
                    #if ((UART0_TXBUFFERSIZE > UART0_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Disable TX interrupt to protect variables that could change on interrupt */
                        CyIntDisable(UART0_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    loc_txBufferWrite = UART0_txBufferWrite;
                    loc_txBufferRead = UART0_txBufferRead;
                    #if ((UART0_TXBUFFERSIZE > UART0_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Enable interrupt to continue transmission */
                        CyIntEnable(UART0_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }while( (loc_txBufferWrite < loc_txBufferRead) ? (loc_txBufferWrite == (loc_txBufferRead - 1u)) :
                                        ((loc_txBufferWrite - loc_txBufferRead) ==
                                        (uint8)(UART0_TXBUFFERSIZE - 1u)) );

                if( (loc_txBufferRead == loc_txBufferWrite) &&
                    ((UART0_TXSTATUS_REG & UART0_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    UART0_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(loc_txBufferWrite >= UART0_TXBUFFERSIZE)
                    {
                        loc_txBufferWrite = 0u;
                    }
                    /* Add to the software buffer. */
                    UART0_txBuffer[loc_txBufferWrite] = txDataByte;
                    loc_txBufferWrite++;

                    /* Finally, update the real output pointer */
                    #if ((UART0_TXBUFFERSIZE > UART0_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntDisable(UART0_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    UART0_txBufferWrite = loc_txBufferWrite;
                    #if ((UART0_TXBUFFERSIZE > UART0_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntEnable(UART0_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }

            #else /* UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */

                while((UART0_TXSTATUS_REG & UART0_TX_STS_FIFO_FULL) != 0u)
                {
                    ; /* Wait for room in the FIFO. */
                }

                /* Add directly to the FIFO. */
                UART0_TXDATA_REG = txDataByte;

            #endif /* End UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: UART0_PutString
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: char pointer to character string of Data to Send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  This function will block if there is not enough memory to place the whole
    *  string, it will block until the entire string has been written to the
    *  transmit buffer.
    *
    *******************************************************************************/
    void UART0_PutString(const char8 string[]) 
    {
        uint16 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(UART0_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent*/
            while(string[buf_index] != (char8)0)
            {
                UART0_PutChar((uint8)string[buf_index]);
                buf_index++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: UART0_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of Bytes to be transmitted.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UART0_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(UART0_initVar != 0u)
        {
            do
            {
                UART0_PutChar(string[buf_index]);
                buf_index++;
            }while(buf_index < byteCount);
        }
    }


    /*******************************************************************************
    * Function Name: UART0_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Write a character and then carriage return and line feed.
    *
    * Parameters:
    *  txDataByte: uint8 Character to send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UART0_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(UART0_initVar != 0u)
        {
            UART0_PutChar(txDataByte);
            UART0_PutChar(0x0Du);
            UART0_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: UART0_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of space left in the TX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Integer count of the number of bytes left in the TX buffer
    *
    * Global Variables:
    *  UART0_txBufferWrite - used to calculate left space.
    *  UART0_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 UART0_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UART0_TX_INTERRUPT_ENABLED)
                UART0_DisableTxInt();
            #endif /* End UART0_TX_INTERRUPT_ENABLED */

            if(UART0_txBufferRead == UART0_txBufferWrite)
            {
                size = 0u;
            }
            else if(UART0_txBufferRead < UART0_txBufferWrite)
            {
                size = (UART0_txBufferWrite - UART0_txBufferRead);
            }
            else
            {
                size = (UART0_TXBUFFERSIZE - UART0_txBufferRead) + UART0_txBufferWrite;
            }

            /* Enable Tx interrupt. */
            #if(UART0_TX_INTERRUPT_ENABLED)
                UART0_EnableTxInt();
            #endif /* End UART0_TX_INTERRUPT_ENABLED */

        #else /* UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */

            size = UART0_TXSTATUS_REG;

            /* Is the fifo is full. */
            if((size & UART0_TX_STS_FIFO_FULL) != 0u)
            {
                size = UART0_FIFO_LENGTH;
            }
            else if((size & UART0_TX_STS_FIFO_EMPTY) != 0u)
            {
                size = 0u;
            }
            else
            {
                /* We only know there is data in the fifo. */
                size = 1u;
            }

        #endif /* End UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: UART0_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the TX RAM buffer by setting the read and write pointers both to zero.
    *  Clears the hardware TX FIFO.  Any data present in the FIFO will not be sent.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_txBufferWrite - cleared to zero.
    *  UART0_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM buffer will be lost when overwritten.
    *
    *******************************************************************************/
    void UART0_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        /* clear the HW FIFO */
        UART0_TXDATA_AUX_CTL_REG |=  UART0_TX_FIFO_CLR;
        UART0_TXDATA_AUX_CTL_REG &= (uint8)~UART0_TX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UART0_TX_INTERRUPT_ENABLED)
                UART0_DisableTxInt();
            #endif /* End UART0_TX_INTERRUPT_ENABLED */

            UART0_txBufferRead = 0u;
            UART0_txBufferWrite = 0u;

            /* Enable Tx interrupt. */
            #if(UART0_TX_INTERRUPT_ENABLED)
                UART0_EnableTxInt();
            #endif /* End UART0_TX_INTERRUPT_ENABLED */

        #endif /* End UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: UART0_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Write a Break command to the UART
    *
    * Parameters:
    *  uint8 retMode:  Wait mode,
    *   0 - Initialize registers for Break, sends the Break signal and return
    *       imediately.
    *   1 - Wait until Break sending is complete, reinitialize registers to normal
    *       transmission mode then return.
    *   2 - Reinitialize registers to normal transmission mode then return.
    *   3 - both steps: 0 and 1
    *       init registers for Break, send Break signal
    *       wait until Break sending is complete, reinit registers to normal
    *       transmission mode then return.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_initVar - checked to identify that the component has been
    *     initialized.
    *  tx_period - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  Trere are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Funcition will block CPU untill transmition
    *     complete.
    *  2) User may want to use bloking time if UART configured to the low speed
    *     operation
    *     Emample for this case:
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to init and use the interrupt for complete
    *     break operation.
    *     Example for this case:
    *     Init TX interrupt whith "TX - On TX Complete" parameter
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     When interrupt appear with UART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *   Uses static variable to keep registers configuration.
    *
    *******************************************************************************/
    void UART0_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(UART0_initVar != 0u)
        {
            /*Set the Counter to 13-bits and transmit a 00 byte*/
            /*When that is done then reset the counter value back*/
            uint8 tmpStat;

            #if(UART0_HD_ENABLED) /* Half Duplex mode*/

                if( (retMode == UART0_SEND_BREAK) ||
                    (retMode == UART0_SEND_WAIT_REINIT ) )
                {
                    /* CTRL_HD_SEND_BREAK - sends break bits in HD mode*/
                    UART0_WriteControlRegister(UART0_ReadControlRegister() |
                                                          UART0_CTRL_HD_SEND_BREAK);
                    /* Send zeros*/
                    UART0_TXDATA_REG = 0u;

                    do /*wait until transmit starts*/
                    {
                        tmpStat = UART0_TXSTATUS_REG;
                    }while((tmpStat & UART0_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == UART0_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UART0_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = UART0_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & UART0_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == UART0_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UART0_REINIT) ||
                    (retMode == UART0_SEND_WAIT_REINIT) )
                {
                    UART0_WriteControlRegister(UART0_ReadControlRegister() &
                                                  (uint8)~UART0_CTRL_HD_SEND_BREAK);
                }

            #else /* UART0_HD_ENABLED Full Duplex mode */

                static uint8 tx_period;

                if( (retMode == UART0_SEND_BREAK) ||
                    (retMode == UART0_SEND_WAIT_REINIT) )
                {
                    /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode*/
                    #if( (UART0_PARITY_TYPE != UART0__B_UART__NONE_REVB) || \
                                        (UART0_PARITY_TYPE_SW != 0u) )
                        UART0_WriteControlRegister(UART0_ReadControlRegister() |
                                                              UART0_CTRL_HD_SEND_BREAK);
                    #endif /* End UART0_PARITY_TYPE != UART0__B_UART__NONE_REVB  */

                    #if(UART0_TXCLKGEN_DP)
                        tx_period = UART0_TXBITCLKTX_COMPLETE_REG;
                        UART0_TXBITCLKTX_COMPLETE_REG = UART0_TXBITCTR_BREAKBITS;
                    #else
                        tx_period = UART0_TXBITCTR_PERIOD_REG;
                        UART0_TXBITCTR_PERIOD_REG = UART0_TXBITCTR_BREAKBITS8X;
                    #endif /* End UART0_TXCLKGEN_DP */

                    /* Send zeros*/
                    UART0_TXDATA_REG = 0u;

                    do /* wait until transmit starts */
                    {
                        tmpStat = UART0_TXSTATUS_REG;
                    }while((tmpStat & UART0_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == UART0_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UART0_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = UART0_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & UART0_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == UART0_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UART0_REINIT) ||
                    (retMode == UART0_SEND_WAIT_REINIT) )
                {

                    #if(UART0_TXCLKGEN_DP)
                        UART0_TXBITCLKTX_COMPLETE_REG = tx_period;
                    #else
                        UART0_TXBITCTR_PERIOD_REG = tx_period;
                    #endif /* End UART0_TXCLKGEN_DP */

                    #if( (UART0_PARITY_TYPE != UART0__B_UART__NONE_REVB) || \
                         (UART0_PARITY_TYPE_SW != 0u) )
                        UART0_WriteControlRegister(UART0_ReadControlRegister() &
                                                      (uint8)~UART0_CTRL_HD_SEND_BREAK);
                    #endif /* End UART0_PARITY_TYPE != NONE */
                }
            #endif    /* End UART0_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: UART0_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the transmit addressing mode
    *
    * Parameters:
    *  addressMode: 0 -> Space
    *               1 -> Mark
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UART0_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable*/
        if(addressMode != 0u)
        {
            #if( UART0_CONTROL_REG_REMOVED == 0u )
                UART0_WriteControlRegister(UART0_ReadControlRegister() |
                                                      UART0_CTRL_MARK);
            #endif /* End UART0_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
            #if( UART0_CONTROL_REG_REMOVED == 0u )
                UART0_WriteControlRegister(UART0_ReadControlRegister() &
                                                    (uint8)~UART0_CTRL_MARK);
            #endif /* End UART0_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndUART0_TX_ENABLED */

#if(UART0_HD_ENABLED)


    /*******************************************************************************
    * Function Name: UART0_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Rx configuration if required and loads the
    *  Tx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Tx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART.
    *
    * Side Effects:
    *  Disable RX interrupt mask, when software buffer has been used.
    *
    *******************************************************************************/
    void UART0_LoadTxConfig(void) 
    {
        #if((UART0_RX_INTERRUPT_ENABLED) && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))
            /* Disable RX interrupts before set TX configuration */
            UART0_SetRxInterruptMode(0u);
        #endif /* UART0_RX_INTERRUPT_ENABLED */

        UART0_WriteControlRegister(UART0_ReadControlRegister() | UART0_CTRL_HD_SEND);
        UART0_RXBITCTR_PERIOD_REG = UART0_HD_TXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(UART0_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */
    }


    /*******************************************************************************
    * Function Name: UART0_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Tx configuration if required and loads the
    *  Rx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Rx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART
    *
    * Side Effects:
    *  Set RX interrupt mask based on customizer settings, when software buffer
    *  has been used.
    *
    *******************************************************************************/
    void UART0_LoadRxConfig(void) 
    {
        UART0_WriteControlRegister(UART0_ReadControlRegister() &
                                                (uint8)~UART0_CTRL_HD_SEND);
        UART0_RXBITCTR_PERIOD_REG = UART0_HD_RXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(UART0_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */

        #if((UART0_RX_INTERRUPT_ENABLED) && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))
            /* Enable RX interrupt after set RX configuration */
            UART0_SetRxInterruptMode(UART0_INIT_RX_INTERRUPTS_MASK);
        #endif /* UART0_RX_INTERRUPT_ENABLED */
    }

#endif  /* UART0_HD_ENABLED */


/* [] END OF FILE */
