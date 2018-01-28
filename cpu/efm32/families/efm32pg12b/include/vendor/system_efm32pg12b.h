/***************************************************************************//**
 * @file system_efm32pg12b.h
 * @brief CMSIS Cortex-M3/M4 System Layer for EFM32 devices.
 * @version 5.3.3
 ******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef SYSTEM_EFM32_H
#define SYSTEM_EFM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**************************************************************************//**
 * @addtogroup Parts
 * @{
 *****************************************************************************/
/**************************************************************************//**
 * @addtogroup EFM32 EFM32
 * @{
 *****************************************************************************/

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
void WDOG0_IRQHandler(void);        /**< WDOG0 IRQ Handler */
void WDOG1_IRQHandler(void);        /**< WDOG1 IRQ Handler */
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
void CRYPTO0_IRQHandler(void);      /**< CRYPTO IRQ Handler */
void LETIMER0_IRQHandler(void);     /**< LETIMER0 IRQ Handler */
void RTCC_IRQHandler(void);         /**< RTCC IRQ Handler */
void CRYOTIMER_IRQHandler(void);    /**< CRYOTIMER IRQ Handler */
void SMU_IRQHandler(void);          /**< SMU IRQ Handler */
void WTIMER0_IRQHandler(void);      /**< WTIMER0 IRQ Handler */
void WTIMER1_IRQHandler(void);      /**< WTIMER1 IRQ Handler */
void PCNT1_IRQHandler(void);        /**< PCNT1 IRQ Handler */
void PCNT2_IRQHandler(void);        /**< PCNT2 IRQ Handler */
void USART2_RX_IRQHandler(void);    /**< USART2_RX IRQ Handler */
void USART2_TX_IRQHandler(void);    /**< USART2_TX IRQ Handler */
void I2C1_IRQHandler(void);         /**< I2C1 IRQ Handler */
void USART3_RX_IRQHandler(void);    /**< USART3_RX IRQ Handler */
void USART3_TX_IRQHandler(void);    /**< USART3_TX IRQ Handler */
void VDAC0_IRQHandler(void);        /**< VDAC0 IRQ Handler */
void CSEN_IRQHandler(void);         /**< CSEN IRQ Handler */
void LESENSE_IRQHandler(void);      /**< LESENSE IRQ Handler */
void CRYPTO1_IRQHandler(void);      /**< CRYPTO1 IRQ Handler */
void TRNG0_IRQHandler(void);        /**< TRNG0 IRQ Handler */

#if (__FPU_PRESENT == 1)
void FPUEH_IRQHandler(void);        /**< FPUEH IRQ Handler */
#endif

uint32_t SystemCoreClockGet(void);

/**************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EFM CMU
 *   API, this variable will be kept updated. This function is only provided
 *   for CMSIS compliance and if a user modifies the the core clock outside
 *   the CMU API.
 *****************************************************************************/
static __INLINE void SystemCoreClockUpdate(void)
{
  SystemCoreClockGet();
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
#endif /* SYSTEM_EFM32_H */
