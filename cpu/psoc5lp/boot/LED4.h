/*******************************************************************************
* File Name: LED4.h
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

#if !defined(CY_PINS_LED4_H) /* Pins LED4_H */
#define CY_PINS_LED4_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LED4_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LED4__PORT == 15 && ((LED4__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes
***************************************/

void    LED4_Write(uint8 value) ;
void    LED4_SetDriveMode(uint8 mode) ;
uint8   LED4_ReadDataReg(void) ;
uint8   LED4_Read(void) ;
uint8   LED4_ClearInterrupt(void) ;


/***************************************
*           API Constants
***************************************/

/* Drive Modes */
#define LED4_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define LED4_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define LED4_DM_RES_UP          PIN_DM_RES_UP
#define LED4_DM_RES_DWN         PIN_DM_RES_DWN
#define LED4_DM_OD_LO           PIN_DM_OD_LO
#define LED4_DM_OD_HI           PIN_DM_OD_HI
#define LED4_DM_STRONG          PIN_DM_STRONG
#define LED4_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define LED4_MASK               LED4__MASK
#define LED4_SHIFT              LED4__SHIFT
#define LED4_WIDTH              1u


/***************************************
*             Registers
***************************************/

/* Main Port Registers */
/* Pin State */
#define LED4_PS                     (* (reg8 *) LED4__PS)
/* Data Register */
#define LED4_DR                     (* (reg8 *) LED4__DR)
/* Port Number */
#define LED4_PRT_NUM                (* (reg8 *) LED4__PRT)
/* Connect to Analog Globals */
#define LED4_AG                     (* (reg8 *) LED4__AG)
/* Analog MUX bux enable */
#define LED4_AMUX                   (* (reg8 *) LED4__AMUX)
/* Bidirectional Enable */
#define LED4_BIE                    (* (reg8 *) LED4__BIE)
/* Bit-mask for Aliased Register Access */
#define LED4_BIT_MASK               (* (reg8 *) LED4__BIT_MASK)
/* Bypass Enable */
#define LED4_BYP                    (* (reg8 *) LED4__BYP)
/* Port wide control signals */
#define LED4_CTL                    (* (reg8 *) LED4__CTL)
/* Drive Modes */
#define LED4_DM0                    (* (reg8 *) LED4__DM0)
#define LED4_DM1                    (* (reg8 *) LED4__DM1)
#define LED4_DM2                    (* (reg8 *) LED4__DM2)
/* Input Buffer Disable Override */
#define LED4_INP_DIS                (* (reg8 *) LED4__INP_DIS)
/* LCD Common or Segment Drive */
#define LED4_LCD_COM_SEG            (* (reg8 *) LED4__LCD_COM_SEG)
/* Enable Segment LCD */
#define LED4_LCD_EN                 (* (reg8 *) LED4__LCD_EN)
/* Slew Rate Control */
#define LED4_SLW                    (* (reg8 *) LED4__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LED4_PRTDSI__CAPS_SEL       (* (reg8 *) LED4__PRTDSI__CAPS_SEL)
/* Double Sync Enable */
#define LED4_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LED4__PRTDSI__DBL_SYNC_IN)
/* Output Enable Select Drive Strength */
#define LED4_PRTDSI__OE_SEL0        (* (reg8 *) LED4__PRTDSI__OE_SEL0)
#define LED4_PRTDSI__OE_SEL1        (* (reg8 *) LED4__PRTDSI__OE_SEL1)
/* Port Pin Output Select Registers */
#define LED4_PRTDSI__OUT_SEL0       (* (reg8 *) LED4__PRTDSI__OUT_SEL0)
#define LED4_PRTDSI__OUT_SEL1       (* (reg8 *) LED4__PRTDSI__OUT_SEL1)
/* Sync Output Enable Registers */
#define LED4_PRTDSI__SYNC_OUT       (* (reg8 *) LED4__PRTDSI__SYNC_OUT)


#if defined(LED4__INTSTAT)  /* Interrupt Registers */

    #define LED4_INTSTAT                (* (reg8 *) LED4__INTSTAT)
    #define LED4_SNAP                   (* (reg8 *) LED4__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LED4_H */


/* [] END OF FILE */
