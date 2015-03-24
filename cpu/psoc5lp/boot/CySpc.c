/*******************************************************************************
* File Name: CySpc.c
* Version 4.20
*
*  Description:
*   Provides an API for the System Performance Component.
*   The SPC functions are not meant to be called directly by the user
*   application.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CySpc.h"

#define CY_SPC_KEY_ONE                      (0xB6u)
#define CY_SPC_KEY_TWO(x)                   ((uint8) (((uint16) 0xD3u) + ((uint16) (x))))

/* Command Codes */
#define CY_SPC_CMD_LD_BYTE                  (0x00u)
#define CY_SPC_CMD_LD_MULTI_BYTE            (0x01u)
#define CY_SPC_CMD_LD_ROW                   (0x02u)
#define CY_SPC_CMD_RD_BYTE                  (0x03u)
#define CY_SPC_CMD_RD_MULTI_BYTE            (0x04u)
#define CY_SPC_CMD_WR_ROW                   (0x05u)
#define CY_SPC_CMD_WR_USER_NVL              (0x06u)
#define CY_SPC_CMD_PRG_ROW                  (0x07u)
#define CY_SPC_CMD_ER_SECTOR                (0x08u)
#define CY_SPC_CMD_ER_ALL                   (0x09u)
#define CY_SPC_CMD_RD_HIDDEN                (0x0Au)
#define CY_SPC_CMD_PRG_PROTECT              (0x0Bu)
#define CY_SPC_CMD_CHECKSUM                 (0x0Cu)
#define CY_SPC_CMD_DWNLD_ALGORITHM          (0x0Du)
#define CY_SPC_CMD_GET_TEMP                 (0x0Eu)
#define CY_SPC_CMD_GET_ADC                  (0x0Fu)
#define CY_SPC_CMD_RD_NVL_VOLATILE          (0x10u)
#define CY_SPC_CMD_SETUP_TS                 (0x11u)
#define CY_SPC_CMD_DISABLE_TS               (0x12u)
#define CY_SPC_CMD_ER_ROW                   (0x13u)

/* Enable bit in Active and Alternate Active mode templates */
#define PM_SPC_PM_EN                        (0x08u)

/* Gate calls to the SPC. */
uint8 SpcLockState = CY_SPC_UNLOCKED;


#if(CY_PSOC5)

    /***************************************************************************
    * The wait-state pipeline must be enabled prior to accessing the SPC
    * register interface regardless of CPU frequency. The CySpcLock() saves
    * current wait-state pipeline state and enables it. The CySpcUnlock()
    * function, which must be called after SPC transaction, restores original
    * state.
    ***************************************************************************/
    static uint32 spcWaitPipeBypass = 0u;

#endif  /* (CY_PSOC5) */


/*******************************************************************************
* Function Name: CySpcStart
********************************************************************************
* Summary:
*  Starts the SPC.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CySpcStart(void)
{
    /* Save current global interrupt enable and disable it */
    uint8 interruptState = CyEnterCriticalSection();

    CY_SPC_PM_ACT_REG  |= PM_SPC_PM_EN;
    CY_SPC_PM_STBY_REG |= PM_SPC_PM_EN;

    /* Restore global interrupt enable state */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySpcStop
********************************************************************************
* Summary:
*  Stops the SPC.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CySpcStop(void)
{
    /* Save current global interrupt enable and disable it */
    uint8 interruptState = CyEnterCriticalSection();

    CY_SPC_PM_ACT_REG  &= ((uint8)(~PM_SPC_PM_EN));
    CY_SPC_PM_STBY_REG &= ((uint8)(~PM_SPC_PM_EN));

    /* Restore global interrupt enable state */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySpcReadData
********************************************************************************
* Summary:
*  Reads data from the SPC.
*
* Parameters:
*  uint8 buffer:
*   Address to store data read.
*
*  uint8 size:
*   Number of bytes to read from the SPC.
*
* Return:
*  uint8:
*   The number of bytes read from the SPC.
*
*******************************************************************************/
uint8 CySpcReadData(uint8 buffer[], uint8 size)
{
    uint8 i;

    for(i = 0u; i < size; i++)
    {
        while(!CY_SPC_DATA_READY)
        {
            CyDelayUs(1u);
        }
        buffer[i] = CY_SPC_CPU_DATA_REG;
    }

    return(i);
}


/*******************************************************************************
* Function Name: CySpcLoadMultiByte
********************************************************************************
* Summary:
*  Loads 1 to 32 bytes of data into the row latch of a Flash/EEPROM array.
*
* Parameters:
*  uint8 array:
*   Id of the array.
*
*  uint16 address:
*   Flash/eeprom addrress
*
*  uint8* buffer:
*   Data to load to the row latch
*
*  uint16 number:
*   Number bytes to load.
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*  CYRET_BAD_PARAM
*
*******************************************************************************/
cystatus CySpcLoadMultiByte(uint8 array, uint16 address, const uint8 buffer[], uint8 size)\

{
    cystatus status = CYRET_STARTED;
    uint8 i;

    /***************************************************************************
    * Check if number is correct for array. Number must be less than
    * 32 for Flash or less than 16 for EEPROM.
    ***************************************************************************/
    if(((array < CY_SPC_LAST_FLASH_ARRAYID) && (size < 32u)) ||
       ((array > CY_SPC_LAST_FLASH_ARRAYID) && (size < 16u)))
    {
        if(CY_SPC_IDLE)
        {
            CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
            CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_LD_MULTI_BYTE);
            CY_SPC_CPU_DATA_REG = CY_SPC_CMD_LD_MULTI_BYTE;

            if(CY_SPC_BUSY)
            {
                CY_SPC_CPU_DATA_REG = array;
                CY_SPC_CPU_DATA_REG = 1u & HI8(address);
                CY_SPC_CPU_DATA_REG = LO8(address);
                CY_SPC_CPU_DATA_REG = ((uint8)(size - 1u));

                for(i = 0u; i < size; i++)
                {
                    CY_SPC_CPU_DATA_REG = buffer[i];
                }
            }
            else
            {
                status = CYRET_CANCELED;
            }
        }
        else
        {
            status = CYRET_LOCKED;
        }
    }
    else
    {
        status = CYRET_BAD_PARAM;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcLoadRow
********************************************************************************
* Summary:
*  Loads a row of data into the row latch of a Flash/EEPROM array.
*
*  The buffer pointer should point to the data that should be written to the
*  flash row directly (no data in ECC/flash will be preserved). It is Flash API
*  responsibility to prepare data: the preserved data are copied from flash into
*  array with the modified data.
*
* Parameters:
*  uint8 array:
*   Id of the array.
*
*  uint8* buffer:
*   Data to be loaded to the row latch
*
*  uint8 size:
*   The number of data bytes that the SPC expects to be written. Depends on the
*   type of the array and, if the array is Flash, whether ECC is being enabled
*   or not. There are following values: flash row latch size with ECC enabled,
*   flash row latch size with ECC disabled and EEPROM row latch size.
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcLoadRow(uint8 array, const uint8 buffer[], uint16 size)
{
    cystatus status = CYRET_STARTED;
    uint16 i;

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_LD_ROW);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_LD_ROW;

        /* Make sure the command was accepted */
        if(CY_SPC_BUSY)
        {
            CY_SPC_CPU_DATA_REG = array;

            for(i = 0u; i < size; i++)
            {
                CY_SPC_CPU_DATA_REG = buffer[i];
            }
        }
        else
        {
            status = CYRET_CANCELED;
        }
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcLoadRowFull
********************************************************************************
* Summary:
*  Loads a row of data into the row latch of a Flash/EEPROM array.
*
*  The only data that are going to be changed should be passed. The function
*  will handle unmodified data preservation based on DWR settings and input
*  parameters.
*
* Parameters:
*  uint8 array:
*   Id of the array.
*
*  uint16 row:
*   Flash row number to be loaded.
*
*  uint8* buffer:
*   Data to be loaded to the row latch
*
*  uint8 size:
*   The number of data bytes that the SPC expects to be written. Depends on the
*   type of the array and, if the array is Flash, whether ECC is being enabled
*   or not. There are following values: flash row latch size with ECC enabled,
*   flash row latch size with ECC disabled and EEPROM row latch size.
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcLoadRowFull(uint8 array, uint16 row, const uint8 buffer[], uint16 size)\

{
    cystatus status = CYRET_STARTED;
    uint16 i;

    #if (CYDEV_ECC_ENABLE == 0)
        uint32 offset;
    #endif /* (CYDEV_ECC_ENABLE == 0) */

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_LD_ROW);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_LD_ROW;

        /* Make sure the command was accepted */
        if(CY_SPC_BUSY)
        {
            CY_SPC_CPU_DATA_REG = array;

            /*******************************************************************
            * If "Enable Error Correcting Code (ECC)" and "Store Configuration
            * Data in ECC" DWR options are disabled, ECC section is available
            * for user data.
            *******************************************************************/
            #if ((CYDEV_ECC_ENABLE == 0u) && (CYDEV_CONFIGURATION_ECC == 0u))

                /*******************************************************************
                * If size parameter equals size of the ECC row and selected array
                * identification corresponds to the flash array (but not to EEPROM
                * array) then data are going to be written to the ECC section.
                * In this case flash data must be preserved. The flash data copied
                * from flash data section to the SPC data register.
                *******************************************************************/
                if ((size == CYDEV_ECC_ROW_SIZE) && (array <= CY_SPC_LAST_FLASH_ARRAYID))
                {
                    offset = CYDEV_FLS_BASE +
                             ((uint32) array * CYDEV_FLS_SECTOR_SIZE) +
                             ((uint32)   row * CYDEV_FLS_ROW_SIZE   );

                    for (i = 0u; i < CYDEV_FLS_ROW_SIZE; i++)
                    {
                        CY_SPC_CPU_DATA_REG = CY_GET_XTND_REG8((void CYFAR *)(offset + i));
                    }
                }

            #endif /* ((CYDEV_ECC_ENABLE == 0u) && (CYDEV_CONFIGURATION_ECC == 0u)) */


            for(i = 0u; i < size; i++)
            {
                CY_SPC_CPU_DATA_REG = buffer[i];
            }


            /*******************************************************************
            * If "Enable Error Correcting Code (ECC)" DWR option is disabled,
            * ECC section can be used for storing device configuration data
            * ("Store Configuration Data in ECC" DWR option is enabled) or for
            * storing user data in the ECC section ("Store Configuration Data in
            * ECC" DWR option is enabled). In both cases, the data in the ECC
            * section must be preserved if flash data is written.
            *******************************************************************/
            #if (CYDEV_ECC_ENABLE == 0)


                /*******************************************************************
                * If size parameter equals size of the flash row and selected array
                * identification corresponds to the flash array (but not to EEPROM
                * array) then data are going to be written to the flash data
                * section. In this case, ECC section data must be preserved.
                * The ECC section data copied from ECC section to the SPC data
                * register.
                *******************************************************************/
                if ((size == CYDEV_FLS_ROW_SIZE) && (array <= CY_SPC_LAST_FLASH_ARRAYID))
                {
                    offset = CYDEV_ECC_BASE +
                            ((uint32) array * CYDEV_ECC_SECTOR_SIZE) +
                            ((uint32) row   * CYDEV_ECC_ROW_SIZE   );

                    for (i = 0u; i < CYDEV_ECC_ROW_SIZE; i++)
                    {
                        CY_SPC_CPU_DATA_REG = CY_GET_XTND_REG8((void CYFAR *)(offset + i));
                    }
                }

            #else

                if(0u != row)
                {
                    /* To remove unreferenced local variable warning */
                }

            #endif /* (CYDEV_ECC_ENABLE == 0) */
        }
        else
        {
            status = CYRET_CANCELED;
        }
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcWriteRow
********************************************************************************
* Summary:
*  Erases then programs a row in Flash/EEPROM with data in row latch.
*
* Parameters:
*  uint8 array:
*   Id of the array.
*
*  uint16 address:
*   flash/eeprom addrress
*
*  uint8 tempPolarity:
*   temperature polarity.
*   1: the Temp Magnitude is interpreted as a positive value
*   0: the Temp Magnitude is interpreted as a negative value
*
*  uint8 tempMagnitude:
*   temperature magnitude.
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcWriteRow(uint8 array, uint16 address, uint8 tempPolarity, uint8 tempMagnitude)\

{
    cystatus status = CYRET_STARTED;

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_WR_ROW);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_WR_ROW;

        /* Make sure the command was accepted */
        if(CY_SPC_BUSY)
        {
            CY_SPC_CPU_DATA_REG = array;
            CY_SPC_CPU_DATA_REG = HI8(address);
            CY_SPC_CPU_DATA_REG = LO8(address);
            CY_SPC_CPU_DATA_REG = tempPolarity;
            CY_SPC_CPU_DATA_REG = tempMagnitude;
        }
        else
        {
            status = CYRET_CANCELED;
        }
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcEraseSector
********************************************************************************
* Summary:
*  Erases all data in the addressed sector (block of 64 rows).
*
* Parameters:
*  uint8 array:
*   Id of the array.
*
*  uint8 sectorNumber:
*   Zero based sector number within Flash/EEPROM array
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcEraseSector(uint8 array, uint8 sectorNumber)
{
    cystatus status = CYRET_STARTED;

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_ER_SECTOR);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_ER_SECTOR;

        /* Make sure the command was accepted */
        if(CY_SPC_BUSY)
        {
            CY_SPC_CPU_DATA_REG = array;
            CY_SPC_CPU_DATA_REG = sectorNumber;
        }
        else
        {
            status = CYRET_CANCELED;
        }
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcGetTemp
********************************************************************************
* Summary:
*  Returns the internal die temperature
*
* Parameters:
*  uint8 numSamples:
*   Number of samples. Valid values are 1-5, resulting in 2 - 32 samples
*   respectively.
*
* uint16 timerPeriod:
*   Number of ADC ACLK cycles. A valid 14 bit value is accepted, higher 2 bits
*   of 16 bit values are ignored.
*
* uint8 clkDivSelect:
*   ADC ACLK clock divide value. Valid values are 2 - 225.
*
* Return:
*  CYRET_STARTED
*  CYRET_CANCELED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcGetTemp(uint8 numSamples)
{
    cystatus status = CYRET_STARTED;

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_GET_TEMP);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_GET_TEMP;

        /* Make sure the command was accepted */
        if(CY_SPC_BUSY)
        {
            CY_SPC_CPU_DATA_REG = numSamples;
        }
        else
        {
            status = CYRET_CANCELED;
        }
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}


/*******************************************************************************
* Function Name: CySpcLock
********************************************************************************
* Summary:
*  Locks the SPC so it can not be used by someone else:
*   - Saves wait-pipeline enable state and enable pipeline (PSoC5)
*
* Parameters:
*  Note
*
* Return:
*  CYRET_SUCCESS - if the resource was free.
*  CYRET_LOCKED  - if the SPC is in use.
*
*******************************************************************************/
cystatus CySpcLock(void)
{
    cystatus status = CYRET_LOCKED;
    uint8 interruptState;

    /* Enter critical section */
    interruptState = CyEnterCriticalSection();

    if(CY_SPC_UNLOCKED == SpcLockState)
    {
        SpcLockState = CY_SPC_LOCKED;
        status = CYRET_SUCCESS;

        #if(CY_PSOC5)

            if(0u != (CY_SPC_CPU_WAITPIPE_REG & CY_SPC_CPU_WAITPIPE_BYPASS))
            {
                /* Enable pipeline registers */
                CY_SPC_CPU_WAITPIPE_REG &= ((uint32)(~CY_SPC_CPU_WAITPIPE_BYPASS));

                /* At least 2 NOP instructions are recommended */
                CY_NOP;
                CY_NOP;
                CY_NOP;

                spcWaitPipeBypass = CY_SPC_CPU_WAITPIPE_BYPASS;
            }

        #endif  /* (CY_PSOC5) */
    }

    /* Exit critical section */
    CyExitCriticalSection(interruptState);

    return(status);
}


/*******************************************************************************
* Function Name: CySpcUnlock
********************************************************************************
* Summary:
*  Unlocks the SPC so it can be used by someone else:
*   - Restores wait-pipeline enable state (PSoC5)
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CySpcUnlock(void)
{
    uint8 interruptState;

    /* Enter critical section */
    interruptState = CyEnterCriticalSection();

    /* Release the SPC object */
    SpcLockState = CY_SPC_UNLOCKED;

    #if(CY_PSOC5)

        if(CY_SPC_CPU_WAITPIPE_BYPASS == spcWaitPipeBypass)
        {
            /* Force to bypass pipeline registers */
            CY_SPC_CPU_WAITPIPE_REG |= CY_SPC_CPU_WAITPIPE_BYPASS;

            /* At least 2 NOP instructions are recommended */
            CY_NOP;
            CY_NOP;
            CY_NOP;

            spcWaitPipeBypass = 0u;
        }

    #endif  /* (CY_PSOC5) */

    /* Exit critical section */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CySpcGetAlgorithm
********************************************************************************
* Summary:
*  Downloads SPC algorithm from SPC SROM into SRAM.
*
* Parameters:
*  None
*
* Return:
*  CYRET_STARTED
*  CYRET_LOCKED
*
*******************************************************************************/
cystatus CySpcGetAlgorithm(void)
{
    cystatus status = CYRET_STARTED;

    /* Make sure the SPC is ready to accept command */
    if(CY_SPC_IDLE)
    {
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_ONE;
        CY_SPC_CPU_DATA_REG = CY_SPC_KEY_TWO(CY_SPC_CMD_DWNLD_ALGORITHM);
        CY_SPC_CPU_DATA_REG = CY_SPC_CMD_DWNLD_ALGORITHM;
    }
    else
    {
        status = CYRET_LOCKED;
    }

    return(status);
}

/* [] END OF FILE */
