/*******************************************************************************
* File Name: CharacterLCD_LCDPort.h
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_CharacterLCD_LCDPort_H) /* Pins CharacterLCD_LCDPort_H */
#define CY_PINS_CharacterLCD_LCDPort_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CharacterLCD_LCDPort_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CharacterLCD_LCDPort__PORT == 15 && ((CharacterLCD_LCDPort__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes
***************************************/

void    CharacterLCD_LCDPort_Write(uint8 value) ;
void    CharacterLCD_LCDPort_SetDriveMode(uint8 mode) ;
uint8   CharacterLCD_LCDPort_ReadDataReg(void) ;
uint8   CharacterLCD_LCDPort_Read(void) ;
uint8   CharacterLCD_LCDPort_ClearInterrupt(void) ;


/***************************************
*           API Constants
***************************************/

/* Drive Modes */
#define CharacterLCD_LCDPort_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define CharacterLCD_LCDPort_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define CharacterLCD_LCDPort_DM_RES_UP          PIN_DM_RES_UP
#define CharacterLCD_LCDPort_DM_RES_DWN         PIN_DM_RES_DWN
#define CharacterLCD_LCDPort_DM_OD_LO           PIN_DM_OD_LO
#define CharacterLCD_LCDPort_DM_OD_HI           PIN_DM_OD_HI
#define CharacterLCD_LCDPort_DM_STRONG          PIN_DM_STRONG
#define CharacterLCD_LCDPort_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define CharacterLCD_LCDPort_MASK               CharacterLCD_LCDPort__MASK
#define CharacterLCD_LCDPort_SHIFT              CharacterLCD_LCDPort__SHIFT
#define CharacterLCD_LCDPort_WIDTH              7u


/***************************************
*             Registers
***************************************/

/* Main Port Registers */
/* Pin State */
#define CharacterLCD_LCDPort_PS                     (* (reg8 *) CharacterLCD_LCDPort__PS)
/* Data Register */
#define CharacterLCD_LCDPort_DR                     (* (reg8 *) CharacterLCD_LCDPort__DR)
/* Port Number */
#define CharacterLCD_LCDPort_PRT_NUM                (* (reg8 *) CharacterLCD_LCDPort__PRT)
/* Connect to Analog Globals */
#define CharacterLCD_LCDPort_AG                     (* (reg8 *) CharacterLCD_LCDPort__AG)
/* Analog MUX bux enable */
#define CharacterLCD_LCDPort_AMUX                   (* (reg8 *) CharacterLCD_LCDPort__AMUX)
/* Bidirectional Enable */
#define CharacterLCD_LCDPort_BIE                    (* (reg8 *) CharacterLCD_LCDPort__BIE)
/* Bit-mask for Aliased Register Access */
#define CharacterLCD_LCDPort_BIT_MASK               (* (reg8 *) CharacterLCD_LCDPort__BIT_MASK)
/* Bypass Enable */
#define CharacterLCD_LCDPort_BYP                    (* (reg8 *) CharacterLCD_LCDPort__BYP)
/* Port wide control signals */
#define CharacterLCD_LCDPort_CTL                    (* (reg8 *) CharacterLCD_LCDPort__CTL)
/* Drive Modes */
#define CharacterLCD_LCDPort_DM0                    (* (reg8 *) CharacterLCD_LCDPort__DM0)
#define CharacterLCD_LCDPort_DM1                    (* (reg8 *) CharacterLCD_LCDPort__DM1)
#define CharacterLCD_LCDPort_DM2                    (* (reg8 *) CharacterLCD_LCDPort__DM2)
/* Input Buffer Disable Override */
#define CharacterLCD_LCDPort_INP_DIS                (* (reg8 *) CharacterLCD_LCDPort__INP_DIS)
/* LCD Common or Segment Drive */
#define CharacterLCD_LCDPort_LCD_COM_SEG            (* (reg8 *) CharacterLCD_LCDPort__LCD_COM_SEG)
/* Enable Segment LCD */
#define CharacterLCD_LCDPort_LCD_EN                 (* (reg8 *) CharacterLCD_LCDPort__LCD_EN)
/* Slew Rate Control */
#define CharacterLCD_LCDPort_SLW                    (* (reg8 *) CharacterLCD_LCDPort__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CharacterLCD_LCDPort_PRTDSI__CAPS_SEL       (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__CAPS_SEL)
/* Double Sync Enable */
#define CharacterLCD_LCDPort_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__DBL_SYNC_IN)
/* Output Enable Select Drive Strength */
#define CharacterLCD_LCDPort_PRTDSI__OE_SEL0        (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__OE_SEL0)
#define CharacterLCD_LCDPort_PRTDSI__OE_SEL1        (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__OE_SEL1)
/* Port Pin Output Select Registers */
#define CharacterLCD_LCDPort_PRTDSI__OUT_SEL0       (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__OUT_SEL0)
#define CharacterLCD_LCDPort_PRTDSI__OUT_SEL1       (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__OUT_SEL1)
/* Sync Output Enable Registers */
#define CharacterLCD_LCDPort_PRTDSI__SYNC_OUT       (* (reg8 *) CharacterLCD_LCDPort__PRTDSI__SYNC_OUT)


#if defined(CharacterLCD_LCDPort__INTSTAT)  /* Interrupt Registers */

    #define CharacterLCD_LCDPort_INTSTAT                (* (reg8 *) CharacterLCD_LCDPort__INTSTAT)
    #define CharacterLCD_LCDPort_SNAP                   (* (reg8 *) CharacterLCD_LCDPort__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CharacterLCD_LCDPort_H */


/* [] END OF FILE */
