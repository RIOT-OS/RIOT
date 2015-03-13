/*******************************************************************************
* File Name: CLK_UART.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_CLK_UART_H)
#define CY_CLOCK_CLK_UART_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void CLK_UART_Start(void) ;
void CLK_UART_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void CLK_UART_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void CLK_UART_StandbyPower(uint8 state) ;
void CLK_UART_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 CLK_UART_GetDividerRegister(void) ;
void CLK_UART_SetModeRegister(uint8 modeBitMask) ;
void CLK_UART_ClearModeRegister(uint8 modeBitMask) ;
uint8 CLK_UART_GetModeRegister(void) ;
void CLK_UART_SetSourceRegister(uint8 clkSource) ;
uint8 CLK_UART_GetSourceRegister(void) ;
#if defined(CLK_UART__CFG3)
void CLK_UART_SetPhaseRegister(uint8 clkPhase) ;
uint8 CLK_UART_GetPhaseRegister(void) ;
#endif /* defined(CLK_UART__CFG3) */

#define CLK_UART_Enable()                       CLK_UART_Start()
#define CLK_UART_Disable()                      CLK_UART_Stop()
#define CLK_UART_SetDivider(clkDivider)         CLK_UART_SetDividerRegister(clkDivider, 1u)
#define CLK_UART_SetDividerValue(clkDivider)    CLK_UART_SetDividerRegister((clkDivider) - 1u, 1u)
#define CLK_UART_SetMode(clkMode)               CLK_UART_SetModeRegister(clkMode)
#define CLK_UART_SetSource(clkSource)           CLK_UART_SetSourceRegister(clkSource)
#if defined(CLK_UART__CFG3)
#define CLK_UART_SetPhase(clkPhase)             CLK_UART_SetPhaseRegister(clkPhase)
#define CLK_UART_SetPhaseValue(clkPhase)        CLK_UART_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(CLK_UART__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define CLK_UART_CLKEN              (* (reg8 *) CLK_UART__PM_ACT_CFG)
#define CLK_UART_CLKEN_PTR          ((reg8 *) CLK_UART__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define CLK_UART_CLKSTBY            (* (reg8 *) CLK_UART__PM_STBY_CFG)
#define CLK_UART_CLKSTBY_PTR        ((reg8 *) CLK_UART__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define CLK_UART_DIV_LSB            (* (reg8 *) CLK_UART__CFG0)
#define CLK_UART_DIV_LSB_PTR        ((reg8 *) CLK_UART__CFG0)
#define CLK_UART_DIV_PTR            ((reg16 *) CLK_UART__CFG0)

/* Clock MSB divider configuration register. */
#define CLK_UART_DIV_MSB            (* (reg8 *) CLK_UART__CFG1)
#define CLK_UART_DIV_MSB_PTR        ((reg8 *) CLK_UART__CFG1)

/* Mode and source configuration register */
#define CLK_UART_MOD_SRC            (* (reg8 *) CLK_UART__CFG2)
#define CLK_UART_MOD_SRC_PTR        ((reg8 *) CLK_UART__CFG2)

#if defined(CLK_UART__CFG3)
/* Analog clock phase configuration register */
#define CLK_UART_PHASE              (* (reg8 *) CLK_UART__CFG3)
#define CLK_UART_PHASE_PTR          ((reg8 *) CLK_UART__CFG3)
#endif /* defined(CLK_UART__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define CLK_UART_CLKEN_MASK         CLK_UART__PM_ACT_MSK
#define CLK_UART_CLKSTBY_MASK       CLK_UART__PM_STBY_MSK

/* CFG2 field masks */
#define CLK_UART_SRC_SEL_MSK        CLK_UART__CFG2_SRC_SEL_MASK
#define CLK_UART_MODE_MASK          (~(CLK_UART_SRC_SEL_MSK))

#if defined(CLK_UART__CFG3)
/* CFG3 phase mask */
#define CLK_UART_PHASE_MASK         CLK_UART__CFG3_PHASE_DLY_MASK
#endif /* defined(CLK_UART__CFG3) */

#endif /* CY_CLOCK_CLK_UART_H */


/* [] END OF FILE */
