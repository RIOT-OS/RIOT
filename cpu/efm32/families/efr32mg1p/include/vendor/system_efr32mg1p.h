/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M3/M4 System Layer for EFR32 devices.
 * @version 5.7.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SYSTEM_EFR32_H
#define SYSTEM_EFR32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFR32 EFR32
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

extern uint32_t SystemCoreClock;        /**< System Clock Frequency (Core Clock) */
extern uint32_t SystemHfrcoFreq;        /**< System HFRCO frequency */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void Reset_Handler(void);           /**< Reset Handler */
void NMI_Handler(void);             /**< NMI Handler */
void HardFault_Handler(void);       /**< Hard Fault Handler */
void MemManage_Handler(void);       /**< MPU Fault Handler */
void BusFault_Handler(void);        /**< Bus Fault Handler */
void UsageFault_Handler(void);      /**< Usage Fault Handler */
void SVC_Handler(void);             /**< SVCall Handler */
void DebugMon_Handler(void);        /**< Debug Monitor Handler */
void PendSV_Handler(void);          /**< PendSV Handler */
void SysTick_Handler(void);         /**< SysTick Handler */

void EMU_IRQHandler(void);          /**< EMU IRQ Handler */
void FRC_PRI_IRQHandler(void);      /**< FRC_PRI IRQ Handler */
void WDOG0_IRQHandler(void);        /**< WDOG0 IRQ Handler */
void FRC_IRQHandler(void);          /**< FRC IRQ Handler */
void MODEM_IRQHandler(void);        /**< MODEM IRQ Handler */
void RAC_SEQ_IRQHandler(void);      /**< RAC_SEQ IRQ Handler */
void RAC_RSM_IRQHandler(void);      /**< RAC_RSM IRQ Handler */
void BUFC_IRQHandler(void);         /**< BUFC IRQ Handler */
void LDMA_IRQHandler(void);         /**< LDMA IRQ Handler */
void GPIO_EVEN_IRQHandler(void);    /**< GPIO_EVEN IRQ Handler */
void TIMER0_IRQHandler(void);       /**< TIMER0 IRQ Handler */
void USART0_RX_IRQHandler(void);    /**< USART0_RX IRQ Handler */
void USART0_TX_IRQHandler(void);    /**< USART0_TX IRQ Handler */
void ACMP0_IRQHandler(void);        /**< ACMP0 IRQ Handler */
void ADC0_IRQHandler(void);         /**< ADC0 IRQ Handler */
void IDAC0_IRQHandler(void);        /**< IDAC0 IRQ Handler */
void I2C0_IRQHandler(void);         /**< I2C0 IRQ Handler */
void GPIO_ODD_IRQHandler(void);     /**< GPIO_ODD IRQ Handler */
void TIMER1_IRQHandler(void);       /**< TIMER1 IRQ Handler */
void USART1_RX_IRQHandler(void);    /**< USART1_RX IRQ Handler */
void USART1_TX_IRQHandler(void);    /**< USART1_TX IRQ Handler */
void LEUART0_IRQHandler(void);      /**< LEUART0 IRQ Handler */
void PCNT0_IRQHandler(void);        /**< PCNT0 IRQ Handler */
void CMU_IRQHandler(void);          /**< CMU IRQ Handler */
void MSC_IRQHandler(void);          /**< MSC IRQ Handler */
void CRYPTO_IRQHandler(void);       /**< CRYPTO IRQ Handler */
void LETIMER0_IRQHandler(void);     /**< LETIMER0 IRQ Handler */
void AGC_IRQHandler(void);          /**< AGC IRQ Handler */
void PROTIMER_IRQHandler(void);     /**< PROTIMER IRQ Handler */
void RTCC_IRQHandler(void);         /**< RTCC IRQ Handler */
void SYNTH_IRQHandler(void);        /**< SYNTH IRQ Handler */
void CRYOTIMER_IRQHandler(void);    /**< CRYOTIMER IRQ Handler */
void RFSENSE_IRQHandler(void);      /**< RFSENSE IRQ Handler */

#if (__FPU_PRESENT == 1)
void FPUEH_IRQHandler(void);        /**< FPUEH IRQ Handler */
#endif

uint32_t SystemCoreClockGet(void);

/***************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EFR CMU
 *   API, this variable will be kept updated. This function is only provided
 *   for CMSIS compliance and if a user modifies the the core clock outside
 *   the CMU API.
 ******************************************************************************/
static __INLINE void SystemCoreClockUpdate(void)
{
  (void)SystemCoreClockGet();
}

uint32_t SystemMaxCoreClockGet(void);

void SystemInit(void);
uint32_t SystemHFClockGet(void);

uint32_t SystemHFXOClockGet(void);
void SystemHFXOClockSet(uint32_t freq);

uint32_t SystemLFRCOClockGet(void);
uint32_t SystemULFRCOClockGet(void);

uint32_t SystemLFXOClockGet(void);
void SystemLFXOClockSet(uint32_t freq);

/** @} End of group */
/** @} End of group Parts */

#ifdef __cplusplus
}
#endif
#endif /* SYSTEM_EFR32_H */
