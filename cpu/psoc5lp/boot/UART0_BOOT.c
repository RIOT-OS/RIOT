/*******************************************************************************
* File Name: UART0_BOOT.c
* Version 2.30
*
* Description:
*  This file provides the source code of bootloader communication APIs for the
*  UART component.
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

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (0u != ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UART0) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)))


/*******************************************************************************
* Function Name: UART0_CyBtldrCommStart
********************************************************************************
*
* Summary:
*  Starts the UART communication component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This component automatically enables global interrupt.
*
*******************************************************************************/
void UART0_CyBtldrCommStart(void) CYSMALL
{
    /* Start UART component and clear the Tx,Rx buffers */
    UART0_Start();
    UART0_ClearRxBuffer();
    UART0_ClearTxBuffer();
}


/*******************************************************************************
* Function Name: UART0_CyBtldrCommStop
********************************************************************************
*
* Summary:
*  Disables the communication component and disables the interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART0_CyBtldrCommStop(void) CYSMALL
{
    /* Stop UART component */
    UART0_Stop();
}


/*******************************************************************************
* Function Name: UART0_CyBtldrCommReset
********************************************************************************
*
* Summary:
*  Resets the receive and transmit communication Buffers.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART0_CyBtldrCommReset(void) CYSMALL
{
    /* Clear RX and TX buffers */
    UART0_ClearRxBuffer();
    UART0_ClearTxBuffer();
}


/*******************************************************************************
* Function Name: UART0_CyBtldrCommWrite
********************************************************************************
*
* Summary:
*  Allows the caller to write data to the boot loader host. This function uses
* a blocking write function for writing data using UART communication component.
*
* Parameters:
*  pData:    A pointer to the block of data to send to the device
*  size:     The number of bytes to write.
*  count:    Pointer to an unsigned short variable to write the number of
*             bytes actually written.
*  timeOut:  Number of units to wait before returning because of a timeout.
*
* Return:
*   cystatus: This function will return CYRET_SUCCESS if data is sent
*             succesfully.
*
* Side Effects:
*  This function should be called after command was received .
*
*******************************************************************************/
cystatus UART0_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL

{
    uint16 buf_index = 0u;

    /* The timeout is not used in this function, defined to avoid compiler warning */
    if(timeOut != 0u) { }

    /* Clear receive buffers */
    UART0_ClearRxBuffer();

    /* Write TX data using blocking function */
    do
    {
        UART0_PutChar(pData[buf_index]);
        buf_index++;
    }while(buf_index < size);

    /* Return success code */
    *count = size;

    return (CYRET_SUCCESS);
}


/*******************************************************************************
* Function Name: UART0_CyBtldrCommRead
********************************************************************************
*
* Summary:
*  Receives the command.
*
* Parameters:
*  pData:    A pointer to the area to store the block of data received
*             from the device.
*  size:     Maximum size of the read buffer
*  count:    Pointer to an unsigned short variable to write the number
*             of bytes actually read.
*  timeOut:  Number of units to wait before returning because of a timeOut.
*            Timeout is measured in 10s of ms.
*
* Return:
*  cystatus: This function will return CYRET_SUCCESS if atleast one byte is
*            received succesfully within the timeout interval .If no data is
*            received  this function will return CYRET_EMPTY.
*
*  BYTE2BYTE_TIME_OUT is used for detecting timeout marking end of block data
*  from host. This has to be set to a value which is greater than the expected
*  maximum delay between two bytes during a block/packet transmission from the
*  host. You have to account for the delay in hardware convertors while
*  calculating this value, if you are using any USB-UART bridges.
*******************************************************************************/
cystatus UART0_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL

{
    uint16 iCntr;
    uint16 dataIndexCntr;
    uint16 tempCount;
    uint16 oldDataCount;

    cystatus status = CYRET_EMPTY;

    /* Check whether data is received within the timeout period.
    *  Timeout period is in units of 10ms.
    *  If atleast one byte is received within the timeout interval, wait for more data */
    for (iCntr = 0u; iCntr < ((uint16)10u * timeOut); iCntr++)
    {
        /* If atleast one byte is received within the timeout interval
        *  enter the next loop waiting for more data reception
        */
        if(0u != UART0_GetRxBufferSize())
        {
            /* Wait for more data until 25ms byte to byte time out interval.
            * If no data is received during the last 25 ms(BYTE2BYTE_TIME_OUT)
            * then it is considered as end of transmitted data block(packet)
            * from the host and the program execution will break from the
            * data awaiting loop with status=CYRET_SUCCESS
            */
            do
            {

                oldDataCount = UART0_GetRxBufferSize();
                CyDelay(UART0_BYTE2BYTE_TIME_OUT);
            }while(UART0_GetRxBufferSize() > oldDataCount);
            status = CYRET_SUCCESS;
            break;
        }
        /* If not data is received , give a delay of 1ms and check again until the Timeout specified in .cydwr. */
        else
        {
            CyDelay(1u);
        }
    }

    /* Initialize the data read indexes and Count value*/
    *count = 0u;
    dataIndexCntr = 0u;

    /* If GetRxBufferSize()>0 , move the received data to the pData buffer */
    while(UART0_GetRxBufferSize() > 0u)
    {
        tempCount = UART0_GetRxBufferSize();
        *count  =(*count) + tempCount;

        /* Check if buffer overflow will occur before moving the data */
        if(*count < size)
        {
            for (iCntr = 0u; iCntr < tempCount; iCntr++)
            {
                /* Read the data and move it to the pData buffer */
                pData[dataIndexCntr] = UART0_ReadRxData();
                dataIndexCntr++;
            }

            /* Check if the last data received is End of packet(0x17)
            *  If not wait for additional 5ms
            */
            if(pData[dataIndexCntr - 1u] != 0x17u)
            {
                CyDelay(5u);
            }
        }
        /* If there is no space to move data, break from the loop */
        else
        {
            *count = (*count) - tempCount;
            break;
        }
    }
    return (status);
}

#endif /* end CYDEV_BOOTLOADER_IO_COMP */


/* [] END OF FILE */
