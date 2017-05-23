/*******************************************************************************
* File Name: CySpc.c
* Version 4.20
*
* Description:
*  Provides definitions for the System Performance Component API.
*  The SPC functions are not meant to be called directly by the user
*  application.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYSPC_H)
#define CY_BOOT_CYSPC_H

#include "cytypes.h"
#include "CyLib.h"
#include "cydevice_trm.h"


/***************************************
*    Global Variables
***************************************/
extern uint8 SpcLockState;


/***************************************
*    Function Prototypes
***************************************/
void     CySpcStart(void);
void     CySpcStop(void);
uint8    CySpcReadData(uint8 buffer[], uint8 size);
cystatus CySpcLoadMultiByte(uint8 array, uint16 address, const uint8 buffer[], uint8 size)\
;
cystatus CySpcLoadRow(uint8 array, const uint8 buffer[], uint16 size);
cystatus CySpcLoadRowFull(uint8 array, uint16 row, const uint8 buffer[], uint16 size)\
;
cystatus CySpcWriteRow(uint8 array, uint16 address, uint8 tempPolarity, uint8 tempMagnitude)\
;
cystatus CySpcEraseSector(uint8 array, uint8 sectorNumber);
cystatus CySpcGetTemp(uint8 numSamples);
cystatus CySpcGetAlgorithm(void);
cystatus CySpcLock(void);
void     CySpcUnlock(void);


/***************************************
*    API Constants
***************************************/

#define CY_SPC_LOCKED                       (0x01u)
#define CY_SPC_UNLOCKED                     (0x00u)

/*******************************************************************************
* The Array ID indicates the unique ID of the SONOS array being accessed:
* - 0x00-0x3E : Flash Arrays
* - 0x3F      : Selects all Flash arrays simultaneously
* - 0x40-0x7F : Embedded EEPROM Arrays
*******************************************************************************/
#define CY_SPC_FIRST_FLASH_ARRAYID          (0x00u)
#define CY_SPC_LAST_FLASH_ARRAYID           (0x3Fu)
#define CY_SPC_FIRST_EE_ARRAYID             (0x40u)
#define CY_SPC_LAST_EE_ARRAYID              (0x7Fu)


#define CY_SPC_STATUS_DATA_READY_MASK       (0x01u)
#define CY_SPC_STATUS_IDLE_MASK             (0x02u)
#define CY_SPC_STATUS_CODE_MASK             (0xFCu)
#define CY_SPC_STATUS_CODE_SHIFT            (0x02u)

/* Status codes for SPC. */
#define CY_SPC_STATUS_SUCCESS               (0x00u)   /* Operation Successful */
#define CY_SPC_STATUS_INVALID_ARRAY_ID      (0x01u)   /* Invalid Array ID for given command */
#define CY_SPC_STATUS_INVALID_2BYTEKEY      (0x02u)   /* Invalid 2-byte key */
#define CY_SPC_STATUS_ARRAY_ASLEEP          (0x03u)   /* Addressed Array is Asleep */
#define CY_SPC_STATUS_EXTERN_ACCESS         (0x04u)   /* External Access Failure (SPC is not in external access mode) */
#define CY_SPC_STATUS_INVALID_NUMBER        (0x05u)   /* Invalid 'N' Value for given command */
#define CY_SPC_STATUS_TEST_MODE             (0x06u)   /* Test Mode Failure (SPC is not in test mode) */
#define CY_SPC_STATUS_ALG_CSUM              (0x07u)   /* Smart Write Algorithm Checksum Failure */
#define CY_SPC_STATUS_PARAM_CSUM            (0x08u)   /* Smart Write Parameter Checksum Failure */
#define CY_SPC_STATUS_PROTECTION            (0x09u)   /* Protection Check Failure */
#define CY_SPC_STATUS_ADDRESS_PARAM         (0x0Au)   /* Invalid Address parameter for the given command */
#define CY_SPC_STATUS_COMMAND_CODE          (0x0Bu)   /* Invalid Command Code */
#define CY_SPC_STATUS_ROW_ID                (0x0Cu)   /* Invalid Row ID parameter for given command */
#define CY_SPC_STATUS_TADC_INPUT            (0x0Du)   /* Invalid input value for Get Temp & Get ADC commands */
#define CY_SPC_STATUS_BUSY                  (0xFFu)   /* SPC is busy */

#if(CY_PSOC5)

    /* Wait-state pipeline */
    #define CY_SPC_CPU_WAITPIPE_BYPASS      ((uint32)0x01u)

#endif  /* (CY_PSOC5) */


/***************************************
* Registers
***************************************/

/* SPC CPU Data Register */
#define CY_SPC_CPU_DATA_REG         (* (reg8 *) CYREG_SPC_CPU_DATA )
#define CY_SPC_CPU_DATA_PTR         (  (reg8 *) CYREG_SPC_CPU_DATA )

/* SPC Status Register */
#define CY_SPC_STATUS_REG           (* (reg8 *) CYREG_SPC_SR )
#define CY_SPC_STATUS_PTR           (  (reg8 *) CYREG_SPC_SR )

/* Active Power Mode Configuration Register 0 */
#define CY_SPC_PM_ACT_REG           (* (reg8 *) CYREG_PM_ACT_CFG0 )
#define CY_SPC_PM_ACT_PTR           (  (reg8 *) CYREG_PM_ACT_CFG0 )

/* Standby Power Mode Configuration Register 0 */
#define CY_SPC_PM_STBY_REG          (* (reg8 *) CYREG_PM_STBY_CFG0 )
#define CY_SPC_PM_STBY_PTR          (  (reg8 *) CYREG_PM_STBY_CFG0 )

#if(CY_PSOC5)

    /* Wait State Pipeline */
    #define CY_SPC_CPU_WAITPIPE_REG     (* (reg32 *) CYREG_PANTHER_WAITPIPE )
    #define CY_SPC_CPU_WAITPIPE_PTR     (  (reg32 *) CYREG_PANTHER_WAITPIPE )

#endif  /* (CY_PSOC5) */


/***************************************
* Macros
***************************************/
#define CY_SPC_IDLE                 (0u != (CY_SPC_STATUS_REG & CY_SPC_STATUS_IDLE_MASK))
#define CY_SPC_BUSY                 (0u == (CY_SPC_STATUS_REG & CY_SPC_STATUS_IDLE_MASK))
#define CY_SPC_DATA_READY           (0u != (CY_SPC_STATUS_REG & CY_SPC_STATUS_DATA_READY_MASK))

/* SPC must be in idle state in order to obtain correct status */
#define CY_SPC_READ_STATUS          (CY_SPC_IDLE ? \
                                     ((uint8)(CY_SPC_STATUS_REG >> CY_SPC_STATUS_CODE_SHIFT)) : \
                                     ((uint8) CY_SPC_STATUS_BUSY))


/*******************************************************************************
* The following code is OBSOLETE and must not be used.
*
* If the obsoleted macro definitions intended for use in the application use the
* following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#define FIRST_FLASH_ARRAYID         (CY_SPC_FIRST_FLASH_ARRAYID)
#define LAST_FLASH_ARRAYID          (CY_SPC_LAST_FLASH_ARRAYID)
#define FIRST_EE_ARRAYID            (CY_SPC_FIRST_EE_ARRAYID)
#define LAST_EE_ARRAYID             (CY_SPC_LAST_EE_ARRAYID)
#define SIZEOF_ECC_ROW              (CYDEV_ECC_ROW_SIZE)
#define SIZEOF_FLASH_ROW            (CYDEV_FLS_ROW_SIZE)
#define SIZEOF_EEPROM_ROW           (CYDEV_EEPROM_ROW_SIZE)


#endif /* (CY_BOOT_CYSPC_H) */


/* [] END OF FILE */
