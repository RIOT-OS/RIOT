/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M4 System Layer for EFM32 devices.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SYSTEM_EFM32_H
#define SYSTEM_EFM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFM32GG11B EFM32GG11B
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   TYPEDEFS   ***********************************
 ******************************************************************************/

/* Interrupt vectortable entry */
typedef union {
  void (*pFunc)(void);
  void *topOfStack;
} tVectorEntry;

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

extern uint32_t SystemCoreClock;    /**< System Clock Frequency (Core Clock) */
extern uint32_t SystemHfrcoFreq;    /**< System HFRCO frequency */

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
#if defined(__ICCARM__)    /* IAR requires the __vector_table symbol */
#define __Vectors    __vector_table
#endif
extern const tVectorEntry __Vectors[];
#endif

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
void LDMA_IRQHandler(void);         /**< LDMA IRQ Handler */
void GPIO_EVEN_IRQHandler(void);    /**< GPIO_EVEN IRQ Handler */
void SMU_IRQHandler(void);          /**< SMU IRQ Handler */
void TIMER0_IRQHandler(void);       /**< TIMER0 IRQ Handler */
void USART0_RX_IRQHandler(void);    /**< USART0_RX IRQ Handler */
void USART0_TX_IRQHandler(void);    /**< USART0_TX IRQ Handler */
void ACMP0_IRQHandler(void);        /**< ACMP0 IRQ Handler */
void ADC0_IRQHandler(void);         /**< ADC0 IRQ Handler */
void IDAC0_IRQHandler(void);        /**< IDAC0 IRQ Handler */
void I2C0_IRQHandler(void);         /**< I2C0 IRQ Handler */
void I2C1_IRQHandler(void);         /**< I2C1 IRQ Handler */
void GPIO_ODD_IRQHandler(void);     /**< GPIO_ODD IRQ Handler */
void TIMER1_IRQHandler(void);       /**< TIMER1 IRQ Handler */
void TIMER2_IRQHandler(void);       /**< TIMER2 IRQ Handler */
void TIMER3_IRQHandler(void);       /**< TIMER3 IRQ Handler */
void USART1_RX_IRQHandler(void);    /**< USART1_RX IRQ Handler */
void USART1_TX_IRQHandler(void);    /**< USART1_TX IRQ Handler */
void USART2_RX_IRQHandler(void);    /**< USART2_RX IRQ Handler */
void USART2_TX_IRQHandler(void);    /**< USART2_TX IRQ Handler */
void UART0_RX_IRQHandler(void);     /**< UART0_RX IRQ Handler */
void UART0_TX_IRQHandler(void);     /**< UART0_TX IRQ Handler */
void UART1_RX_IRQHandler(void);     /**< UART1_RX IRQ Handler */
void UART1_TX_IRQHandler(void);     /**< UART1_TX IRQ Handler */
void LEUART0_IRQHandler(void);      /**< LEUART0 IRQ Handler */
void LEUART1_IRQHandler(void);      /**< LEUART1 IRQ Handler */
void LETIMER0_IRQHandler(void);     /**< LETIMER0 IRQ Handler */
void PCNT0_IRQHandler(void);        /**< PCNT0 IRQ Handler */
void PCNT1_IRQHandler(void);        /**< PCNT1 IRQ Handler */
void PCNT2_IRQHandler(void);        /**< PCNT2 IRQ Handler */
void RTCC_IRQHandler(void);         /**< RTCC IRQ Handler */
void CMU_IRQHandler(void);          /**< CMU IRQ Handler */
void MSC_IRQHandler(void);          /**< MSC IRQ Handler */
void CRYPTO0_IRQHandler(void);      /**< CRYPTO0 IRQ Handler */
void CRYOTIMER_IRQHandler(void);    /**< CRYOTIMER IRQ Handler */
void FPUEH_IRQHandler(void);        /**< FPUEH IRQ Handler */
void USART3_RX_IRQHandler(void);    /**< USART3_RX IRQ Handler */
void USART3_TX_IRQHandler(void);    /**< USART3_TX IRQ Handler */
void USART4_RX_IRQHandler(void);    /**< USART4_RX IRQ Handler */
void USART4_TX_IRQHandler(void);    /**< USART4_TX IRQ Handler */
void WTIMER0_IRQHandler(void);      /**< WTIMER0 IRQ Handler */
void WTIMER1_IRQHandler(void);      /**< WTIMER1 IRQ Handler */
void WTIMER2_IRQHandler(void);      /**< WTIMER2 IRQ Handler */
void WTIMER3_IRQHandler(void);      /**< WTIMER3 IRQ Handler */
void I2C2_IRQHandler(void);         /**< I2C2 IRQ Handler */
void VDAC0_IRQHandler(void);        /**< VDAC0 IRQ Handler */
void TIMER4_IRQHandler(void);       /**< TIMER4 IRQ Handler */
void TIMER5_IRQHandler(void);       /**< TIMER5 IRQ Handler */
void TIMER6_IRQHandler(void);       /**< TIMER6 IRQ Handler */
void USART5_RX_IRQHandler(void);    /**< USART5_RX IRQ Handler */
void USART5_TX_IRQHandler(void);    /**< USART5_TX IRQ Handler */
void CSEN_IRQHandler(void);         /**< CSEN IRQ Handler */
void LESENSE_IRQHandler(void);      /**< LESENSE IRQ Handler */
void EBI_IRQHandler(void);          /**< EBI IRQ Handler */
void ACMP2_IRQHandler(void);        /**< ACMP2 IRQ Handler */
void ADC1_IRQHandler(void);         /**< ADC1 IRQ Handler */
void LCD_IRQHandler(void);          /**< LCD IRQ Handler */
void SDIO_IRQHandler(void);         /**< SDIO IRQ Handler */
void ETH_IRQHandler(void);          /**< ETH IRQ Handler */
void CAN0_IRQHandler(void);         /**< CAN0 IRQ Handler */
void CAN1_IRQHandler(void);         /**< CAN1 IRQ Handler */
void USB_IRQHandler(void);          /**< USB IRQ Handler */
void RTC_IRQHandler(void);          /**< RTC IRQ Handler */
void WDOG1_IRQHandler(void);        /**< WDOG1 IRQ Handler */
void LETIMER1_IRQHandler(void);     /**< LETIMER1 IRQ Handler */
void TRNG0_IRQHandler(void);        /**< TRNG0 IRQ Handler */
void QSPI0_IRQHandler(void);        /**< QSPI0 IRQ Handler */

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
 *   Notice that only if changing the core clock frequency through the EFM32 CMU
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
#endif /* SYSTEM_EFM32_H */
