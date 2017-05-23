/*******************************************************************************
* File Name: TIMER0_IRQn.h
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
#if !defined(CY_ISR_TIMER0_IRQn_H)
#define CY_ISR_TIMER0_IRQn_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void TIMER0_IRQn_Start(void);
void TIMER0_IRQn_StartEx(cyisraddress address);
void TIMER0_IRQn_Stop(void);

CY_ISR_PROTO(TIMER0_IRQn_Interrupt);

void TIMER0_IRQn_SetVector(cyisraddress address);
cyisraddress TIMER0_IRQn_GetVector(void);

void TIMER0_IRQn_SetPriority(uint8 priority);
uint8 TIMER0_IRQn_GetPriority(void);

void TIMER0_IRQn_Enable(void);
uint8 TIMER0_IRQn_GetState(void);
void TIMER0_IRQn_Disable(void);

void TIMER0_IRQn_SetPending(void);
void TIMER0_IRQn_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the TIMER0_IRQn ISR. */
#define TIMER0_IRQn_INTC_VECTOR            ((reg32 *) TIMER0_IRQn__INTC_VECT)

/* Address of the TIMER0_IRQn ISR priority. */
#define TIMER0_IRQn_INTC_PRIOR             ((reg8 *) TIMER0_IRQn__INTC_PRIOR_REG)

/* Priority of the TIMER0_IRQn interrupt. */
#define TIMER0_IRQn_INTC_PRIOR_NUMBER      TIMER0_IRQn__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable TIMER0_IRQn interrupt. */
#define TIMER0_IRQn_INTC_SET_EN            ((reg32 *) TIMER0_IRQn__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the TIMER0_IRQn interrupt. */
#define TIMER0_IRQn_INTC_CLR_EN            ((reg32 *) TIMER0_IRQn__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the TIMER0_IRQn interrupt state to pending. */
#define TIMER0_IRQn_INTC_SET_PD            ((reg32 *) TIMER0_IRQn__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the TIMER0_IRQn interrupt. */
#define TIMER0_IRQn_INTC_CLR_PD            ((reg32 *) TIMER0_IRQn__INTC_CLR_PD_REG)


#endif /* CY_ISR_TIMER0_IRQn_H */


/* [] END OF FILE */
