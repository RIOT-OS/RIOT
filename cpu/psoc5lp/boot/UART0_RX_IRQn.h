/*******************************************************************************
* File Name: UART0_RX_IRQn.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_UART0_RX_IRQn_H)
#define CY_ISR_UART0_RX_IRQn_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void UART0_RX_IRQn_Start(void);
void UART0_RX_IRQn_StartEx(cyisraddress address);
void UART0_RX_IRQn_Stop(void);

CY_ISR_PROTO(UART0_RX_IRQn_Interrupt);

void UART0_RX_IRQn_SetVector(cyisraddress address);
cyisraddress UART0_RX_IRQn_GetVector(void);

void UART0_RX_IRQn_SetPriority(uint8 priority);
uint8 UART0_RX_IRQn_GetPriority(void);

void UART0_RX_IRQn_Enable(void);
uint8 UART0_RX_IRQn_GetState(void);
void UART0_RX_IRQn_Disable(void);

void UART0_RX_IRQn_SetPending(void);
void UART0_RX_IRQn_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the UART0_RX_IRQn ISR. */
#define UART0_RX_IRQn_INTC_VECTOR            ((reg32 *) UART0_RX_IRQn__INTC_VECT)

/* Address of the UART0_RX_IRQn ISR priority. */
#define UART0_RX_IRQn_INTC_PRIOR             ((reg8 *) UART0_RX_IRQn__INTC_PRIOR_REG)

/* Priority of the UART0_RX_IRQn interrupt. */
#define UART0_RX_IRQn_INTC_PRIOR_NUMBER      UART0_RX_IRQn__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable UART0_RX_IRQn interrupt. */
#define UART0_RX_IRQn_INTC_SET_EN            ((reg32 *) UART0_RX_IRQn__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the UART0_RX_IRQn interrupt. */
#define UART0_RX_IRQn_INTC_CLR_EN            ((reg32 *) UART0_RX_IRQn__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the UART0_RX_IRQn interrupt state to pending. */
#define UART0_RX_IRQn_INTC_SET_PD            ((reg32 *) UART0_RX_IRQn__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the UART0_RX_IRQn interrupt. */
#define UART0_RX_IRQn_INTC_CLR_PD            ((reg32 *) UART0_RX_IRQn__INTC_CLR_PD_REG)


#endif /* CY_ISR_UART0_RX_IRQn_H */


/* [] END OF FILE */
