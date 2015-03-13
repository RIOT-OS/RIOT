/*******************************************************************************
* File Name: cypins.h
* Version 4.20
*
*  Description:
*   This file contains the function prototypes and constants used for a port/pin
*   in access and control.
*
*  Note:
*   Documentation of the API's in this file is located in the
*   System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYPINS_H)
#define CY_BOOT_CYPINS_H

#include "cyfitter.h"
#include "cytypes.h"


/**************************************
*        API Parameter Constants
**************************************/

#define CY_PINS_PC_DRIVE_MODE_SHIFT (0x01u)
#define CY_PINS_PC_DRIVE_MODE_MASK  ((uint8)(0x07u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_0     ((uint8)(0x00u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_1     ((uint8)(0x01u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_2     ((uint8)(0x02u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_3     ((uint8)(0x03u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_4     ((uint8)(0x04u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_5     ((uint8)(0x05u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_6     ((uint8)(0x06u << CY_PINS_PC_DRIVE_MODE_SHIFT))
#define CY_PINS_PC_DRIVE_MODE_7     ((uint8)(0x07u << CY_PINS_PC_DRIVE_MODE_SHIFT))


/*  SetPinDriveMode */
#define CY_PINS_DM_ALG_HIZ          (CY_PINS_PC_DRIVE_MODE_0)
#define CY_PINS_DM_DIG_HIZ          (CY_PINS_PC_DRIVE_MODE_1)
#define CY_PINS_DM_RES_UP           (CY_PINS_PC_DRIVE_MODE_2)
#define CY_PINS_DM_RES_DWN          (CY_PINS_PC_DRIVE_MODE_3)
#define CY_PINS_DM_OD_LO            (CY_PINS_PC_DRIVE_MODE_4)
#define CY_PINS_DM_OD_HI            (CY_PINS_PC_DRIVE_MODE_5)
#define CY_PINS_DM_STRONG           (CY_PINS_PC_DRIVE_MODE_6)
#define CY_PINS_DM_RES_UPDWN        (CY_PINS_PC_DRIVE_MODE_7)


/**************************************
*       Register Constants
**************************************/

/* Port Pin Configuration Register */
#define CY_PINS_PC_DATAOUT          (0x01u)
#define CY_PINS_PC_PIN_FASTSLEW     (0xBFu)
#define CY_PINS_PC_PIN_SLOWSLEW     (0x40u)
#define CY_PINS_PC_PIN_STATE        (0x10u)
#define CY_PINS_PC_BIDIR_EN         (0x20u)
#define CY_PINS_PC_SLEW             (0x40u)
#define CY_PINS_PC_BYPASS           (0x80u)


/**************************************
*       Pin API Macros
**************************************/

/*******************************************************************************
* Macro Name: CyPins_ReadPin
********************************************************************************
*
* Summary:
*  Reads the current value on the pin (pin state, PS).
*
* Parameters:
*   pinPC: Port pin configuration register (uint16).
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
* Return:
*   Pin state
*    0: Logic low value
*    Non-0: Logic high value
*
*******************************************************************************/
#define CyPins_ReadPin(pinPC)    ( *(reg8 *)(pinPC) & CY_PINS_PC_PIN_STATE )


/*******************************************************************************
* Macro Name: CyPins_SetPin
********************************************************************************
*
* Summary:
*  Set the output value for the pin (data register, DR) to a logic high.
*
*  Note that this only has an effect for pins configured as software pins that
*  are not driven by hardware.
*
*  The macro operation is not atomic. It is not guaranteed that shared register
*  will remain uncorrupted during simultaneous read-modify-write operations
*  performed by two threads (main and interrupt threads). To guarantee data
*  integrity in such cases, the macro should be invoked while the specific
*  interrupt is disabled or within critical section (all interrupts are
*  disabled).
*
* Parameters:
*   pinPC: Port pin configuration register (uint16).
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
* Return:
*   None
*
*******************************************************************************/
#define CyPins_SetPin(pinPC)     ( *(reg8 *)(pinPC) |= CY_PINS_PC_DATAOUT)


/*******************************************************************************
* Macro Name: CyPins_ClearPin
********************************************************************************
*
* Summary:
*  This macro sets the state of the specified pin to 0.
*
*  The macro operation is not atomic. It is not guaranteed that shared register
*  will remain uncorrupted during simultaneous read-modify-write operations
*  performed by two threads (main and interrupt threads). To guarantee data
*  integrity in such cases, the macro should be invoked while the specific
*  interrupt is disabled or within critical section (all interrupts are
*  disabled).
*
* Parameters:
*   pinPC: address of a Pin Configuration register.
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
* Return:
*   None
*
*******************************************************************************/
#define CyPins_ClearPin(pinPC)   ( *(reg8 *)(pinPC) &= ((uint8)(~CY_PINS_PC_DATAOUT)))


/*******************************************************************************
* Macro Name: CyPins_SetPinDriveMode
********************************************************************************
*
* Summary:
*  Sets the drive mode for the pin (DM).
*
*  The macro operation is not atomic. It is not guaranteed that shared register
*  will remain uncorrupted during simultaneous read-modify-write operations
*  performed by two threads (main and interrupt threads). To guarantee data
*  integrity in such cases, the macro should be invoked while the specific
*  interrupt is disabled or within critical section (all interrupts are
*  disabled).
*
* Parameters:
*   pinPC: Port pin configuration register (uint16)
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
*   mode: Desired drive mode
*
*   Define                Source
*   PIN_DM_ALG_HIZ        Analog HiZ
*   PIN_DM_DIG_HIZ        Digital HiZ
*   PIN_DM_RES_UP        Resistive pull up
*   PIN_DM_RES_DWN        Resistive pull down
*   PIN_DM_OD_LO        Open drain - drive low
*   PIN_DM_OD_HI        Open drain - drive high
*   PIN_DM_STRONG        Strong CMOS Output
*   PIN_DM_RES_UPDWN    Resistive pull up/down
*
* Return:
*   None
*
*******************************************************************************/
#define CyPins_SetPinDriveMode(pinPC, mode) \
            ( *(reg8 *)(pinPC) = (*(reg8 *)(pinPC) & ((uint8)(~CY_PINS_PC_DRIVE_MODE_MASK))) | \
            ((mode) & CY_PINS_PC_DRIVE_MODE_MASK))


/*******************************************************************************
* Macro Name: CyPins_ReadPinDriveMode
********************************************************************************
*
* Summary:
*  Reads the drive mode for the pin (DM).
*
* Parameters:
*   pinPC: Port pin configuration register (uint16)
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
*
* Return:
*   mode:  The current drive mode for the pin
*
*   Define                Source
*   PIN_DM_ALG_HIZ        Analog HiZ
*   PIN_DM_DIG_HIZ        Digital HiZ
*   PIN_DM_RES_UP        Resistive pull up
*   PIN_DM_RES_DWN        Resistive pull down
*   PIN_DM_OD_LO        Open drain - drive low
*   PIN_DM_OD_HI        Open drain - drive high
*   PIN_DM_STRONG        Strong CMOS Output
*   PIN_DM_RES_UPDWN    Resistive pull up/down
*
*******************************************************************************/
#define CyPins_ReadPinDriveMode(pinPC)      (*(reg8 *)(pinPC) & CY_PINS_PC_DRIVE_MODE_MASK)


/*******************************************************************************
* Macro Name: CyPins_FastSlew
********************************************************************************
*
* Summary:
*  Set the slew rate for the pin to fast the edge rate.
*  Note that this only applies for pins in strong output drive modes,
*  not to resistive drive modes.
*
*  The macro operation is not atomic. It is not guaranteed that shared register
*  will remain uncorrupted during simultaneous read-modify-write operations
*  performed by two threads (main and interrupt threads). To guarantee data
*  integrity in such cases, the macro should be invoked while the specific
*  interrupt is disabled or within critical section (all interrupts are
*  disabled).
*
* Parameters:
*   pinPC: address of a Pin Configuration register.
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
*
* Return:
*   None
*
*******************************************************************************/
#define CyPins_FastSlew(pinPC)      (*(reg8 *)(pinPC) = (*(reg8 *)(pinPC) & CY_PINS_PC_PIN_FASTSLEW))


/*******************************************************************************
* Macro Name: CyPins_SlowSlew
********************************************************************************
*
* Summary:
*  Set the slew rate for the pin to slow the edge rate.
*  Note that this only applies for pins in strong output drive modes,
*  not to resistive drive modes.
*
*  The macro operation is not atomic. It is not guaranteed that shared register
*  will remain uncorrupted during simultaneous read-modify-write operations
*  performed by two threads (main and interrupt threads). To guarantee data
*  integrity in such cases, the macro should be invoked while the specific
*  interrupt is disabled or within critical section (all interrupts are
*  disabled).
*
* Parameters:
*   pinPC: address of a Pin Configuration register.
*   #defines for each pin on a chip are provided in the cydevice_trm.h file
*   in the form:
*       CYREG_PRTx_PCy
*
*   where x is a port number 0 - 15 and y is a pin number 0 - 7
*
* Return:
*   None
*
*******************************************************************************/
#define CyPins_SlowSlew(pinPC)      (*(reg8 *)(pinPC) = (*(reg8 *)(pinPC) | CY_PINS_PC_PIN_SLOWSLEW))


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
#define PC_DRIVE_MODE_SHIFT (CY_PINS_PC_DRIVE_MODE_SHIFT)
#define PC_DRIVE_MODE_MASK  (CY_PINS_PC_DRIVE_MODE_MASK)
#define PC_DRIVE_MODE_0     (CY_PINS_PC_DRIVE_MODE_0)
#define PC_DRIVE_MODE_1     (CY_PINS_PC_DRIVE_MODE_1)
#define PC_DRIVE_MODE_2     (CY_PINS_PC_DRIVE_MODE_2)
#define PC_DRIVE_MODE_3     (CY_PINS_PC_DRIVE_MODE_3)
#define PC_DRIVE_MODE_4     (CY_PINS_PC_DRIVE_MODE_4)
#define PC_DRIVE_MODE_5     (CY_PINS_PC_DRIVE_MODE_5)
#define PC_DRIVE_MODE_6     (CY_PINS_PC_DRIVE_MODE_6)
#define PC_DRIVE_MODE_7     (CY_PINS_PC_DRIVE_MODE_7)

#define PIN_DM_ALG_HIZ      (CY_PINS_DM_ALG_HIZ)
#define PIN_DM_DIG_HIZ      (CY_PINS_DM_DIG_HIZ)
#define PIN_DM_RES_UP       (CY_PINS_DM_RES_UP)
#define PIN_DM_RES_DWN      (CY_PINS_DM_RES_DWN)
#define PIN_DM_OD_LO        (CY_PINS_DM_OD_LO)
#define PIN_DM_OD_HI        (CY_PINS_DM_OD_HI)
#define PIN_DM_STRONG       (CY_PINS_DM_STRONG)
#define PIN_DM_RES_UPDWN    (CY_PINS_DM_RES_UPDWN)

#define PC_DATAOUT          (CY_PINS_PC_DATAOUT)
#define PC_PIN_FASTSLEW     (CY_PINS_PC_PIN_FASTSLEW)
#define PC_PIN_SLOWSLEW     (CY_PINS_PC_PIN_SLOWSLEW)
#define PC_PIN_STATE        (CY_PINS_PC_PIN_STATE)
#define PC_BIDIR_EN         (CY_PINS_PC_BIDIR_EN)
#define PC_SLEW             (CY_PINS_PC_SLEW)
#define PC_BYPASS           (CY_PINS_PC_BYPASS)

#endif /* (CY_BOOT_CYPINS_H) */


/* [] END OF FILE */
