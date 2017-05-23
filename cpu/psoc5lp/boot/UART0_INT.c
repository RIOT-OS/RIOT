/*******************************************************************************
* File Name: UART0_INT.c
* Version 2.30
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART0.h"
#include "CyLib.h"


/***************************************
* Custom Declratations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if( (UART0_RX_ENABLED || UART0_HD_ENABLED) && \
     (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: UART0_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_rxBuffer - RAM buffer pointer for save received data.
    *  UART0_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  UART0_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  UART0_rxBufferOverflow - software overflow flag. Set to one
    *     when UART0_rxBufferWrite index overtakes
    *     UART0_rxBufferRead index.
    *  UART0_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when UART0_rxBufferWrite is equal to
    *    UART0_rxBufferRead
    *  UART0_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  UART0_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(UART0_RXISR)
    {
        uint8 readData;
        uint8 increment_pointer = 0u;
        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START UART0_RXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        readData = UART0_RXSTATUS_REG;

        if((readData & (UART0_RX_STS_BREAK | UART0_RX_STS_PAR_ERROR |
                        UART0_RX_STS_STOP_ERROR | UART0_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            /* `#START UART0_RXISR_ERROR` */

            /* `#END` */
        }

        while((readData & UART0_RX_STS_FIFO_NOTEMPTY) != 0u)
        {

            #if (UART0_RXHW_ADDRESS_ENABLED)
                if(UART0_rxAddressMode == (uint8)UART0__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readData & UART0_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readData & UART0_RX_STS_ADDR_MATCH) != 0u)
                        {
                            UART0_rxAddressDetected = 1u;
                        }
                        else
                        {
                            UART0_rxAddressDetected = 0u;
                        }
                    }

                    readData = UART0_RXDATA_REG;
                    if(UART0_rxAddressDetected != 0u)
                    {   /* store only addressed data */
                        UART0_rxBuffer[UART0_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* without software addressing */
                {
                    UART0_rxBuffer[UART0_rxBufferWrite] = UART0_RXDATA_REG;
                    increment_pointer = 1u;
                }
            #else  /* without addressing */
                UART0_rxBuffer[UART0_rxBufferWrite] = UART0_RXDATA_REG;
                increment_pointer = 1u;
            #endif /* End SW_DETECT_TO_BUFFER */

            /* do not increment buffer pointer when skip not adderessed data */
            if( increment_pointer != 0u )
            {
                if(UART0_rxBufferLoopDetect != 0u)
                {   /* Set Software Buffer status Overflow */
                    UART0_rxBufferOverflow = 1u;
                }
                /* Set next pointer. */
                UART0_rxBufferWrite++;

                /* Check pointer for a loop condition */
                if(UART0_rxBufferWrite >= UART0_RXBUFFERSIZE)
                {
                    UART0_rxBufferWrite = 0u;
                }
                /* Detect pre-overload condition and set flag */
                if(UART0_rxBufferWrite == UART0_rxBufferRead)
                {
                    UART0_rxBufferLoopDetect = 1u;
                    /* When Hardware Flow Control selected */
                    #if(UART0_FLOW_CONTROL != 0u)
                    /* Disable RX interrupt mask, it will be enabled when user read data from the buffer using APIs */
                        UART0_RXSTATUS_MASK_REG  &= (uint8)~UART0_RX_STS_FIFO_NOTEMPTY;
                        CyIntClearPending(UART0_RX_VECT_NUM);
                        break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                    #endif /* End UART0_FLOW_CONTROL != 0 */
                }
            }

            /* Check again if there is data. */
            readData = UART0_RXSTATUS_REG;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START UART0_RXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End UART0_RX_ENABLED && (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) */


#if(UART0_TX_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: UART0_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART0_txBuffer - RAM buffer pointer for transmit data from.
    *  UART0_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmited byte.
    *  UART0_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(UART0_TXISR)
    {

        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START UART0_TXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        while((UART0_txBufferRead != UART0_txBufferWrite) &&
             ((UART0_TXSTATUS_REG & UART0_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer. */
            if(UART0_txBufferRead >= UART0_TXBUFFERSIZE)
            {
                UART0_txBufferRead = 0u;
            }

            UART0_TXDATA_REG = UART0_txBuffer[UART0_txBufferRead];

            /* Set next pointer. */
            UART0_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START UART0_TXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End UART0_TX_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH) */


/* [] END OF FILE */
