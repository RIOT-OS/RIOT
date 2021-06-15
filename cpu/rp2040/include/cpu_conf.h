#ifndef CPU_CONF_H
#define CPU_CONF_H

#define __CM0PLUS_REV          1         /*!< Core revision r0p1 */
#define __MPU_PRESENT          0         /*!< MPU present or not */
#define __NVIC_PRIO_BITS       2         /*!< Number of bits used for Priority Levels */
#define __VTOR_PRESENT         1         /*!< VTOR present or not */
#define __Vendor_SysTickConfig 0         /*!< Set to 1 if different SysTick Config is used */

#define __CHECK_DEVICE_DEFINES

#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (26U)

typedef enum {
/* =  ARM Cortex-M0 Specific Interrupt Numbers  = */
  Reset_IRQn = -15, /*!< -15  Reset Vector, invoked on Power up and warm reset */
  NonMaskableInt_IRQn = -14, /*!< -14  Non maskable Interrupt, cannot be stopped or preempted */
  HardFault_IRQn = -13, /*!< -13  Hard Fault, all classes of Fault */
  SVCall_IRQn =  -5, /*!< -5 System Service Call via SVC instruction */
  PendSV_IRQn =  -2, /*!< -2 Pendable request for system service */
  SysTick_IRQn =  -1, /*!< -1 System Tick Timer */
/* =  rp2040 Specific Interrupt Numbers  = */
  TIMER_0_IRQn = 0, /*!< 0  TIMER_0_IRQn */
  TIMER_1_IRQn = 1, /*!< 1  TIMER_1_IRQn */
  TIMER_2_IRQn = 2, /*!< 2  TIMER_2_IRQn */
  TIMER_3_IRQn = 3, /*!< 3  TIMER_3_IRQn */
  PWM_WRAP_IRQn = 4, /*!< 4  PWM_WRAP_IRQn */
  USBCTRL_IRQn = 5, /*!< 5  USBCTRL_IRQn */
  XIP_IRQn = 6, /*!< 6  XIP_IRQn */
  PIO0_0_IRQn = 7, /*!< 7  PIO0_0_IRQn */
  PIO0_1_IRQn = 8, /*!< 8  PIO0_1_IRQn */
  PIO1_0_IRQn = 9, /*!< 9  PIO1_0_IRQn */
  PIO1_1_IRQn = 10, /*!< 10 PIO1_1_IRQn */
  DMA_0_IRQn = 11, /*!< 11 DMA_0_IRQn */
  DMA_1_IRQn = 12, /*!< 12 DMA_1_IRQn */
  IO_BANK0_IRQn = 13, /*!< 13 IO_BANK0_IRQn */
  IO_QSPI_IRQn = 14, /*!< 14 IO_QSPI_IRQn */
  SIO_PROC0_IRQn = 15, /*!< 15 SIO_PROC0_IRQn */
  SIO_PROC1_IRQn = 16, /*!< 16 SIO_PROC1_IRQn */
  CLOCKS_IRQn = 17, /*!< 17 CLOCKS_IRQn */
  SPI0_IRQn = 18, /*!< 18 SPI0_IRQn */
  SPI1_IRQn = 19, /*!< 19 SPI1_IRQn */
  UART0_IRQn = 20, /*!< 20 UART0_IRQn */
  UART1_IRQn = 21, /*!< 21 UART1_IRQn */
  ADC0_FIFO_IRQn = 22, /*!< 22 ADC0_FIFO_IRQn */
  I2C0_IRQn = 23, /*!< 23 I2C0_IRQn */
  I2C1_IRQn = 24, /*!< 24 I2C1_IRQn */
  RTC_IRQn = 25, /*!< 25 RTC_IRQn */
} IRQn_Type;

#include "cpu_conf_common.h"
#include "core_cm0plus.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
