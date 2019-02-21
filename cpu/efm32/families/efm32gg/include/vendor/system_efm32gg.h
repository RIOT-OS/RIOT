/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M3 System Layer for EFM32GG devices.
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

#ifndef SYSTEM_EFM32GG_H
#define SYSTEM_EFM32GG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFM32GG EFM32GG
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

extern uint32_t SystemCoreClock;    /**< System Clock Frequency (Core Clock) */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/* Interrupt routines - prototypes */
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

void DMA_IRQHandler(void);          /**< DMA IRQ Handler */
void GPIO_EVEN_IRQHandler(void);    /**< GPIO EVEN IRQ Handler */
void TIMER0_IRQHandler(void);       /**< TIMER0 IRQ Handler */
void USART0_RX_IRQHandler(void);    /**< USART0 RX IRQ Handler */
void USART0_TX_IRQHandler(void);    /**< USART0 TX IRQ Handler */
void USB_IRQHandler(void);          /**< USB IRQ Handler */
void ACMP0_IRQHandler(void);        /**< ACMP0 IRQ Handler */
void ADC0_IRQHandler(void);         /**< ADC0 IRQ Handler */
void DAC0_IRQHandler(void);         /**< DAC0 IRQ Handler */
void I2C0_IRQHandler(void);         /**< I2C0 IRQ Handler */
void I2C1_IRQHandler(void);         /**< I2C1 IRQ Handler */
void GPIO_ODD_IRQHandler(void);     /**< GPIO ODD IRQ Handler */
void TIMER1_IRQHandler(void);       /**< TIMER1 IRQ Handler */
void TIMER2_IRQHandler(void);       /**< TIMER2 IRQ Handler */
void TIMER3_IRQHandler(void);       /**< TIMER3 IRQ Handler */
void USART1_RX_IRQHandler(void);    /**< USART1 RX IRQ Handler */
void USART1_TX_IRQHandler(void);    /**< USART1 TX IRQ Handler */
void LESENSE_IRQHandler(void);      /**< LESENSE IRQ Handler */
void USART2_RX_IRQHandler(void);    /**< USART2 RX IRQ Handler */
void USART2_TX_IRQHandler(void);    /**< USART2 TX IRQ Handler */
void UART0_RX_IRQHandler(void);     /**< UART0 RX IRQ Handler */
void UART0_TX_IRQHandler(void);     /**< UART0 TX IRQ Handler */
void UART1_RX_IRQHandler(void);     /**< UART1 RX IRQ Handler */
void UART1_TX_IRQHandler(void);     /**< UART1 TX IRQ Handler */
void LEUART0_IRQHandler(void);      /**< LEUART0 IRQ Handler */
void LEUART1_IRQHandler(void);      /**< LEUART1 IRQ Handler */
void LETIMER0_IRQHandler(void);     /**< LETIMER0 IRQ Handler */
void PCNT0_IRQHandler(void);        /**< PCNT0 IRQ Handler */
void PCNT1_IRQHandler(void);        /**< PCNT1 IRQ Handler */
void PCNT2_IRQHandler(void);        /**< PCNT2 IRQ Handler */
void RTC_IRQHandler(void);          /**< RTC IRQ Handler */
void BURTC_IRQHandler(void);        /**< BURTC IRQ Handler */
void CMU_IRQHandler(void);          /**< CMU IRQ Handler */
void VCMP_IRQHandler(void);         /**< VCMP IRQ Handler */
void LCD_IRQHandler(void);          /**< LCD IRQ Handler */
void MSC_IRQHandler(void);          /**< MSC IRQ Handler */
void AES_IRQHandler(void);          /**< AES IRQ Handler */
void EBI_IRQHandler(void);          /**< EBI IRQ Handler */
void EMU_IRQHandler(void);          /**< EMU IRQ Handler */

uint32_t SystemCoreClockGet(void);
uint32_t SystemMaxCoreClockGet(void);

/***************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that if only changing core clock frequency through the EFM32 CMU
 *   API, this variable will be kept updated. This function is only provided
 *   for CMSIS compliance and if a user modifies the the core clock outside
 *   the CMU API.
 ******************************************************************************/
static __INLINE void SystemCoreClockUpdate(void)
{
  (void)SystemCoreClockGet();
}

void SystemInit(void);
uint32_t SystemHFClockGet(void);
uint32_t SystemHFXOClockGet(void);
void SystemHFXOClockSet(uint32_t freq);
uint32_t SystemLFRCOClockGet(void);
uint32_t SystemULFRCOClockGet(void);
uint32_t SystemLFXOClockGet(void);
void SystemLFXOClockSet(uint32_t freq);

/** @} End of group EFM32GG */
/** @} End of group Parts */

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_EFM32GG_H */
