
/****************************************************************************************************//**
 * @file     TM4C123GH6PM.h
 *
 * @brief    CMSIS Cortex-M4 Peripheral Access Layer Header File for
 *           TM4C123GH6PM from Texas Instruments.
 *
 * @version  V12591
 * @date     19. February 2014
 *
 * @note     Generated with SVDConv V2.79v 
 *           from CMSIS SVD File 'TM4C123GH6PM.svd.xml' Version 12591,
 *
 * @par      
 *           Software License Agreement
 *           
 *           Texas Instruments (TI) is supplying this software for use solely and
 *           exclusively on TI's microcontroller products. The software is owned by
 *           TI and/or its suppliers, and is protected under applicable copyright
 *           laws. You may not combine this software with "viral" open-source
 *           software in order to form a larger program.
 *           
 *           THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 *           NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 *           NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *           A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 *           CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 *           DAMAGES, FOR ANY REASON WHATSOEVER.
 *           
 *           
 *
 *******************************************************************************************************/



/** @addtogroup Texas Instruments
  * @{
  */

/** @addtogroup TM4C123GH6PM
  * @{
  */

#ifndef TM4C123GH6PM_H
#define TM4C123GH6PM_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  MemoryManagement_IRQn         = -12,              /*!<   4  Memory Management, MPU mismatch, including Access Violation
                                                         and No Match                                                          */
  BusFault_IRQn                 = -11,              /*!<   5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                         related Fault                                                         */
  UsageFault_IRQn               = -10,              /*!<   6  Usage Fault, i.e. Undef Instruction, Illegal State Transition    */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* -------------------  TM4C123GH6PM Specific Interrupt Numbers  ------------------ */
  GPIOA_IRQn                    =   0,              /*!<   0  GPIOA                                                            */
  GPIOB_IRQn                    =   1,              /*!<   1  GPIOB                                                            */
  GPIOC_IRQn                    =   2,              /*!<   2  GPIOC                                                            */
  GPIOD_IRQn                    =   3,              /*!<   3  GPIOD                                                            */
  GPIOE_IRQn                    =   4,              /*!<   4  GPIOE                                                            */
  UART0_IRQn                    =   5,              /*!<   5  UART0                                                            */
  UART1_IRQn                    =   6,              /*!<   6  UART1                                                            */
  SSI0_IRQn                     =   7,              /*!<   7  SSI0                                                             */
  I2C0_IRQn                     =   8,              /*!<   8  I2C0                                                             */
  PWM0_FAULT_IRQn               =   9,              /*!<   9  PWM0_FAULT                                                       */
  PWM0_0_IRQn                   =  10,              /*!<  10  PWM0_0                                                           */
  PWM0_1_IRQn                   =  11,              /*!<  11  PWM0_1                                                           */
  PWM0_2_IRQn                   =  12,              /*!<  12  PWM0_2                                                           */
  QEI0_IRQn                     =  13,              /*!<  13  QEI0                                                             */
  ADC0SS0_IRQn                  =  14,              /*!<  14  ADC0SS0                                                          */
  ADC0SS1_IRQn                  =  15,              /*!<  15  ADC0SS1                                                          */
  ADC0SS2_IRQn                  =  16,              /*!<  16  ADC0SS2                                                          */
  ADC0SS3_IRQn                  =  17,              /*!<  17  ADC0SS3                                                          */
  WATCHDOG0_IRQn                =  18,              /*!<  18  WATCHDOG0                                                        */
  TIMER0A_IRQn                  =  19,              /*!<  19  TIMER0A                                                          */
  TIMER0B_IRQn                  =  20,              /*!<  20  TIMER0B                                                          */
  TIMER1A_IRQn                  =  21,              /*!<  21  TIMER1A                                                          */
  TIMER1B_IRQn                  =  22,              /*!<  22  TIMER1B                                                          */
  TIMER2A_IRQn                  =  23,              /*!<  23  TIMER2A                                                          */
  TIMER2B_IRQn                  =  24,              /*!<  24  TIMER2B                                                          */
  COMP0_IRQn                    =  25,              /*!<  25  COMP0                                                            */
  COMP1_IRQn                    =  26,              /*!<  26  COMP1                                                            */
  SYSCTL_IRQn                   =  28,              /*!<  28  SYSCTL                                                           */
  FLASH_CTRL_IRQn               =  29,              /*!<  29  FLASH_CTRL                                                       */
  GPIOF_IRQn                    =  30,              /*!<  30  GPIOF                                                            */
  UART2_IRQn                    =  33,              /*!<  33  UART2                                                            */
  SSI1_IRQn                     =  34,              /*!<  34  SSI1                                                             */
  TIMER3A_IRQn                  =  35,              /*!<  35  TIMER3A                                                          */
  TIMER3B_IRQn                  =  36,              /*!<  36  TIMER3B                                                          */
  I2C1_IRQn                     =  37,              /*!<  37  I2C1                                                             */
  QEI1_IRQn                     =  38,              /*!<  38  QEI1                                                             */
  CAN0_IRQn                     =  39,              /*!<  39  CAN0                                                             */
  CAN1_IRQn                     =  40,              /*!<  40  CAN1                                                             */
  HIB_IRQn                      =  43,              /*!<  43  HIB                                                              */
  USB0_IRQn                     =  44,              /*!<  44  USB0                                                             */
  PWM0_3_IRQn                   =  45,              /*!<  45  PWM0_3                                                           */
  UDMA_IRQn                     =  46,              /*!<  46  UDMA                                                             */
  UDMAERR_IRQn                  =  47,              /*!<  47  UDMAERR                                                          */
  ADC1SS0_IRQn                  =  48,              /*!<  48  ADC1SS0                                                          */
  ADC1SS1_IRQn                  =  49,              /*!<  49  ADC1SS1                                                          */
  ADC1SS2_IRQn                  =  50,              /*!<  50  ADC1SS2                                                          */
  ADC1SS3_IRQn                  =  51,              /*!<  51  ADC1SS3                                                          */
  SSI2_IRQn                     =  57,              /*!<  57  SSI2                                                             */
  SSI3_IRQn                     =  58,              /*!<  58  SSI3                                                             */
  UART3_IRQn                    =  59,              /*!<  59  UART3                                                            */
  UART4_IRQn                    =  60,              /*!<  60  UART4                                                            */
  UART5_IRQn                    =  61,              /*!<  61  UART5                                                            */
  UART6_IRQn                    =  62,              /*!<  62  UART6                                                            */
  UART7_IRQn                    =  63,              /*!<  63  UART7                                                            */
  I2C2_IRQn                     =  68,              /*!<  68  I2C2                                                             */
  I2C3_IRQn                     =  69,              /*!<  69  I2C3                                                             */
  TIMER4A_IRQn                  =  70,              /*!<  70  TIMER4A                                                          */
  TIMER4B_IRQn                  =  71,              /*!<  71  TIMER4B                                                          */
  TIMER5A_IRQn                  =  92,              /*!<  92  TIMER5A                                                          */
  TIMER5B_IRQn                  =  93,              /*!<  93  TIMER5B                                                          */
  WTIMER0A_IRQn                 =  94,              /*!<  94  WTIMER0A                                                         */
  WTIMER0B_IRQn                 =  95,              /*!<  95  WTIMER0B                                                         */
  WTIMER1A_IRQn                 =  96,              /*!<  96  WTIMER1A                                                         */
  WTIMER1B_IRQn                 =  97,              /*!<  97  WTIMER1B                                                         */
  WTIMER2A_IRQn                 =  98,              /*!<  98  WTIMER2A                                                         */
  WTIMER2B_IRQn                 =  99,              /*!<  99  WTIMER2B                                                         */
  WTIMER3A_IRQn                 = 100,              /*!< 100  WTIMER3A                                                         */
  WTIMER3B_IRQn                 = 101,              /*!< 101  WTIMER3B                                                         */
  WTIMER4A_IRQn                 = 102,              /*!< 102  WTIMER4A                                                         */
  WTIMER4B_IRQn                 = 103,              /*!< 103  WTIMER4B                                                         */
  WTIMER5A_IRQn                 = 104,              /*!< 104  WTIMER5A                                                         */
  WTIMER5B_IRQn                 = 105,              /*!< 105  WTIMER5B                                                         */
  SYSEXC_IRQn                   = 106,              /*!< 106  SYSEXC                                                           */
  PWM1_0_IRQn                   = 134,              /*!< 134  PWM1_0                                                           */
  PWM1_1_IRQn                   = 135,              /*!< 135  PWM1_1                                                           */
  PWM1_2_IRQn                   = 136,              /*!< 136  PWM1_2                                                           */
  PWM1_3_IRQn                   = 137,              /*!< 137  PWM1_3                                                           */
  PWM1_FAULT_IRQn               = 138               /*!< 138  PWM1_FAULT                                                       */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M4 Processor and Core Peripherals---------------- */
#define __CM4_REV                 0x0102            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               3            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT                  1            /*!< FPU present or not                                                    */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm4.h"                               /*!< Cortex-M4 processor and core peripherals                              */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                    WATCHDOG0                   ================ */
/* ================================================================================ */


/**
  * @brief Register map for WATCHDOG0 peripheral (WATCHDOG0)
  */

typedef struct {                                    /*!< WATCHDOG0 Structure                                                   */
  __IO uint32_t  LOAD;                              /*!< Watchdog Load                                                         */
  __IO uint32_t  VALUE;                             /*!< Watchdog Value                                                        */
  __IO uint32_t  CTL;                               /*!< Watchdog Control                                                      */
  __O  uint32_t  ICR;                               /*!< Watchdog Interrupt Clear                                              */
  __IO uint32_t  RIS;                               /*!< Watchdog Raw Interrupt Status                                         */
  __IO uint32_t  MIS;                               /*!< Watchdog Masked Interrupt Status                                      */
  __I  uint32_t  RESERVED0[256];
  __IO uint32_t  TEST;                              /*!< Watchdog Test                                                         */
  __I  uint32_t  RESERVED1[505];
  __IO uint32_t  LOCK;                              /*!< Watchdog Lock                                                         */
} WATCHDOG0_Type;


/* ================================================================================ */
/* ================                      GPIOA                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for GPIOA peripheral (GPIOA)
  */

typedef struct {                                    /*!< GPIOA Structure                                                       */
  __I  uint32_t  RESERVED0[255];
  __IO uint32_t  DATA;                              /*!< GPIO Data                                                             */
  __IO uint32_t  DIR;                               /*!< GPIO Direction                                                        */
  __IO uint32_t  IS;                                /*!< GPIO Interrupt Sense                                                  */
  __IO uint32_t  IBE;                               /*!< GPIO Interrupt Both Edges                                             */
  __IO uint32_t  IEV;                               /*!< GPIO Interrupt Event                                                  */
  __IO uint32_t  IM;                                /*!< GPIO Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< GPIO Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< GPIO Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< GPIO Interrupt Clear                                                  */
  __IO uint32_t  AFSEL;                             /*!< GPIO Alternate Function Select                                        */
  __I  uint32_t  RESERVED1[55];
  __IO uint32_t  DR2R;                              /*!< GPIO 2-mA Drive Select                                                */
  __IO uint32_t  DR4R;                              /*!< GPIO 4-mA Drive Select                                                */
  __IO uint32_t  DR8R;                              /*!< GPIO 8-mA Drive Select                                                */
  __IO uint32_t  ODR;                               /*!< GPIO Open Drain Select                                                */
  __IO uint32_t  PUR;                               /*!< GPIO Pull-Up Select                                                   */
  __IO uint32_t  PDR;                               /*!< GPIO Pull-Down Select                                                 */
  __IO uint32_t  SLR;                               /*!< GPIO Slew Rate Control Select                                         */
  __IO uint32_t  DEN;                               /*!< GPIO Digital Enable                                                   */
  __IO uint32_t  LOCK;                              /*!< GPIO Lock                                                             */
  __I  uint32_t  CR;                                /*!< GPIO Commit                                                           */
  __IO uint32_t  AMSEL;                             /*!< GPIO Analog Mode Select                                               */
  __IO uint32_t  PCTL;                              /*!< GPIO Port Control                                                     */
  __IO uint32_t  ADCCTL;                            /*!< GPIO ADC Control                                                      */
  __IO uint32_t  DMACTL;                            /*!< GPIO DMA Control                                                      */
} GPIOA_Type;


/* ================================================================================ */
/* ================                      SSI0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for SSI0 peripheral (SSI0)
  */

typedef struct {                                    /*!< SSI0 Structure                                                        */
  __IO uint32_t  CR0;                               /*!< SSI Control 0                                                         */
  __IO uint32_t  CR1;                               /*!< SSI Control 1                                                         */
  __IO uint32_t  DR;                                /*!< SSI Data                                                              */
  __IO uint32_t  SR;                                /*!< SSI Status                                                            */
  __IO uint32_t  CPSR;                              /*!< SSI Clock Prescale                                                    */
  __IO uint32_t  IM;                                /*!< SSI Interrupt Mask                                                    */
  __IO uint32_t  RIS;                               /*!< SSI Raw Interrupt Status                                              */
  __IO uint32_t  MIS;                               /*!< SSI Masked Interrupt Status                                           */
  __O  uint32_t  ICR;                               /*!< SSI Interrupt Clear                                                   */
  __IO uint32_t  DMACTL;                            /*!< SSI DMA Control                                                       */
  __I  uint32_t  RESERVED0[1000];
  __IO uint32_t  CC;                                /*!< SSI Clock Configuration                                               */
} SSI0_Type;


/* ================================================================================ */
/* ================                      UART0                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for UART0 peripheral (UART0)
  */

typedef struct {                                    /*!< UART0 Structure                                                       */
  __IO uint32_t  DR;                                /*!< UART Data                                                             */
  
  union {
    __IO uint32_t  ECR_UART_ALT;                    /*!< UART Receive Status/Error Clear                                       */
    __IO uint32_t  RSR;                             /*!< UART Receive Status/Error Clear                                       */
  };
  __I  uint32_t  RESERVED0[4];
  __IO uint32_t  FR;                                /*!< UART Flag                                                             */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  ILPR;                              /*!< UART IrDA Low-Power Register                                          */
  __IO uint32_t  IBRD;                              /*!< UART Integer Baud-Rate Divisor                                        */
  __IO uint32_t  FBRD;                              /*!< UART Fractional Baud-Rate Divisor                                     */
  __IO uint32_t  LCRH;                              /*!< UART Line Control                                                     */
  __IO uint32_t  CTL;                               /*!< UART Control                                                          */
  __IO uint32_t  IFLS;                              /*!< UART Interrupt FIFO Level Select                                      */
  __IO uint32_t  IM;                                /*!< UART Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< UART Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< UART Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< UART Interrupt Clear                                                  */
  __IO uint32_t  DMACTL;                            /*!< UART DMA Control                                                      */
  __I  uint32_t  RESERVED2[22];
  __IO uint32_t  _9BITADDR;                         /*!< UART 9-Bit Self Address                                               */
  __IO uint32_t  _9BITAMASK;                        /*!< UART 9-Bit Self Address Mask                                          */
  __I  uint32_t  RESERVED3[965];
  __IO uint32_t  PP;                                /*!< UART Peripheral Properties                                            */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  CC;                                /*!< UART Clock Configuration                                              */
} UART0_Type;


/* ================================================================================ */
/* ================                      I2C0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for I2C0 peripheral (I2C0)
  */

typedef struct {                                    /*!< I2C0 Structure                                                        */
  __IO uint32_t  MSA;                               /*!< I2C Master Slave Address                                              */
  
  union {
    __IO uint32_t  MCS_I2C0_ALT;                    /*!< I2C Master Control/Status                                             */
    __IO uint32_t  MCS;                             /*!< I2C Master Control/Status                                             */
  };
  __IO uint32_t  MDR;                               /*!< I2C Master Data                                                       */
  __IO uint32_t  MTPR;                              /*!< I2C Master Timer Period                                               */
  __IO uint32_t  MIMR;                              /*!< I2C Master Interrupt Mask                                             */
  __IO uint32_t  MRIS;                              /*!< I2C Master Raw Interrupt Status                                       */
  __IO uint32_t  MMIS;                              /*!< I2C Master Masked Interrupt Status                                    */
  __O  uint32_t  MICR;                              /*!< I2C Master Interrupt Clear                                            */
  __IO uint32_t  MCR;                               /*!< I2C Master Configuration                                              */
  __IO uint32_t  MCLKOCNT;                          /*!< I2C Master Clock Low Timeout Count                                    */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  MBMON;                             /*!< I2C Master Bus Monitor                                                */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  MCR2;                              /*!< I2C Master Configuration 2                                            */
  __I  uint32_t  RESERVED2[497];
  __IO uint32_t  SOAR;                              /*!< I2C Slave Own Address                                                 */
  
  union {
    __IO uint32_t  SCSR_I2C0_ALT;                   /*!< I2C Slave Control/Status                                              */
    __IO uint32_t  SCSR;                            /*!< I2C Slave Control/Status                                              */
  };
  __IO uint32_t  SDR;                               /*!< I2C Slave Data                                                        */
  __IO uint32_t  SIMR;                              /*!< I2C Slave Interrupt Mask                                              */
  __IO uint32_t  SRIS;                              /*!< I2C Slave Raw Interrupt Status                                        */
  __IO uint32_t  SMIS;                              /*!< I2C Slave Masked Interrupt Status                                     */
  __O  uint32_t  SICR;                              /*!< I2C Slave Interrupt Clear                                             */
  __IO uint32_t  SOAR2;                             /*!< I2C Slave Own Address 2                                               */
  __IO uint32_t  SACKCTL;                           /*!< I2C Slave ACK Control                                                 */
  __I  uint32_t  RESERVED3[487];
  __IO uint32_t  PP;                                /*!< I2C Peripheral Properties                                             */
  __IO uint32_t  PC;                                /*!< I2C Peripheral Configuration                                          */
} I2C0_Type;


/* ================================================================================ */
/* ================                      PWM0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for PWM0 peripheral (PWM0)
  */

typedef struct {                                    /*!< PWM0 Structure                                                        */
  __IO uint32_t  CTL;                               /*!< PWM Master Control                                                    */
  __IO uint32_t  SYNC;                              /*!< PWM Time Base Sync                                                    */
  __IO uint32_t  ENABLE;                            /*!< PWM Output Enable                                                     */
  __IO uint32_t  INVERT;                            /*!< PWM Output Inversion                                                  */
  __IO uint32_t  FAULT;                             /*!< PWM Output Fault                                                      */
  __IO uint32_t  INTEN;                             /*!< PWM Interrupt Enable                                                  */
  __IO uint32_t  RIS;                               /*!< PWM Raw Interrupt Status                                              */
  __IO uint32_t  ISC;                               /*!< PWM Interrupt Status and Clear                                        */
  __IO uint32_t  STATUS;                            /*!< PWM Status                                                            */
  __IO uint32_t  FAULTVAL;                          /*!< PWM Fault Condition Value                                             */
  __IO uint32_t  ENUPD;                             /*!< PWM Enable Update                                                     */
  __I  uint32_t  RESERVED0[5];
  __IO uint32_t  _0_CTL;                            /*!< PWM0 Control                                                          */
  __IO uint32_t  _0_INTEN;                          /*!< PWM0 Interrupt and Trigger Enable                                     */
  __IO uint32_t  _0_RIS;                            /*!< PWM0 Raw Interrupt Status                                             */
  __IO uint32_t  _0_ISC;                            /*!< PWM0 Interrupt Status and Clear                                       */
  __IO uint32_t  _0_LOAD;                           /*!< PWM0 Load                                                             */
  __IO uint32_t  _0_COUNT;                          /*!< PWM0 Counter                                                          */
  __IO uint32_t  _0_CMPA;                           /*!< PWM0 Compare A                                                        */
  __IO uint32_t  _0_CMPB;                           /*!< PWM0 Compare B                                                        */
  __IO uint32_t  _0_GENA;                           /*!< PWM0 Generator A Control                                              */
  __IO uint32_t  _0_GENB;                           /*!< PWM0 Generator B Control                                              */
  __IO uint32_t  _0_DBCTL;                          /*!< PWM0 Dead-Band Control                                                */
  __IO uint32_t  _0_DBRISE;                         /*!< PWM0 Dead-Band Rising-Edge Delay                                      */
  __IO uint32_t  _0_DBFALL;                         /*!< PWM0 Dead-Band Falling-Edge-Delay                                     */
  __IO uint32_t  _0_FLTSRC0;                        /*!< PWM0 Fault Source 0                                                   */
  __IO uint32_t  _0_FLTSRC1;                        /*!< PWM0 Fault Source 1                                                   */
  __IO uint32_t  _0_MINFLTPER;                      /*!< PWM0 Minimum Fault Period                                             */
  __IO uint32_t  _1_CTL;                            /*!< PWM1 Control                                                          */
  __IO uint32_t  _1_INTEN;                          /*!< PWM1 Interrupt and Trigger Enable                                     */
  __IO uint32_t  _1_RIS;                            /*!< PWM1 Raw Interrupt Status                                             */
  __IO uint32_t  _1_ISC;                            /*!< PWM1 Interrupt Status and Clear                                       */
  __IO uint32_t  _1_LOAD;                           /*!< PWM1 Load                                                             */
  __IO uint32_t  _1_COUNT;                          /*!< PWM1 Counter                                                          */
  __IO uint32_t  _1_CMPA;                           /*!< PWM1 Compare A                                                        */
  __IO uint32_t  _1_CMPB;                           /*!< PWM1 Compare B                                                        */
  __IO uint32_t  _1_GENA;                           /*!< PWM1 Generator A Control                                              */
  __IO uint32_t  _1_GENB;                           /*!< PWM1 Generator B Control                                              */
  __IO uint32_t  _1_DBCTL;                          /*!< PWM1 Dead-Band Control                                                */
  __IO uint32_t  _1_DBRISE;                         /*!< PWM1 Dead-Band Rising-Edge Delay                                      */
  __IO uint32_t  _1_DBFALL;                         /*!< PWM1 Dead-Band Falling-Edge-Delay                                     */
  __IO uint32_t  _1_FLTSRC0;                        /*!< PWM1 Fault Source 0                                                   */
  __IO uint32_t  _1_FLTSRC1;                        /*!< PWM1 Fault Source 1                                                   */
  __IO uint32_t  _1_MINFLTPER;                      /*!< PWM1 Minimum Fault Period                                             */
  __IO uint32_t  _2_CTL;                            /*!< PWM2 Control                                                          */
  __IO uint32_t  _2_INTEN;                          /*!< PWM2 Interrupt and Trigger Enable                                     */
  __IO uint32_t  _2_RIS;                            /*!< PWM2 Raw Interrupt Status                                             */
  __IO uint32_t  _2_ISC;                            /*!< PWM2 Interrupt Status and Clear                                       */
  __IO uint32_t  _2_LOAD;                           /*!< PWM2 Load                                                             */
  __IO uint32_t  _2_COUNT;                          /*!< PWM2 Counter                                                          */
  __IO uint32_t  _2_CMPA;                           /*!< PWM2 Compare A                                                        */
  __IO uint32_t  _2_CMPB;                           /*!< PWM2 Compare B                                                        */
  __IO uint32_t  _2_GENA;                           /*!< PWM2 Generator A Control                                              */
  __IO uint32_t  _2_GENB;                           /*!< PWM2 Generator B Control                                              */
  __IO uint32_t  _2_DBCTL;                          /*!< PWM2 Dead-Band Control                                                */
  __IO uint32_t  _2_DBRISE;                         /*!< PWM2 Dead-Band Rising-Edge Delay                                      */
  __IO uint32_t  _2_DBFALL;                         /*!< PWM2 Dead-Band Falling-Edge-Delay                                     */
  __IO uint32_t  _2_FLTSRC0;                        /*!< PWM2 Fault Source 0                                                   */
  __IO uint32_t  _2_FLTSRC1;                        /*!< PWM2 Fault Source 1                                                   */
  __IO uint32_t  _2_MINFLTPER;                      /*!< PWM2 Minimum Fault Period                                             */
  __IO uint32_t  _3_CTL;                            /*!< PWM3 Control                                                          */
  __IO uint32_t  _3_INTEN;                          /*!< PWM3 Interrupt and Trigger Enable                                     */
  __IO uint32_t  _3_RIS;                            /*!< PWM3 Raw Interrupt Status                                             */
  __IO uint32_t  _3_ISC;                            /*!< PWM3 Interrupt Status and Clear                                       */
  __IO uint32_t  _3_LOAD;                           /*!< PWM3 Load                                                             */
  __IO uint32_t  _3_COUNT;                          /*!< PWM3 Counter                                                          */
  __IO uint32_t  _3_CMPA;                           /*!< PWM3 Compare A                                                        */
  __IO uint32_t  _3_CMPB;                           /*!< PWM3 Compare B                                                        */
  __IO uint32_t  _3_GENA;                           /*!< PWM3 Generator A Control                                              */
  __IO uint32_t  _3_GENB;                           /*!< PWM3 Generator B Control                                              */
  __IO uint32_t  _3_DBCTL;                          /*!< PWM3 Dead-Band Control                                                */
  __IO uint32_t  _3_DBRISE;                         /*!< PWM3 Dead-Band Rising-Edge Delay                                      */
  __IO uint32_t  _3_DBFALL;                         /*!< PWM3 Dead-Band Falling-Edge-Delay                                     */
  __IO uint32_t  _3_FLTSRC0;                        /*!< PWM3 Fault Source 0                                                   */
  __IO uint32_t  _3_FLTSRC1;                        /*!< PWM3 Fault Source 1                                                   */
  __IO uint32_t  _3_MINFLTPER;                      /*!< PWM3 Minimum Fault Period                                             */
  __I  uint32_t  RESERVED1[432];
  __IO uint32_t  _0_FLTSEN;                         /*!< PWM0 Fault Pin Logic Sense                                            */
  __I  uint32_t  _0_FLTSTAT0;                       /*!< PWM0 Fault Status 0                                                   */
  __I  uint32_t  _0_FLTSTAT1;                       /*!< PWM0 Fault Status 1                                                   */
  __I  uint32_t  RESERVED2[29];
  __IO uint32_t  _1_FLTSEN;                         /*!< PWM1 Fault Pin Logic Sense                                            */
  __I  uint32_t  _1_FLTSTAT0;                       /*!< PWM1 Fault Status 0                                                   */
  __I  uint32_t  _1_FLTSTAT1;                       /*!< PWM1 Fault Status 1                                                   */
  __I  uint32_t  RESERVED3[30];
  __I  uint32_t  _2_FLTSTAT0;                       /*!< PWM2 Fault Status 0                                                   */
  __I  uint32_t  _2_FLTSTAT1;                       /*!< PWM2 Fault Status 1                                                   */
  __I  uint32_t  RESERVED4[30];
  __I  uint32_t  _3_FLTSTAT0;                       /*!< PWM3 Fault Status 0                                                   */
  __I  uint32_t  _3_FLTSTAT1;                       /*!< PWM3 Fault Status 1                                                   */
  __I  uint32_t  RESERVED5[397];
  __IO uint32_t  PP;                                /*!< PWM Peripheral Properties                                             */
} PWM0_Type;


/* ================================================================================ */
/* ================                      QEI0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for QEI0 peripheral (QEI0)
  */

typedef struct {                                    /*!< QEI0 Structure                                                        */
  __IO uint32_t  CTL;                               /*!< QEI Control                                                           */
  __IO uint32_t  STAT;                              /*!< QEI Status                                                            */
  __IO uint32_t  POS;                               /*!< QEI Position                                                          */
  __IO uint32_t  MAXPOS;                            /*!< QEI Maximum Position                                                  */
  __IO uint32_t  LOAD;                              /*!< QEI Timer Load                                                        */
  __IO uint32_t  TIME;                              /*!< QEI Timer                                                             */
  __IO uint32_t  COUNT;                             /*!< QEI Velocity Counter                                                  */
  __IO uint32_t  SPEED;                             /*!< QEI Velocity                                                          */
  __IO uint32_t  INTEN;                             /*!< QEI Interrupt Enable                                                  */
  __IO uint32_t  RIS;                               /*!< QEI Raw Interrupt Status                                              */
  __IO uint32_t  ISC;                               /*!< QEI Interrupt Status and Clear                                        */
} QEI0_Type;


/* ================================================================================ */
/* ================                     TIMER0                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for TIMER0 peripheral (TIMER0)
  */

typedef struct {                                    /*!< TIMER0 Structure                                                      */
  __IO uint32_t  CFG;                               /*!< GPTM Configuration                                                    */
  __IO uint32_t  TAMR;                              /*!< GPTM Timer A Mode                                                     */
  __IO uint32_t  TBMR;                              /*!< GPTM Timer B Mode                                                     */
  __IO uint32_t  CTL;                               /*!< GPTM Control                                                          */
  __IO uint32_t  SYNC;                              /*!< GPTM Synchronize                                                      */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  IMR;                               /*!< GPTM Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< GPTM Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< GPTM Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< GPTM Interrupt Clear                                                  */
  __IO uint32_t  TAILR;                             /*!< GPTM Timer A Interval Load                                            */
  __IO uint32_t  TBILR;                             /*!< GPTM Timer B Interval Load                                            */
  __IO uint32_t  TAMATCHR;                          /*!< GPTM Timer A Match                                                    */
  __IO uint32_t  TBMATCHR;                          /*!< GPTM Timer B Match                                                    */
  __IO uint32_t  TAPR;                              /*!< GPTM Timer A Prescale                                                 */
  __IO uint32_t  TBPR;                              /*!< GPTM Timer B Prescale                                                 */
  __IO uint32_t  TAPMR;                             /*!< GPTM TimerA Prescale Match                                            */
  __IO uint32_t  TBPMR;                             /*!< GPTM TimerB Prescale Match                                            */
  __IO uint32_t  TAR;                               /*!< GPTM Timer A                                                          */
  __IO uint32_t  TBR;                               /*!< GPTM Timer B                                                          */
  __IO uint32_t  TAV;                               /*!< GPTM Timer A Value                                                    */
  __IO uint32_t  TBV;                               /*!< GPTM Timer B Value                                                    */
  __IO uint32_t  RTCPD;                             /*!< GPTM RTC Predivide                                                    */
  __IO uint32_t  TAPS;                              /*!< GPTM Timer A Prescale Snapshot                                        */
  __IO uint32_t  TBPS;                              /*!< GPTM Timer B Prescale Snapshot                                        */
  __IO uint32_t  TAPV;                              /*!< GPTM Timer A Prescale Value                                           */
  __IO uint32_t  TBPV;                              /*!< GPTM Timer B Prescale Value                                           */
  __I  uint32_t  RESERVED1[981];
  __IO uint32_t  PP;                                /*!< GPTM Peripheral Properties                                            */
} TIMER0_Type;


/* ================================================================================ */
/* ================                     WTIMER0                    ================ */
/* ================================================================================ */


/**
  * @brief Register map for WTIMER0 peripheral (WTIMER0)
  */

typedef struct {                                    /*!< WTIMER0 Structure                                                     */
  __IO uint32_t  CFG;                               /*!< GPTM Configuration                                                    */
  __IO uint32_t  TAMR;                              /*!< GPTM Timer A Mode                                                     */
  __IO uint32_t  TBMR;                              /*!< GPTM Timer B Mode                                                     */
  __IO uint32_t  CTL;                               /*!< GPTM Control                                                          */
  __IO uint32_t  SYNC;                              /*!< GPTM Synchronize                                                      */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  IMR;                               /*!< GPTM Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< GPTM Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< GPTM Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< GPTM Interrupt Clear                                                  */
  __IO uint32_t  TAILR;                             /*!< GPTM Timer A Interval Load                                            */
  __IO uint32_t  TBILR;                             /*!< GPTM Timer B Interval Load                                            */
  __IO uint32_t  TAMATCHR;                          /*!< GPTM Timer A Match                                                    */
  __IO uint32_t  TBMATCHR;                          /*!< GPTM Timer B Match                                                    */
  __IO uint32_t  TAPR;                              /*!< GPTM Timer A Prescale                                                 */
  __IO uint32_t  TBPR;                              /*!< GPTM Timer B Prescale                                                 */
  __IO uint32_t  TAPMR;                             /*!< GPTM TimerA Prescale Match                                            */
  __IO uint32_t  TBPMR;                             /*!< GPTM TimerB Prescale Match                                            */
  __IO uint32_t  TAR;                               /*!< GPTM Timer A                                                          */
  __IO uint32_t  TBR;                               /*!< GPTM Timer B                                                          */
  __IO uint32_t  TAV;                               /*!< GPTM Timer A Value                                                    */
  __IO uint32_t  TBV;                               /*!< GPTM Timer B Value                                                    */
  __IO uint32_t  RTCPD;                             /*!< GPTM RTC Predivide                                                    */
  __IO uint32_t  TAPS;                              /*!< GPTM Timer A Prescale Snapshot                                        */
  __IO uint32_t  TBPS;                              /*!< GPTM Timer B Prescale Snapshot                                        */
  __IO uint32_t  TAPV;                              /*!< GPTM Timer A Prescale Value                                           */
  __IO uint32_t  TBPV;                              /*!< GPTM Timer B Prescale Value                                           */
  __I  uint32_t  RESERVED1[981];
  __IO uint32_t  PP;                                /*!< GPTM Peripheral Properties                                            */
} WTIMER0_Type;


/* ================================================================================ */
/* ================                      ADC0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for ADC0 peripheral (ADC0)
  */

typedef struct {                                    /*!< ADC0 Structure                                                        */
  __IO uint32_t  ACTSS;                             /*!< ADC Active Sample Sequencer                                           */
  __IO uint32_t  RIS;                               /*!< ADC Raw Interrupt Status                                              */
  __IO uint32_t  IM;                                /*!< ADC Interrupt Mask                                                    */
  __IO uint32_t  ISC;                               /*!< ADC Interrupt Status and Clear                                        */
  __IO uint32_t  OSTAT;                             /*!< ADC Overflow Status                                                   */
  __IO uint32_t  EMUX;                              /*!< ADC Event Multiplexer Select                                          */
  __IO uint32_t  USTAT;                             /*!< ADC Underflow Status                                                  */
  __IO uint32_t  TSSEL;                             /*!< ADC Trigger Source Select                                             */
  __IO uint32_t  SSPRI;                             /*!< ADC Sample Sequencer Priority                                         */
  __IO uint32_t  SPC;                               /*!< ADC Sample Phase Control                                              */
  __IO uint32_t  PSSI;                              /*!< ADC Processor Sample Sequence Initiate                                */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  SAC;                               /*!< ADC Sample Averaging Control                                          */
  __IO uint32_t  DCISC;                             /*!< ADC Digital Comparator Interrupt Status and Clear                     */
  __IO uint32_t  CTL;                               /*!< ADC Control                                                           */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  SSMUX0;                            /*!< ADC Sample Sequence Input Multiplexer Select 0                        */
  __IO uint32_t  SSCTL0;                            /*!< ADC Sample Sequence Control 0                                         */
  __IO uint32_t  SSFIFO0;                           /*!< ADC Sample Sequence Result FIFO 0                                     */
  __IO uint32_t  SSFSTAT0;                          /*!< ADC Sample Sequence FIFO 0 Status                                     */
  __IO uint32_t  SSOP0;                             /*!< ADC Sample Sequence 0 Operation                                       */
  __IO uint32_t  SSDC0;                             /*!< ADC Sample Sequence 0 Digital Comparator Select                       */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  SSMUX1;                            /*!< ADC Sample Sequence Input Multiplexer Select 1                        */
  __IO uint32_t  SSCTL1;                            /*!< ADC Sample Sequence Control 1                                         */
  __IO uint32_t  SSFIFO1;                           /*!< ADC Sample Sequence Result FIFO 1                                     */
  __IO uint32_t  SSFSTAT1;                          /*!< ADC Sample Sequence FIFO 1 Status                                     */
  __IO uint32_t  SSOP1;                             /*!< ADC Sample Sequence 1 Operation                                       */
  __IO uint32_t  SSDC1;                             /*!< ADC Sample Sequence 1 Digital Comparator Select                       */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  SSMUX2;                            /*!< ADC Sample Sequence Input Multiplexer Select 2                        */
  __IO uint32_t  SSCTL2;                            /*!< ADC Sample Sequence Control 2                                         */
  __IO uint32_t  SSFIFO2;                           /*!< ADC Sample Sequence Result FIFO 2                                     */
  __IO uint32_t  SSFSTAT2;                          /*!< ADC Sample Sequence FIFO 2 Status                                     */
  __IO uint32_t  SSOP2;                             /*!< ADC Sample Sequence 2 Operation                                       */
  __IO uint32_t  SSDC2;                             /*!< ADC Sample Sequence 2 Digital Comparator Select                       */
  __I  uint32_t  RESERVED4[2];
  __IO uint32_t  SSMUX3;                            /*!< ADC Sample Sequence Input Multiplexer Select 3                        */
  __IO uint32_t  SSCTL3;                            /*!< ADC Sample Sequence Control 3                                         */
  __IO uint32_t  SSFIFO3;                           /*!< ADC Sample Sequence Result FIFO 3                                     */
  __IO uint32_t  SSFSTAT3;                          /*!< ADC Sample Sequence FIFO 3 Status                                     */
  __IO uint32_t  SSOP3;                             /*!< ADC Sample Sequence 3 Operation                                       */
  __IO uint32_t  SSDC3;                             /*!< ADC Sample Sequence 3 Digital Comparator Select                       */
  __I  uint32_t  RESERVED5[786];
  __O  uint32_t  DCRIC;                             /*!< ADC Digital Comparator Reset Initial Conditions                       */
  __I  uint32_t  RESERVED6[63];
  __IO uint32_t  DCCTL0;                            /*!< ADC Digital Comparator Control 0                                      */
  __IO uint32_t  DCCTL1;                            /*!< ADC Digital Comparator Control 1                                      */
  __IO uint32_t  DCCTL2;                            /*!< ADC Digital Comparator Control 2                                      */
  __IO uint32_t  DCCTL3;                            /*!< ADC Digital Comparator Control 3                                      */
  __IO uint32_t  DCCTL4;                            /*!< ADC Digital Comparator Control 4                                      */
  __IO uint32_t  DCCTL5;                            /*!< ADC Digital Comparator Control 5                                      */
  __IO uint32_t  DCCTL6;                            /*!< ADC Digital Comparator Control 6                                      */
  __IO uint32_t  DCCTL7;                            /*!< ADC Digital Comparator Control 7                                      */
  __I  uint32_t  RESERVED7[8];
  __IO uint32_t  DCCMP0;                            /*!< ADC Digital Comparator Range 0                                        */
  __IO uint32_t  DCCMP1;                            /*!< ADC Digital Comparator Range 1                                        */
  __IO uint32_t  DCCMP2;                            /*!< ADC Digital Comparator Range 2                                        */
  __IO uint32_t  DCCMP3;                            /*!< ADC Digital Comparator Range 3                                        */
  __IO uint32_t  DCCMP4;                            /*!< ADC Digital Comparator Range 4                                        */
  __IO uint32_t  DCCMP5;                            /*!< ADC Digital Comparator Range 5                                        */
  __IO uint32_t  DCCMP6;                            /*!< ADC Digital Comparator Range 6                                        */
  __IO uint32_t  DCCMP7;                            /*!< ADC Digital Comparator Range 7                                        */
  __I  uint32_t  RESERVED8[88];
  __IO uint32_t  PP;                                /*!< ADC Peripheral Properties                                             */
  __IO uint32_t  PC;                                /*!< ADC Peripheral Configuration                                          */
  __IO uint32_t  CC;                                /*!< ADC Clock Configuration                                               */
} ADC0_Type;


/* ================================================================================ */
/* ================                      COMP                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for COMP peripheral (COMP)
  */

typedef struct {                                    /*!< COMP Structure                                                        */
  __IO uint32_t  ACMIS;                             /*!< Analog Comparator Masked Interrupt Status                             */
  __IO uint32_t  ACRIS;                             /*!< Analog Comparator Raw Interrupt Status                                */
  __IO uint32_t  ACINTEN;                           /*!< Analog Comparator Interrupt Enable                                    */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  ACREFCTL;                          /*!< Analog Comparator Reference Voltage Control                           */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  ACSTAT0;                           /*!< Analog Comparator Status 0                                            */
  __IO uint32_t  ACCTL0;                            /*!< Analog Comparator Control 0                                           */
  __I  uint32_t  RESERVED2[6];
  __IO uint32_t  ACSTAT1;                           /*!< Analog Comparator Status 1                                            */
  __IO uint32_t  ACCTL1;                            /*!< Analog Comparator Control 1                                           */
  __I  uint32_t  RESERVED3[990];
  __IO uint32_t  PP;                                /*!< Analog Comparator Peripheral Properties                               */
} COMP_Type;


/* ================================================================================ */
/* ================                      CAN0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for CAN0 peripheral (CAN0)
  */

typedef struct {                                    /*!< CAN0 Structure                                                        */
  __IO uint32_t  CTL;                               /*!< CAN Control                                                           */
  __IO uint32_t  STS;                               /*!< CAN Status                                                            */
  __IO uint32_t  ERR;                               /*!< CAN Error Counter                                                     */
  __IO uint32_t  BIT;                               /*!< CAN Bit Timing                                                        */
  __IO uint32_t  INT;                               /*!< CAN Interrupt                                                         */
  __IO uint32_t  TST;                               /*!< CAN Test                                                              */
  __IO uint32_t  BRPE;                              /*!< CAN Baud Rate Prescaler Extension                                     */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  IF1CRQ;                            /*!< CAN IF1 Command Request                                               */
  
  union {
    __IO uint32_t  IF1CMSK_CAN0_ALT;                /*!< CAN IF1 Command Mask                                                  */
    __IO uint32_t  IF1CMSK;                         /*!< CAN IF1 Command Mask                                                  */
  };
  __IO uint32_t  IF1MSK1;                           /*!< CAN IF1 Mask 1                                                        */
  __IO uint32_t  IF1MSK2;                           /*!< CAN IF1 Mask 2                                                        */
  __IO uint32_t  IF1ARB1;                           /*!< CAN IF1 Arbitration 1                                                 */
  __IO uint32_t  IF1ARB2;                           /*!< CAN IF1 Arbitration 2                                                 */
  __IO uint32_t  IF1MCTL;                           /*!< CAN IF1 Message Control                                               */
  __IO uint32_t  IF1DA1;                            /*!< CAN IF1 Data A1                                                       */
  __IO uint32_t  IF1DA2;                            /*!< CAN IF1 Data A2                                                       */
  __IO uint32_t  IF1DB1;                            /*!< CAN IF1 Data B1                                                       */
  __IO uint32_t  IF1DB2;                            /*!< CAN IF1 Data B2                                                       */
  __I  uint32_t  RESERVED1[13];
  __IO uint32_t  IF2CRQ;                            /*!< CAN IF2 Command Request                                               */
  
  union {
    __IO uint32_t  IF2CMSK_CAN0_ALT;                /*!< CAN IF2 Command Mask                                                  */
    __IO uint32_t  IF2CMSK;                         /*!< CAN IF2 Command Mask                                                  */
  };
  __IO uint32_t  IF2MSK1;                           /*!< CAN IF2 Mask 1                                                        */
  __IO uint32_t  IF2MSK2;                           /*!< CAN IF2 Mask 2                                                        */
  __IO uint32_t  IF2ARB1;                           /*!< CAN IF2 Arbitration 1                                                 */
  __IO uint32_t  IF2ARB2;                           /*!< CAN IF2 Arbitration 2                                                 */
  __IO uint32_t  IF2MCTL;                           /*!< CAN IF2 Message Control                                               */
  __IO uint32_t  IF2DA1;                            /*!< CAN IF2 Data A1                                                       */
  __IO uint32_t  IF2DA2;                            /*!< CAN IF2 Data A2                                                       */
  __IO uint32_t  IF2DB1;                            /*!< CAN IF2 Data B1                                                       */
  __IO uint32_t  IF2DB2;                            /*!< CAN IF2 Data B2                                                       */
  __I  uint32_t  RESERVED2[21];
  __IO uint32_t  TXRQ1;                             /*!< CAN Transmission Request 1                                            */
  __IO uint32_t  TXRQ2;                             /*!< CAN Transmission Request 2                                            */
  __I  uint32_t  RESERVED3[6];
  __IO uint32_t  NWDA1;                             /*!< CAN New Data 1                                                        */
  __IO uint32_t  NWDA2;                             /*!< CAN New Data 2                                                        */
  __I  uint32_t  RESERVED4[6];
  __IO uint32_t  MSG1INT;                           /*!< CAN Message 1 Interrupt Pending                                       */
  __IO uint32_t  MSG2INT;                           /*!< CAN Message 2 Interrupt Pending                                       */
  __I  uint32_t  RESERVED5[6];
  __IO uint32_t  MSG1VAL;                           /*!< CAN Message 1 Valid                                                   */
  __IO uint32_t  MSG2VAL;                           /*!< CAN Message 2 Valid                                                   */
} CAN0_Type;


/* ================================================================================ */
/* ================                      USB0                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for USB0 peripheral (USB0)
  */

typedef struct {                                    /*!< USB0 Structure                                                        */
  __IO uint8_t   FADDR;                             /*!< USB Device Functional Address                                         */
  __IO uint8_t   POWER;                             /*!< USB Power                                                             */
  __IO uint16_t  TXIS;                              /*!< USB Transmit Interrupt Status                                         */
  __IO uint16_t  RXIS;                              /*!< USB Receive Interrupt Status                                          */
  __IO uint16_t  TXIE;                              /*!< USB Transmit Interrupt Enable                                         */
  __IO uint16_t  RXIE;                              /*!< USB Receive Interrupt Enable                                          */
  
  union {
    __IO uint8_t   IS_USB0_ALT;                     /*!< USB General Interrupt Status                                          */
    __IO uint8_t   IS;                              /*!< USB General Interrupt Status                                          */
  };
  
  union {
    __IO uint8_t   IE_USB0_ALT;                     /*!< USB Interrupt Enable                                                  */
    __IO uint8_t   IE;                              /*!< USB Interrupt Enable                                                  */
  };
  __IO uint16_t  FRAME;                             /*!< USB Frame Value                                                       */
  __IO uint8_t   EPIDX;                             /*!< USB Endpoint Index                                                    */
  __IO uint8_t   TEST;                              /*!< USB Test Mode                                                         */
  __I  uint32_t  RESERVED0[4];
  __IO uint32_t  FIFO0;                             /*!< USB FIFO Endpoint 0                                                   */
  __IO uint32_t  FIFO1;                             /*!< USB FIFO Endpoint 1                                                   */
  __IO uint32_t  FIFO2;                             /*!< USB FIFO Endpoint 2                                                   */
  __IO uint32_t  FIFO3;                             /*!< USB FIFO Endpoint 3                                                   */
  __IO uint32_t  FIFO4;                             /*!< USB FIFO Endpoint 4                                                   */
  __IO uint32_t  FIFO5;                             /*!< USB FIFO Endpoint 5                                                   */
  __IO uint32_t  FIFO6;                             /*!< USB FIFO Endpoint 6                                                   */
  __IO uint32_t  FIFO7;                             /*!< USB FIFO Endpoint 7                                                   */
  __I  uint32_t  RESERVED1[8];
  __IO uint8_t   DEVCTL;                            /*!< USB Device Control                                                    */
  __I  uint8_t   RESERVED2[1];
  __IO uint8_t   TXFIFOSZ;                          /*!< USB Transmit Dynamic FIFO Sizing                                      */
  __IO uint8_t   RXFIFOSZ;                          /*!< USB Receive Dynamic FIFO Sizing                                       */
  __IO uint16_t  TXFIFOADD;                         /*!< USB Transmit FIFO Start Address                                       */
  __IO uint16_t  RXFIFOADD;                         /*!< USB Receive FIFO Start Address                                        */
  __I  uint32_t  RESERVED3[4];
  __I  uint16_t  RESERVED4;
  __IO uint8_t   CONTIM;                            /*!< USB Connect Timing                                                    */
  __IO uint8_t   VPLEN;                             /*!< USB OTG VBUS Pulse Timing                                             */
  __I  uint8_t   RESERVED5[1];
  __IO uint8_t   FSEOF;                             /*!< USB Full-Speed Last Transaction to End of Frame Timing                */
  __IO uint8_t   LSEOF;                             /*!< USB Low-Speed Last Transaction to End of Frame Timing                 */
  __I  uint8_t   RESERVED6[1];
  __IO uint8_t   TXFUNCADDR0;                       /*!< USB Transmit Functional Address Endpoint 0                            */
  __I  uint8_t   RESERVED7[1];
  __IO uint8_t   TXHUBADDR0;                        /*!< USB Transmit Hub Address Endpoint 0                                   */
  __IO uint8_t   TXHUBPORT0;                        /*!< USB Transmit Hub Port Endpoint 0                                      */
  __I  uint32_t  RESERVED8;
  __IO uint8_t   TXFUNCADDR1;                       /*!< USB Transmit Functional Address Endpoint 1                            */
  __I  uint8_t   RESERVED9[1];
  __IO uint8_t   TXHUBADDR1;                        /*!< USB Transmit Hub Address Endpoint 1                                   */
  __IO uint8_t   TXHUBPORT1;                        /*!< USB Transmit Hub Port Endpoint 1                                      */
  __IO uint8_t   RXFUNCADDR1;                       /*!< USB Receive Functional Address Endpoint 1                             */
  __I  uint8_t   RESERVED10[1];
  __IO uint8_t   RXHUBADDR1;                        /*!< USB Receive Hub Address Endpoint 1                                    */
  __IO uint8_t   RXHUBPORT1;                        /*!< USB Receive Hub Port Endpoint 1                                       */
  __IO uint8_t   TXFUNCADDR2;                       /*!< USB Transmit Functional Address Endpoint 2                            */
  __I  uint8_t   RESERVED11[1];
  __IO uint8_t   TXHUBADDR2;                        /*!< USB Transmit Hub Address Endpoint 2                                   */
  __IO uint8_t   TXHUBPORT2;                        /*!< USB Transmit Hub Port Endpoint 2                                      */
  __IO uint8_t   RXFUNCADDR2;                       /*!< USB Receive Functional Address Endpoint 2                             */
  __I  uint8_t   RESERVED12[1];
  __IO uint8_t   RXHUBADDR2;                        /*!< USB Receive Hub Address Endpoint 2                                    */
  __IO uint8_t   RXHUBPORT2;                        /*!< USB Receive Hub Port Endpoint 2                                       */
  __IO uint8_t   TXFUNCADDR3;                       /*!< USB Transmit Functional Address Endpoint 3                            */
  __I  uint8_t   RESERVED13[1];
  __IO uint8_t   TXHUBADDR3;                        /*!< USB Transmit Hub Address Endpoint 3                                   */
  __IO uint8_t   TXHUBPORT3;                        /*!< USB Transmit Hub Port Endpoint 3                                      */
  __IO uint8_t   RXFUNCADDR3;                       /*!< USB Receive Functional Address Endpoint 3                             */
  __I  uint8_t   RESERVED14[1];
  __IO uint8_t   RXHUBADDR3;                        /*!< USB Receive Hub Address Endpoint 3                                    */
  __IO uint8_t   RXHUBPORT3;                        /*!< USB Receive Hub Port Endpoint 3                                       */
  __IO uint8_t   TXFUNCADDR4;                       /*!< USB Transmit Functional Address Endpoint 4                            */
  __I  uint8_t   RESERVED15[1];
  __IO uint8_t   TXHUBADDR4;                        /*!< USB Transmit Hub Address Endpoint 4                                   */
  __IO uint8_t   TXHUBPORT4;                        /*!< USB Transmit Hub Port Endpoint 4                                      */
  __IO uint8_t   RXFUNCADDR4;                       /*!< USB Receive Functional Address Endpoint 4                             */
  __I  uint8_t   RESERVED16[1];
  __IO uint8_t   RXHUBADDR4;                        /*!< USB Receive Hub Address Endpoint 4                                    */
  __IO uint8_t   RXHUBPORT4;                        /*!< USB Receive Hub Port Endpoint 4                                       */
  __IO uint8_t   TXFUNCADDR5;                       /*!< USB Transmit Functional Address Endpoint 5                            */
  __I  uint8_t   RESERVED17[1];
  __IO uint8_t   TXHUBADDR5;                        /*!< USB Transmit Hub Address Endpoint 5                                   */
  __IO uint8_t   TXHUBPORT5;                        /*!< USB Transmit Hub Port Endpoint 5                                      */
  __IO uint8_t   RXFUNCADDR5;                       /*!< USB Receive Functional Address Endpoint 5                             */
  __I  uint8_t   RESERVED18[1];
  __IO uint8_t   RXHUBADDR5;                        /*!< USB Receive Hub Address Endpoint 5                                    */
  __IO uint8_t   RXHUBPORT5;                        /*!< USB Receive Hub Port Endpoint 5                                       */
  __IO uint8_t   TXFUNCADDR6;                       /*!< USB Transmit Functional Address Endpoint 6                            */
  __I  uint8_t   RESERVED19[1];
  __IO uint8_t   TXHUBADDR6;                        /*!< USB Transmit Hub Address Endpoint 6                                   */
  __IO uint8_t   TXHUBPORT6;                        /*!< USB Transmit Hub Port Endpoint 6                                      */
  __IO uint8_t   RXFUNCADDR6;                       /*!< USB Receive Functional Address Endpoint 6                             */
  __I  uint8_t   RESERVED20[1];
  __IO uint8_t   RXHUBADDR6;                        /*!< USB Receive Hub Address Endpoint 6                                    */
  __IO uint8_t   RXHUBPORT6;                        /*!< USB Receive Hub Port Endpoint 6                                       */
  __IO uint8_t   TXFUNCADDR7;                       /*!< USB Transmit Functional Address Endpoint 7                            */
  __I  uint8_t   RESERVED21[1];
  __IO uint8_t   TXHUBADDR7;                        /*!< USB Transmit Hub Address Endpoint 7                                   */
  __IO uint8_t   TXHUBPORT7;                        /*!< USB Transmit Hub Port Endpoint 7                                      */
  __IO uint8_t   RXFUNCADDR7;                       /*!< USB Receive Functional Address Endpoint 7                             */
  __I  uint8_t   RESERVED22[1];
  __IO uint8_t   RXHUBADDR7;                        /*!< USB Receive Hub Address Endpoint 7                                    */
  __IO uint8_t   RXHUBPORT7;                        /*!< USB Receive Hub Port Endpoint 7                                       */
  __I  uint32_t  RESERVED23[16];
  __I  uint16_t  RESERVED24;
  
  union {
    __O  uint8_t   CSRL0_USB0_ALT;                  /*!< USB Control and Status Endpoint 0 Low                                 */
    __O  uint8_t   CSRL0;                           /*!< USB Control and Status Endpoint 0 Low                                 */
  };
  __O  uint8_t   CSRH0;                             /*!< USB Control and Status Endpoint 0 High                                */
  __I  uint16_t  RESERVED25[3];
  __IO uint8_t   COUNT0;                            /*!< USB Receive Byte Count Endpoint 0                                     */
  __I  uint8_t   RESERVED26[1];
  __IO uint8_t   TYPE0;                             /*!< USB Type Endpoint 0                                                   */
  __IO uint8_t   NAKLMT;                            /*!< USB NAK Limit                                                         */
  __I  uint32_t  RESERVED27;
  __IO uint16_t  TXMAXP1;                           /*!< USB Maximum Transmit Data Endpoint 1                                  */
  
  union {
    __IO uint8_t   TXCSRL1_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 1 Low                        */
    __IO uint8_t   TXCSRL1;                         /*!< USB Transmit Control and Status Endpoint 1 Low                        */
  };
  __IO uint8_t   TXCSRH1;                           /*!< USB Transmit Control and Status Endpoint 1 High                       */
  __IO uint16_t  RXMAXP1;                           /*!< USB Maximum Receive Data Endpoint 1                                   */
  
  union {
    __IO uint8_t   RXCSRL1_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 1 Low                         */
    __IO uint8_t   RXCSRL1;                         /*!< USB Receive Control and Status Endpoint 1 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH1_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 1 High                        */
    __IO uint8_t   RXCSRH1;                         /*!< USB Receive Control and Status Endpoint 1 High                        */
  };
  __IO uint16_t  RXCOUNT1;                          /*!< USB Receive Byte Count Endpoint 1                                     */
  __IO uint8_t   TXTYPE1;                           /*!< USB Host Transmit Configure Type Endpoint 1                           */
  
  union {
    __IO uint8_t   TXINTERVAL1_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 1                                 */
    __IO uint8_t   TXINTERVAL1;                     /*!< USB Host Transmit Interval Endpoint 1                                 */
  };
  __IO uint8_t   RXTYPE1;                           /*!< USB Host Configure Receive Type Endpoint 1                            */
  
  union {
    __IO uint8_t   RXINTERVAL1_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 1                          */
    __IO uint8_t   RXINTERVAL1;                     /*!< USB Host Receive Polling Interval Endpoint 1                          */
  };
  __I  uint16_t  RESERVED28;
  __IO uint16_t  TXMAXP2;                           /*!< USB Maximum Transmit Data Endpoint 2                                  */
  
  union {
    __IO uint8_t   TXCSRL2_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 2 Low                        */
    __IO uint8_t   TXCSRL2;                         /*!< USB Transmit Control and Status Endpoint 2 Low                        */
  };
  __IO uint8_t   TXCSRH2;                           /*!< USB Transmit Control and Status Endpoint 2 High                       */
  __IO uint16_t  RXMAXP2;                           /*!< USB Maximum Receive Data Endpoint 2                                   */
  
  union {
    __IO uint8_t   RXCSRL2_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 2 Low                         */
    __IO uint8_t   RXCSRL2;                         /*!< USB Receive Control and Status Endpoint 2 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH2_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 2 High                        */
    __IO uint8_t   RXCSRH2;                         /*!< USB Receive Control and Status Endpoint 2 High                        */
  };
  __IO uint16_t  RXCOUNT2;                          /*!< USB Receive Byte Count Endpoint 2                                     */
  __IO uint8_t   TXTYPE2;                           /*!< USB Host Transmit Configure Type Endpoint 2                           */
  
  union {
    __IO uint8_t   TXINTERVAL2_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 2                                 */
    __IO uint8_t   TXINTERVAL2;                     /*!< USB Host Transmit Interval Endpoint 2                                 */
  };
  __IO uint8_t   RXTYPE2;                           /*!< USB Host Configure Receive Type Endpoint 2                            */
  
  union {
    __IO uint8_t   RXINTERVAL2_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 2                          */
    __IO uint8_t   RXINTERVAL2;                     /*!< USB Host Receive Polling Interval Endpoint 2                          */
  };
  __I  uint16_t  RESERVED29;
  __IO uint16_t  TXMAXP3;                           /*!< USB Maximum Transmit Data Endpoint 3                                  */
  
  union {
    __IO uint8_t   TXCSRL3_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 3 Low                        */
    __IO uint8_t   TXCSRL3;                         /*!< USB Transmit Control and Status Endpoint 3 Low                        */
  };
  __IO uint8_t   TXCSRH3;                           /*!< USB Transmit Control and Status Endpoint 3 High                       */
  __IO uint16_t  RXMAXP3;                           /*!< USB Maximum Receive Data Endpoint 3                                   */
  
  union {
    __IO uint8_t   RXCSRL3_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 3 Low                         */
    __IO uint8_t   RXCSRL3;                         /*!< USB Receive Control and Status Endpoint 3 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH3_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 3 High                        */
    __IO uint8_t   RXCSRH3;                         /*!< USB Receive Control and Status Endpoint 3 High                        */
  };
  __IO uint16_t  RXCOUNT3;                          /*!< USB Receive Byte Count Endpoint 3                                     */
  __IO uint8_t   TXTYPE3;                           /*!< USB Host Transmit Configure Type Endpoint 3                           */
  
  union {
    __IO uint8_t   TXINTERVAL3_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 3                                 */
    __IO uint8_t   TXINTERVAL3;                     /*!< USB Host Transmit Interval Endpoint 3                                 */
  };
  __IO uint8_t   RXTYPE3;                           /*!< USB Host Configure Receive Type Endpoint 3                            */
  
  union {
    __IO uint8_t   RXINTERVAL3_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 3                          */
    __IO uint8_t   RXINTERVAL3;                     /*!< USB Host Receive Polling Interval Endpoint 3                          */
  };
  __I  uint16_t  RESERVED30;
  __IO uint16_t  TXMAXP4;                           /*!< USB Maximum Transmit Data Endpoint 4                                  */
  
  union {
    __IO uint8_t   TXCSRL4_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 4 Low                        */
    __IO uint8_t   TXCSRL4;                         /*!< USB Transmit Control and Status Endpoint 4 Low                        */
  };
  __IO uint8_t   TXCSRH4;                           /*!< USB Transmit Control and Status Endpoint 4 High                       */
  __IO uint16_t  RXMAXP4;                           /*!< USB Maximum Receive Data Endpoint 4                                   */
  
  union {
    __IO uint8_t   RXCSRL4_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 4 Low                         */
    __IO uint8_t   RXCSRL4;                         /*!< USB Receive Control and Status Endpoint 4 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH4_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 4 High                        */
    __IO uint8_t   RXCSRH4;                         /*!< USB Receive Control and Status Endpoint 4 High                        */
  };
  __IO uint16_t  RXCOUNT4;                          /*!< USB Receive Byte Count Endpoint 4                                     */
  __IO uint8_t   TXTYPE4;                           /*!< USB Host Transmit Configure Type Endpoint 4                           */
  
  union {
    __IO uint8_t   TXINTERVAL4_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 4                                 */
    __IO uint8_t   TXINTERVAL4;                     /*!< USB Host Transmit Interval Endpoint 4                                 */
  };
  __IO uint8_t   RXTYPE4;                           /*!< USB Host Configure Receive Type Endpoint 4                            */
  
  union {
    __IO uint8_t   RXINTERVAL4_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 4                          */
    __IO uint8_t   RXINTERVAL4;                     /*!< USB Host Receive Polling Interval Endpoint 4                          */
  };
  __I  uint16_t  RESERVED31;
  __IO uint16_t  TXMAXP5;                           /*!< USB Maximum Transmit Data Endpoint 5                                  */
  
  union {
    __IO uint8_t   TXCSRL5_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 5 Low                        */
    __IO uint8_t   TXCSRL5;                         /*!< USB Transmit Control and Status Endpoint 5 Low                        */
  };
  __IO uint8_t   TXCSRH5;                           /*!< USB Transmit Control and Status Endpoint 5 High                       */
  __IO uint16_t  RXMAXP5;                           /*!< USB Maximum Receive Data Endpoint 5                                   */
  
  union {
    __IO uint8_t   RXCSRL5_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 5 Low                         */
    __IO uint8_t   RXCSRL5;                         /*!< USB Receive Control and Status Endpoint 5 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH5_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 5 High                        */
    __IO uint8_t   RXCSRH5;                         /*!< USB Receive Control and Status Endpoint 5 High                        */
  };
  __IO uint16_t  RXCOUNT5;                          /*!< USB Receive Byte Count Endpoint 5                                     */
  __IO uint8_t   TXTYPE5;                           /*!< USB Host Transmit Configure Type Endpoint 5                           */
  
  union {
    __IO uint8_t   TXINTERVAL5_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 5                                 */
    __IO uint8_t   TXINTERVAL5;                     /*!< USB Host Transmit Interval Endpoint 5                                 */
  };
  __IO uint8_t   RXTYPE5;                           /*!< USB Host Configure Receive Type Endpoint 5                            */
  
  union {
    __IO uint8_t   RXINTERVAL5_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 5                          */
    __IO uint8_t   RXINTERVAL5;                     /*!< USB Host Receive Polling Interval Endpoint 5                          */
  };
  __I  uint16_t  RESERVED32;
  __IO uint16_t  TXMAXP6;                           /*!< USB Maximum Transmit Data Endpoint 6                                  */
  
  union {
    __IO uint8_t   TXCSRL6_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 6 Low                        */
    __IO uint8_t   TXCSRL6;                         /*!< USB Transmit Control and Status Endpoint 6 Low                        */
  };
  __IO uint8_t   TXCSRH6;                           /*!< USB Transmit Control and Status Endpoint 6 High                       */
  __IO uint16_t  RXMAXP6;                           /*!< USB Maximum Receive Data Endpoint 6                                   */
  
  union {
    __IO uint8_t   RXCSRL6_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 6 Low                         */
    __IO uint8_t   RXCSRL6;                         /*!< USB Receive Control and Status Endpoint 6 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH6_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 6 High                        */
    __IO uint8_t   RXCSRH6;                         /*!< USB Receive Control and Status Endpoint 6 High                        */
  };
  __IO uint16_t  RXCOUNT6;                          /*!< USB Receive Byte Count Endpoint 6                                     */
  __IO uint8_t   TXTYPE6;                           /*!< USB Host Transmit Configure Type Endpoint 6                           */
  
  union {
    __IO uint8_t   TXINTERVAL6_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 6                                 */
    __IO uint8_t   TXINTERVAL6;                     /*!< USB Host Transmit Interval Endpoint 6                                 */
  };
  __IO uint8_t   RXTYPE6;                           /*!< USB Host Configure Receive Type Endpoint 6                            */
  
  union {
    __IO uint8_t   RXINTERVAL6_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 6                          */
    __IO uint8_t   RXINTERVAL6;                     /*!< USB Host Receive Polling Interval Endpoint 6                          */
  };
  __I  uint16_t  RESERVED33;
  __IO uint16_t  TXMAXP7;                           /*!< USB Maximum Transmit Data Endpoint 7                                  */
  
  union {
    __IO uint8_t   TXCSRL7_USB0_ALT;                /*!< USB Transmit Control and Status Endpoint 7 Low                        */
    __IO uint8_t   TXCSRL7;                         /*!< USB Transmit Control and Status Endpoint 7 Low                        */
  };
  __IO uint8_t   TXCSRH7;                           /*!< USB Transmit Control and Status Endpoint 7 High                       */
  __IO uint16_t  RXMAXP7;                           /*!< USB Maximum Receive Data Endpoint 7                                   */
  
  union {
    __IO uint8_t   RXCSRL7_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 7 Low                         */
    __IO uint8_t   RXCSRL7;                         /*!< USB Receive Control and Status Endpoint 7 Low                         */
  };
  
  union {
    __IO uint8_t   RXCSRH7_USB0_ALT;                /*!< USB Receive Control and Status Endpoint 7 High                        */
    __IO uint8_t   RXCSRH7;                         /*!< USB Receive Control and Status Endpoint 7 High                        */
  };
  __IO uint16_t  RXCOUNT7;                          /*!< USB Receive Byte Count Endpoint 7                                     */
  __IO uint8_t   TXTYPE7;                           /*!< USB Host Transmit Configure Type Endpoint 7                           */
  
  union {
    __IO uint8_t   TXINTERVAL7_USB0_ALT;            /*!< USB Host Transmit Interval Endpoint 7                                 */
    __IO uint8_t   TXINTERVAL7;                     /*!< USB Host Transmit Interval Endpoint 7                                 */
  };
  __IO uint8_t   RXTYPE7;                           /*!< USB Host Configure Receive Type Endpoint 7                            */
  
  union {
    __IO uint8_t   RXINTERVAL7_USB0_ALT;            /*!< USB Host Receive Polling Interval Endpoint 7                          */
    __IO uint8_t   RXINTERVAL7;                     /*!< USB Host Receive Polling Interval Endpoint 7                          */
  };
  __I  uint16_t  RESERVED34[195];
  __IO uint16_t  RQPKTCOUNT1;                       /*!< USB Request Packet Count in Block Transfer Endpoint 1                 */
  __I  uint16_t  RESERVED35;
  __IO uint16_t  RQPKTCOUNT2;                       /*!< USB Request Packet Count in Block Transfer Endpoint 2                 */
  __I  uint16_t  RESERVED36;
  __IO uint16_t  RQPKTCOUNT3;                       /*!< USB Request Packet Count in Block Transfer Endpoint 3                 */
  __I  uint16_t  RESERVED37;
  __IO uint16_t  RQPKTCOUNT4;                       /*!< USB Request Packet Count in Block Transfer Endpoint 4                 */
  __I  uint16_t  RESERVED38;
  __IO uint16_t  RQPKTCOUNT5;                       /*!< USB Request Packet Count in Block Transfer Endpoint 5                 */
  __I  uint16_t  RESERVED39;
  __IO uint16_t  RQPKTCOUNT6;                       /*!< USB Request Packet Count in Block Transfer Endpoint 6                 */
  __I  uint16_t  RESERVED40;
  __IO uint16_t  RQPKTCOUNT7;                       /*!< USB Request Packet Count in Block Transfer Endpoint 7                 */
  __I  uint16_t  RESERVED41[17];
  __IO uint16_t  RXDPKTBUFDIS;                      /*!< USB Receive Double Packet Buffer Disable                              */
  __IO uint16_t  TXDPKTBUFDIS;                      /*!< USB Transmit Double Packet Buffer Disable                             */
  __I  uint32_t  RESERVED42[47];
  __IO uint32_t  EPC;                               /*!< USB External Power Control                                            */
  __IO uint32_t  EPCRIS;                            /*!< USB External Power Control Raw Interrupt Status                       */
  __IO uint32_t  EPCIM;                             /*!< USB External Power Control Interrupt Mask                             */
  __IO uint32_t  EPCISC;                            /*!< USB External Power Control Interrupt Status and Clear                 */
  __IO uint32_t  DRRIS;                             /*!< USB Device RESUME Raw Interrupt Status                                */
  __IO uint32_t  DRIM;                              /*!< USB Device RESUME Interrupt Mask                                      */
  __O  uint32_t  DRISC;                             /*!< USB Device RESUME Interrupt Status and Clear                          */
  __IO uint32_t  GPCS;                              /*!< USB General-Purpose Control and Status                                */
  __I  uint32_t  RESERVED43[4];
  __IO uint32_t  VDC;                               /*!< USB VBUS Droop Control                                                */
  __IO uint32_t  VDCRIS;                            /*!< USB VBUS Droop Control Raw Interrupt Status                           */
  __IO uint32_t  VDCIM;                             /*!< USB VBUS Droop Control Interrupt Mask                                 */
  __IO uint32_t  VDCISC;                            /*!< USB VBUS Droop Control Interrupt Status and Clear                     */
  __I  uint32_t  RESERVED44;
  __IO uint32_t  IDVRIS;                            /*!< USB ID Valid Detect Raw Interrupt Status                              */
  __IO uint32_t  IDVIM;                             /*!< USB ID Valid Detect Interrupt Mask                                    */
  __IO uint32_t  IDVISC;                            /*!< USB ID Valid Detect Interrupt Status and Clear                        */
  __IO uint32_t  DMASEL;                            /*!< USB DMA Select                                                        */
  __I  uint32_t  RESERVED45[731];
  __IO uint32_t  PP;                                /*!< USB Peripheral Properties                                             */
} USB0_Type;


/* ================================================================================ */
/* ================                     EEPROM                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for EEPROM peripheral (EEPROM)
  */

typedef struct {                                    /*!< EEPROM Structure                                                      */
  __IO uint32_t  EESIZE;                            /*!< EEPROM Size Information                                               */
  __IO uint32_t  EEBLOCK;                           /*!< EEPROM Current Block                                                  */
  __IO uint32_t  EEOFFSET;                          /*!< EEPROM Current Offset                                                 */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  EERDWR;                            /*!< EEPROM Read-Write                                                     */
  __IO uint32_t  EERDWRINC;                         /*!< EEPROM Read-Write with Increment                                      */
  __IO uint32_t  EEDONE;                            /*!< EEPROM Done Status                                                    */
  __IO uint32_t  EESUPP;                            /*!< EEPROM Support Control and Status                                     */
  __IO uint32_t  EEUNLOCK;                          /*!< EEPROM Unlock                                                         */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  EEPROT;                            /*!< EEPROM Protection                                                     */
  __IO uint32_t  EEPASS0;                           /*!< EEPROM Password                                                       */
  __IO uint32_t  EEPASS1;                           /*!< EEPROM Password                                                       */
  __IO uint32_t  EEPASS2;                           /*!< EEPROM Password                                                       */
  __IO uint32_t  EEINT;                             /*!< EEPROM Interrupt                                                      */
  __I  uint32_t  RESERVED2[3];
  __IO uint32_t  EEHIDE;                            /*!< EEPROM Block Hide                                                     */
  __I  uint32_t  RESERVED3[11];
  __IO uint32_t  EEDBGME;                           /*!< EEPROM Debug Mass Erase                                               */
  __I  uint32_t  RESERVED4[975];
  __IO uint32_t  PP;                                /*!< EEPROM Peripheral Properties                                          */
} EEPROM_Type;


/* ================================================================================ */
/* ================                     SYSEXC                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for SYSEXC peripheral (SYSEXC)
  */

typedef struct {                                    /*!< SYSEXC Structure                                                      */
  __IO uint32_t  RIS;                               /*!< System Exception Raw Interrupt Status                                 */
  __IO uint32_t  IM;                                /*!< System Exception Interrupt Mask                                       */
  __IO uint32_t  MIS;                               /*!< System Exception Masked Interrupt Status                              */
  __O  uint32_t  IC;                                /*!< System Exception Interrupt Clear                                      */
} SYSEXC_Type;


/* ================================================================================ */
/* ================                       HIB                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for HIB peripheral (HIB)
  */

typedef struct {                                    /*!< HIB Structure                                                         */
  __IO uint32_t  RTCC;                              /*!< Hibernation RTC Counter                                               */
  __IO uint32_t  RTCM0;                             /*!< Hibernation RTC Match 0                                               */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  RTCLD;                             /*!< Hibernation RTC Load                                                  */
  __IO uint32_t  CTL;                               /*!< Hibernation Control                                                   */
  __IO uint32_t  IM;                                /*!< Hibernation Interrupt Mask                                            */
  __IO uint32_t  RIS;                               /*!< Hibernation Raw Interrupt Status                                      */
  __IO uint32_t  MIS;                               /*!< Hibernation Masked Interrupt Status                                   */
  __IO uint32_t  IC;                                /*!< Hibernation Interrupt Clear                                           */
  __IO uint32_t  RTCT;                              /*!< Hibernation RTC Trim                                                  */
  __IO uint32_t  RTCSS;                             /*!< Hibernation RTC Sub Seconds                                           */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  DATA;                              /*!< Hibernation Data                                                      */
} HIB_Type;


/* ================================================================================ */
/* ================                   FLASH_CTRL                   ================ */
/* ================================================================================ */


/**
  * @brief Register map for FLASH_CTRL peripheral (FLASH_CTRL)
  */

typedef struct {                                    /*!< FLASH_CTRL Structure                                                  */
  __IO uint32_t  FMA;                               /*!< Flash Memory Address                                                  */
  __IO uint32_t  FMD;                               /*!< Flash Memory Data                                                     */
  __IO uint32_t  FMC;                               /*!< Flash Memory Control                                                  */
  __IO uint32_t  FCRIS;                             /*!< Flash Controller Raw Interrupt Status                                 */
  __IO uint32_t  FCIM;                              /*!< Flash Controller Interrupt Mask                                       */
  __IO uint32_t  FCMISC;                            /*!< Flash Controller Masked Interrupt Status and Clear                    */
  __I  uint32_t  RESERVED0[2];
  __IO uint32_t  FMC2;                              /*!< Flash Memory Control 2                                                */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  FWBVAL;                            /*!< Flash Write Buffer Valid                                              */
  __I  uint32_t  RESERVED2[51];
  __IO uint32_t  FWBN;                              /*!< Flash Write Buffer n                                                  */
  __I  uint32_t  RESERVED3[943];
  __IO uint32_t  FSIZE;                             /*!< Flash Size                                                            */
  __IO uint32_t  SSIZE;                             /*!< SRAM Size                                                             */
  __I  uint32_t  RESERVED4;
  
  union {
    __IO uint32_t  ROMSWMAP_FLASH_CTRL_ALT;         /*!< ROM Software Map                                                      */
    __IO uint32_t  ROMSWMAP;                        /*!< ROM Software Map                                                      */
  };
  __I  uint32_t  RESERVED5[72];
  __IO uint32_t  RMCTL;                             /*!< ROM Control                                                           */
  __I  uint32_t  RESERVED6[55];
  __IO uint32_t  BOOTCFG;                           /*!< Boot Configuration                                                    */
  __I  uint32_t  RESERVED7[3];
  __IO uint32_t  USERREG0;                          /*!< User Register 0                                                       */
  __IO uint32_t  USERREG1;                          /*!< User Register 1                                                       */
  __IO uint32_t  USERREG2;                          /*!< User Register 2                                                       */
  __IO uint32_t  USERREG3;                          /*!< User Register 3                                                       */
  __I  uint32_t  RESERVED8[4];
  __IO uint32_t  FMPRE0;                            /*!< Flash Memory Protection Read Enable 0                                 */
  __IO uint32_t  FMPRE1;                            /*!< Flash Memory Protection Read Enable 1                                 */
  __IO uint32_t  FMPRE2;                            /*!< Flash Memory Protection Read Enable 2                                 */
  __IO uint32_t  FMPRE3;                            /*!< Flash Memory Protection Read Enable 3                                 */
  __I  uint32_t  RESERVED9[124];
  __IO uint32_t  FMPPE0;                            /*!< Flash Memory Protection Program Enable 0                              */
  __IO uint32_t  FMPPE1;                            /*!< Flash Memory Protection Program Enable 1                              */
  __IO uint32_t  FMPPE2;                            /*!< Flash Memory Protection Program Enable 2                              */
  __IO uint32_t  FMPPE3;                            /*!< Flash Memory Protection Program Enable 3                              */
} FLASH_CTRL_Type;


/* ================================================================================ */
/* ================                     SYSCTL                     ================ */
/* ================================================================================ */


/**
  * @brief Register map for SYSCTL peripheral (SYSCTL)
  */

typedef struct {                                    /*!< SYSCTL Structure                                                      */
  __IO uint32_t  DID0;                              /*!< Device Identification 0                                               */
  __IO uint32_t  DID1;                              /*!< Device Identification 1                                               */
  __IO uint32_t  DC0;                               /*!< Device Capabilities 0                                                 */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  DC1;                               /*!< Device Capabilities 1                                                 */
  __IO uint32_t  DC2;                               /*!< Device Capabilities 2                                                 */
  __IO uint32_t  DC3;                               /*!< Device Capabilities 3                                                 */
  __IO uint32_t  DC4;                               /*!< Device Capabilities 4                                                 */
  __IO uint32_t  DC5;                               /*!< Device Capabilities 5                                                 */
  __IO uint32_t  DC6;                               /*!< Device Capabilities 6                                                 */
  __IO uint32_t  DC7;                               /*!< Device Capabilities 7                                                 */
  __IO uint32_t  DC8;                               /*!< Device Capabilities 8                                                 */
  __IO uint32_t  PBORCTL;                           /*!< Brown-Out Reset Control                                               */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  SRCR0;                             /*!< Software Reset Control 0                                              */
  __IO uint32_t  SRCR1;                             /*!< Software Reset Control 1                                              */
  __IO uint32_t  SRCR2;                             /*!< Software Reset Control 2                                              */
  __I  uint32_t  RESERVED2;
  __IO uint32_t  RIS;                               /*!< Raw Interrupt Status                                                  */
  __IO uint32_t  IMC;                               /*!< Interrupt Mask Control                                                */
  __IO uint32_t  MISC;                              /*!< Masked Interrupt Status and Clear                                     */
  __IO uint32_t  RESC;                              /*!< Reset Cause                                                           */
  __IO uint32_t  RCC;                               /*!< Run-Mode Clock Configuration                                          */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  GPIOHBCTL;                         /*!< GPIO High-Performance Bus Control                                     */
  __IO uint32_t  RCC2;                              /*!< Run-Mode Clock Configuration 2                                        */
  __I  uint32_t  RESERVED4[2];
  __IO uint32_t  MOSCCTL;                           /*!< Main Oscillator Control                                               */
  __I  uint32_t  RESERVED5[32];
  __IO uint32_t  RCGC0;                             /*!< Run Mode Clock Gating Control Register 0                              */
  __IO uint32_t  RCGC1;                             /*!< Run Mode Clock Gating Control Register 1                              */
  __IO uint32_t  RCGC2;                             /*!< Run Mode Clock Gating Control Register 2                              */
  __I  uint32_t  RESERVED6;
  __IO uint32_t  SCGC0;                             /*!< Sleep Mode Clock Gating Control Register 0                            */
  __IO uint32_t  SCGC1;                             /*!< Sleep Mode Clock Gating Control Register 1                            */
  __IO uint32_t  SCGC2;                             /*!< Sleep Mode Clock Gating Control Register 2                            */
  __I  uint32_t  RESERVED7;
  __IO uint32_t  DCGC0;                             /*!< Deep Sleep Mode Clock Gating Control Register 0                       */
  __IO uint32_t  DCGC1;                             /*!< Deep-Sleep Mode Clock Gating Control Register 1                       */
  __IO uint32_t  DCGC2;                             /*!< Deep Sleep Mode Clock Gating Control Register 2                       */
  __I  uint32_t  RESERVED8[6];
  __IO uint32_t  DSLPCLKCFG;                        /*!< Deep Sleep Clock Configuration                                        */
  __I  uint32_t  RESERVED9;
  __IO uint32_t  SYSPROP;                           /*!< System Properties                                                     */
  __IO uint32_t  PIOSCCAL;                          /*!< Precision Internal Oscillator Calibration                             */
  __IO uint32_t  PIOSCSTAT;                         /*!< Precision Internal Oscillator Statistics                              */
  __I  uint32_t  RESERVED10[2];
  __IO uint32_t  PLLFREQ0;                          /*!< PLL Frequency 0                                                       */
  __IO uint32_t  PLLFREQ1;                          /*!< PLL Frequency 1                                                       */
  __IO uint32_t  PLLSTAT;                           /*!< PLL Status                                                            */
  __I  uint32_t  RESERVED11[7];
  __IO uint32_t  SLPPWRCFG;                         /*!< Sleep Power Configuration                                             */
  __IO uint32_t  DSLPPWRCFG;                        /*!< Deep-Sleep Power Configuration                                        */
  __IO uint32_t  DC9;                               /*!< Device Capabilities 9                                                 */
  __I  uint32_t  RESERVED12[3];
  __IO uint32_t  NVMSTAT;                           /*!< Non-Volatile Memory Information                                       */
  __I  uint32_t  RESERVED13[4];
  __IO uint32_t  LDOSPCTL;                          /*!< LDO Sleep Power Control                                               */
  __I  uint32_t  RESERVED14;
  __IO uint32_t  LDODPCTL;                          /*!< LDO Deep-Sleep Power Control                                          */
  __I  uint32_t  RESERVED15[80];
  __IO uint32_t  PPWD;                              /*!< Watchdog Timer Peripheral Present                                     */
  __IO uint32_t  PPTIMER;                           /*!< 16/32-Bit General-Purpose Timer Peripheral Present                    */
  __IO uint32_t  PPGPIO;                            /*!< General-Purpose Input/Output Peripheral Present                       */
  __IO uint32_t  PPDMA;                             /*!< Micro Direct Memory Access Peripheral Present                         */
  __I  uint32_t  RESERVED16;
  __IO uint32_t  PPHIB;                             /*!< Hibernation Peripheral Present                                        */
  __IO uint32_t  PPUART;                            /*!< Universal Asynchronous Receiver/Transmitter Peripheral Present        */
  __IO uint32_t  PPSSI;                             /*!< Synchronous Serial Interface Peripheral Present                       */
  __IO uint32_t  PPI2C;                             /*!< Inter-Integrated Circuit Peripheral Present                           */
  __I  uint32_t  RESERVED17;
  __IO uint32_t  PPUSB;                             /*!< Universal Serial Bus Peripheral Present                               */
  __I  uint32_t  RESERVED18[2];
  __IO uint32_t  PPCAN;                             /*!< Controller Area Network Peripheral Present                            */
  __IO uint32_t  PPADC;                             /*!< Analog-to-Digital Converter Peripheral Present                        */
  __IO uint32_t  PPACMP;                            /*!< Analog Comparator Peripheral Present                                  */
  __IO uint32_t  PPPWM;                             /*!< Pulse Width Modulator Peripheral Present                              */
  __IO uint32_t  PPQEI;                             /*!< Quadrature Encoder Interface Peripheral Present                       */
  __I  uint32_t  RESERVED19[4];
  __IO uint32_t  PPEEPROM;                          /*!< EEPROM Peripheral Present                                             */
  __IO uint32_t  PPWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Peripheral Present               */
  __I  uint32_t  RESERVED20[104];
  __IO uint32_t  SRWD;                              /*!< Watchdog Timer Software Reset                                         */
  __IO uint32_t  SRTIMER;                           /*!< 16/32-Bit General-Purpose Timer Software Reset                        */
  __IO uint32_t  SRGPIO;                            /*!< General-Purpose Input/Output Software Reset                           */
  __IO uint32_t  SRDMA;                             /*!< Micro Direct Memory Access Software Reset                             */
  __I  uint32_t  RESERVED21;
  __IO uint32_t  SRHIB;                             /*!< Hibernation Software Reset                                            */
  __IO uint32_t  SRUART;                            /*!< Universal Asynchronous Receiver/Transmitter Software Reset            */
  __IO uint32_t  SRSSI;                             /*!< Synchronous Serial Interface Software Reset                           */
  __IO uint32_t  SRI2C;                             /*!< Inter-Integrated Circuit Software Reset                               */
  __I  uint32_t  RESERVED22;
  __IO uint32_t  SRUSB;                             /*!< Universal Serial Bus Software Reset                                   */
  __I  uint32_t  RESERVED23[2];
  __IO uint32_t  SRCAN;                             /*!< Controller Area Network Software Reset                                */
  __IO uint32_t  SRADC;                             /*!< Analog-to-Digital Converter Software Reset                            */
  __IO uint32_t  SRACMP;                            /*!< Analog Comparator Software Reset                                      */
  __IO uint32_t  SRPWM;                             /*!< Pulse Width Modulator Software Reset                                  */
  __IO uint32_t  SRQEI;                             /*!< Quadrature Encoder Interface Software Reset                           */
  __I  uint32_t  RESERVED24[4];
  __IO uint32_t  SREEPROM;                          /*!< EEPROM Software Reset                                                 */
  __IO uint32_t  SRWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Software Reset                   */
  __I  uint32_t  RESERVED25[40];
  __IO uint32_t  RCGCWD;                            /*!< Watchdog Timer Run Mode Clock Gating Control                          */
  __IO uint32_t  RCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Run Mode Clock Gating Control         */
  __IO uint32_t  RCGCGPIO;                          /*!< General-Purpose Input/Output Run Mode Clock Gating Control            */
  __IO uint32_t  RCGCDMA;                           /*!< Micro Direct Memory Access Run Mode Clock Gating Control              */
  __I  uint32_t  RESERVED26;
  __IO uint32_t  RCGCHIB;                           /*!< Hibernation Run Mode Clock Gating Control                             */
  __IO uint32_t  RCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Run Mode Clock Gating
                                                         Control                                                               */
  __IO uint32_t  RCGCSSI;                           /*!< Synchronous Serial Interface Run Mode Clock Gating Control            */
  __IO uint32_t  RCGCI2C;                           /*!< Inter-Integrated Circuit Run Mode Clock Gating Control                */
  __I  uint32_t  RESERVED27;
  __IO uint32_t  RCGCUSB;                           /*!< Universal Serial Bus Run Mode Clock Gating Control                    */
  __I  uint32_t  RESERVED28[2];
  __IO uint32_t  RCGCCAN;                           /*!< Controller Area Network Run Mode Clock Gating Control                 */
  __IO uint32_t  RCGCADC;                           /*!< Analog-to-Digital Converter Run Mode Clock Gating Control             */
  __IO uint32_t  RCGCACMP;                          /*!< Analog Comparator Run Mode Clock Gating Control                       */
  __IO uint32_t  RCGCPWM;                           /*!< Pulse Width Modulator Run Mode Clock Gating Control                   */
  __IO uint32_t  RCGCQEI;                           /*!< Quadrature Encoder Interface Run Mode Clock Gating Control            */
  __I  uint32_t  RESERVED29[4];
  __IO uint32_t  RCGCEEPROM;                        /*!< EEPROM Run Mode Clock Gating Control                                  */
  __IO uint32_t  RCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Run Mode Clock Gating Control    */
  __I  uint32_t  RESERVED30[40];
  __IO uint32_t  SCGCWD;                            /*!< Watchdog Timer Sleep Mode Clock Gating Control                        */
  __IO uint32_t  SCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Sleep Mode Clock Gating Control       */
  __IO uint32_t  SCGCGPIO;                          /*!< General-Purpose Input/Output Sleep Mode Clock Gating Control          */
  __IO uint32_t  SCGCDMA;                           /*!< Micro Direct Memory Access Sleep Mode Clock Gating Control            */
  __I  uint32_t  RESERVED31;
  __IO uint32_t  SCGCHIB;                           /*!< Hibernation Sleep Mode Clock Gating Control                           */
  __IO uint32_t  SCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Sleep Mode Clock
                                                         Gating Control                                                        */
  __IO uint32_t  SCGCSSI;                           /*!< Synchronous Serial Interface Sleep Mode Clock Gating Control          */
  __IO uint32_t  SCGCI2C;                           /*!< Inter-Integrated Circuit Sleep Mode Clock Gating Control              */
  __I  uint32_t  RESERVED32;
  __IO uint32_t  SCGCUSB;                           /*!< Universal Serial Bus Sleep Mode Clock Gating Control                  */
  __I  uint32_t  RESERVED33[2];
  __IO uint32_t  SCGCCAN;                           /*!< Controller Area Network Sleep Mode Clock Gating Control               */
  __IO uint32_t  SCGCADC;                           /*!< Analog-to-Digital Converter Sleep Mode Clock Gating Control           */
  __IO uint32_t  SCGCACMP;                          /*!< Analog Comparator Sleep Mode Clock Gating Control                     */
  __IO uint32_t  SCGCPWM;                           /*!< Pulse Width Modulator Sleep Mode Clock Gating Control                 */
  __IO uint32_t  SCGCQEI;                           /*!< Quadrature Encoder Interface Sleep Mode Clock Gating Control          */
  __I  uint32_t  RESERVED34[4];
  __IO uint32_t  SCGCEEPROM;                        /*!< EEPROM Sleep Mode Clock Gating Control                                */
  __IO uint32_t  SCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Sleep Mode Clock Gating
                                                         Control                                                               */
  __I  uint32_t  RESERVED35[40];
  __IO uint32_t  DCGCWD;                            /*!< Watchdog Timer Deep-Sleep Mode Clock Gating Control                   */
  __IO uint32_t  DCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Deep-Sleep Mode Clock Gating
                                                         Control                                                               */
  __IO uint32_t  DCGCGPIO;                          /*!< General-Purpose Input/Output Deep-Sleep Mode Clock Gating Control     */
  __IO uint32_t  DCGCDMA;                           /*!< Micro Direct Memory Access Deep-Sleep Mode Clock Gating Control       */
  __I  uint32_t  RESERVED36;
  __IO uint32_t  DCGCHIB;                           /*!< Hibernation Deep-Sleep Mode Clock Gating Control                      */
  __IO uint32_t  DCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Deep-Sleep Mode
                                                         Clock Gating Control                                                  */
  __IO uint32_t  DCGCSSI;                           /*!< Synchronous Serial Interface Deep-Sleep Mode Clock Gating Control     */
  __IO uint32_t  DCGCI2C;                           /*!< Inter-Integrated Circuit Deep-Sleep Mode Clock Gating Control         */
  __I  uint32_t  RESERVED37;
  __IO uint32_t  DCGCUSB;                           /*!< Universal Serial Bus Deep-Sleep Mode Clock Gating Control             */
  __I  uint32_t  RESERVED38[2];
  __IO uint32_t  DCGCCAN;                           /*!< Controller Area Network Deep-Sleep Mode Clock Gating Control          */
  __IO uint32_t  DCGCADC;                           /*!< Analog-to-Digital Converter Deep-Sleep Mode Clock Gating Control      */
  __IO uint32_t  DCGCACMP;                          /*!< Analog Comparator Deep-Sleep Mode Clock Gating Control                */
  __IO uint32_t  DCGCPWM;                           /*!< Pulse Width Modulator Deep-Sleep Mode Clock Gating Control            */
  __IO uint32_t  DCGCQEI;                           /*!< Quadrature Encoder Interface Deep-Sleep Mode Clock Gating Control     */
  __I  uint32_t  RESERVED39[4];
  __IO uint32_t  DCGCEEPROM;                        /*!< EEPROM Deep-Sleep Mode Clock Gating Control                           */
  __IO uint32_t  DCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Deep-Sleep Mode Clock Gating
                                                         Control                                                               */
  __I  uint32_t  RESERVED40[104];
  __IO uint32_t  PRWD;                              /*!< Watchdog Timer Peripheral Ready                                       */
  __IO uint32_t  PRTIMER;                           /*!< 16/32-Bit General-Purpose Timer Peripheral Ready                      */
  __IO uint32_t  PRGPIO;                            /*!< General-Purpose Input/Output Peripheral Ready                         */
  __IO uint32_t  PRDMA;                             /*!< Micro Direct Memory Access Peripheral Ready                           */
  __I  uint32_t  RESERVED41;
  __IO uint32_t  PRHIB;                             /*!< Hibernation Peripheral Ready                                          */
  __IO uint32_t  PRUART;                            /*!< Universal Asynchronous Receiver/Transmitter Peripheral Ready          */
  __IO uint32_t  PRSSI;                             /*!< Synchronous Serial Interface Peripheral Ready                         */
  __IO uint32_t  PRI2C;                             /*!< Inter-Integrated Circuit Peripheral Ready                             */
  __I  uint32_t  RESERVED42;
  __IO uint32_t  PRUSB;                             /*!< Universal Serial Bus Peripheral Ready                                 */
  __I  uint32_t  RESERVED43[2];
  __IO uint32_t  PRCAN;                             /*!< Controller Area Network Peripheral Ready                              */
  __IO uint32_t  PRADC;                             /*!< Analog-to-Digital Converter Peripheral Ready                          */
  __IO uint32_t  PRACMP;                            /*!< Analog Comparator Peripheral Ready                                    */
  __IO uint32_t  PRPWM;                             /*!< Pulse Width Modulator Peripheral Ready                                */
  __IO uint32_t  PRQEI;                             /*!< Quadrature Encoder Interface Peripheral Ready                         */
  __I  uint32_t  RESERVED44[4];
  __IO uint32_t  PREEPROM;                          /*!< EEPROM Peripheral Ready                                               */
  __IO uint32_t  PRWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Peripheral Ready                 */
} SYSCTL_Type;


/* ================================================================================ */
/* ================                      UDMA                      ================ */
/* ================================================================================ */


/**
  * @brief Register map for UDMA peripheral (UDMA)
  */

typedef struct {                                    /*!< UDMA Structure                                                        */
  __IO uint32_t  STAT;                              /*!< DMA Status                                                            */
  __O  uint32_t  CFG;                               /*!< DMA Configuration                                                     */
  __IO uint32_t  CTLBASE;                           /*!< DMA Channel Control Base Pointer                                      */
  __IO uint32_t  ALTBASE;                           /*!< DMA Alternate Channel Control Base Pointer                            */
  __IO uint32_t  WAITSTAT;                          /*!< DMA Channel Wait-on-Request Status                                    */
  __O  uint32_t  SWREQ;                             /*!< DMA Channel Software Request                                          */
  __IO uint32_t  USEBURSTSET;                       /*!< DMA Channel Useburst Set                                              */
  __O  uint32_t  USEBURSTCLR;                       /*!< DMA Channel Useburst Clear                                            */
  __IO uint32_t  REQMASKSET;                        /*!< DMA Channel Request Mask Set                                          */
  __O  uint32_t  REQMASKCLR;                        /*!< DMA Channel Request Mask Clear                                        */
  __IO uint32_t  ENASET;                            /*!< DMA Channel Enable Set                                                */
  __O  uint32_t  ENACLR;                            /*!< DMA Channel Enable Clear                                              */
  __IO uint32_t  ALTSET;                            /*!< DMA Channel Primary Alternate Set                                     */
  __O  uint32_t  ALTCLR;                            /*!< DMA Channel Primary Alternate Clear                                   */
  __IO uint32_t  PRIOSET;                           /*!< DMA Channel Priority Set                                              */
  __O  uint32_t  PRIOCLR;                           /*!< DMA Channel Priority Clear                                            */
  __I  uint32_t  RESERVED0[3];
  __IO uint32_t  ERRCLR;                            /*!< DMA Bus Error Clear                                                   */
  __I  uint32_t  RESERVED1[300];
  __IO uint32_t  CHASGN;                            /*!< DMA Channel Assignment                                                */
  __IO uint32_t  CHIS;                              /*!< DMA Channel Interrupt Status                                          */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  CHMAP0;                            /*!< DMA Channel Map Select 0                                              */
  __IO uint32_t  CHMAP1;                            /*!< DMA Channel Map Select 1                                              */
  __IO uint32_t  CHMAP2;                            /*!< DMA Channel Map Select 2                                              */
  __IO uint32_t  CHMAP3;                            /*!< DMA Channel Map Select 3                                              */
} UDMA_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define WATCHDOG0_BASE                  0x40000000UL
#define WATCHDOG1_BASE                  0x40001000UL
#define GPIOA_BASE                      0x40004000UL
#define GPIOB_BASE                      0x40005000UL
#define GPIOC_BASE                      0x40006000UL
#define GPIOD_BASE                      0x40007000UL
#define SSI0_BASE                       0x40008000UL
#define SSI1_BASE                       0x40009000UL
#define SSI2_BASE                       0x4000A000UL
#define SSI3_BASE                       0x4000B000UL
#define UART0_BASE                      0x4000C000UL
#define UART1_BASE                      0x4000D000UL
#define UART2_BASE                      0x4000E000UL
#define UART3_BASE                      0x4000F000UL
#define UART4_BASE                      0x40010000UL
#define UART5_BASE                      0x40011000UL
#define UART6_BASE                      0x40012000UL
#define UART7_BASE                      0x40013000UL
#define I2C0_BASE                       0x40020000UL
#define I2C1_BASE                       0x40021000UL
#define I2C2_BASE                       0x40022000UL
#define I2C3_BASE                       0x40023000UL
#define GPIOE_BASE                      0x40024000UL
#define GPIOF_BASE                      0x40025000UL
#define PWM0_BASE                       0x40028000UL
#define PWM1_BASE                       0x40029000UL
#define QEI0_BASE                       0x4002C000UL
#define QEI1_BASE                       0x4002D000UL
#define TIMER0_BASE                     0x40030000UL
#define TIMER1_BASE                     0x40031000UL
#define TIMER2_BASE                     0x40032000UL
#define TIMER3_BASE                     0x40033000UL
#define TIMER4_BASE                     0x40034000UL
#define TIMER5_BASE                     0x40035000UL
#define WTIMER0_BASE                    0x40036000UL
#define WTIMER1_BASE                    0x40037000UL
#define ADC0_BASE                       0x40038000UL
#define ADC1_BASE                       0x40039000UL
#define COMP_BASE                       0x4003C000UL
#define CAN0_BASE                       0x40040000UL
#define CAN1_BASE                       0x40041000UL
#define WTIMER2_BASE                    0x4004C000UL
#define WTIMER3_BASE                    0x4004D000UL
#define WTIMER4_BASE                    0x4004E000UL
#define WTIMER5_BASE                    0x4004F000UL
#define USB0_BASE                       0x40050000UL
#define GPIOA_AHB_BASE                  0x40058000UL
#define GPIOB_AHB_BASE                  0x40059000UL
#define GPIOC_AHB_BASE                  0x4005A000UL
#define GPIOD_AHB_BASE                  0x4005B000UL
#define GPIOE_AHB_BASE                  0x4005C000UL
#define GPIOF_AHB_BASE                  0x4005D000UL
#define EEPROM_BASE                     0x400AF000UL
#define SYSEXC_BASE                     0x400F9000UL
#define HIB_BASE                        0x400FC000UL
#define FLASH_CTRL_BASE                 0x400FD000UL
#define SYSCTL_BASE                     0x400FE000UL
#define UDMA_BASE                       0x400FF000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define WATCHDOG0                       ((WATCHDOG0_Type          *) WATCHDOG0_BASE)
#define WATCHDOG1                       ((WATCHDOG0_Type          *) WATCHDOG1_BASE)
#define GPIOA                           ((GPIOA_Type              *) GPIOA_BASE)
#define GPIOB                           ((GPIOA_Type              *) GPIOB_BASE)
#define GPIOC                           ((GPIOA_Type              *) GPIOC_BASE)
#define GPIOD                           ((GPIOA_Type              *) GPIOD_BASE)
#define SSI0                            ((SSI0_Type               *) SSI0_BASE)
#define SSI1                            ((SSI0_Type               *) SSI1_BASE)
#define SSI2                            ((SSI0_Type               *) SSI2_BASE)
#define SSI3                            ((SSI0_Type               *) SSI3_BASE)
#define UART0                           ((UART0_Type              *) UART0_BASE)
#define UART1                           ((UART0_Type              *) UART1_BASE)
#define UART2                           ((UART0_Type              *) UART2_BASE)
#define UART3                           ((UART0_Type              *) UART3_BASE)
#define UART4                           ((UART0_Type              *) UART4_BASE)
#define UART5                           ((UART0_Type              *) UART5_BASE)
#define UART6                           ((UART0_Type              *) UART6_BASE)
#define UART7                           ((UART0_Type              *) UART7_BASE)
#define I2C0                            ((I2C0_Type               *) I2C0_BASE)
#define I2C1                            ((I2C0_Type               *) I2C1_BASE)
#define I2C2                            ((I2C0_Type               *) I2C2_BASE)
#define I2C3                            ((I2C0_Type               *) I2C3_BASE)
#define GPIOE                           ((GPIOA_Type              *) GPIOE_BASE)
#define GPIOF                           ((GPIOA_Type              *) GPIOF_BASE)
#define PWM0                            ((PWM0_Type               *) PWM0_BASE)
#define PWM1                            ((PWM0_Type               *) PWM1_BASE)
#define QEI0                            ((QEI0_Type               *) QEI0_BASE)
#define QEI1                            ((QEI0_Type               *) QEI1_BASE)
#define TIMER0                          ((TIMER0_Type             *) TIMER0_BASE)
#define TIMER1                          ((TIMER0_Type             *) TIMER1_BASE)
#define TIMER2                          ((TIMER0_Type             *) TIMER2_BASE)
#define TIMER3                          ((TIMER0_Type             *) TIMER3_BASE)
#define TIMER4                          ((TIMER0_Type             *) TIMER4_BASE)
#define TIMER5                          ((TIMER0_Type             *) TIMER5_BASE)
#define WTIMER0                         ((WTIMER0_Type            *) WTIMER0_BASE)
#define WTIMER1                         ((TIMER0_Type             *) WTIMER1_BASE)
#define ADC0                            ((ADC0_Type               *) ADC0_BASE)
#define ADC1                            ((ADC0_Type               *) ADC1_BASE)
#define COMP                            ((COMP_Type               *) COMP_BASE)
#define CAN0                            ((CAN0_Type               *) CAN0_BASE)
#define CAN1                            ((CAN0_Type               *) CAN1_BASE)
#define WTIMER2                         ((TIMER0_Type             *) WTIMER2_BASE)
#define WTIMER3                         ((TIMER0_Type             *) WTIMER3_BASE)
#define WTIMER4                         ((TIMER0_Type             *) WTIMER4_BASE)
#define WTIMER5                         ((TIMER0_Type             *) WTIMER5_BASE)
#define USB0                            ((USB0_Type               *) USB0_BASE)
#define GPIOA_AHB                       ((GPIOA_Type              *) GPIOA_AHB_BASE)
#define GPIOB_AHB                       ((GPIOA_Type              *) GPIOB_AHB_BASE)
#define GPIOC_AHB                       ((GPIOA_Type              *) GPIOC_AHB_BASE)
#define GPIOD_AHB                       ((GPIOA_Type              *) GPIOD_AHB_BASE)
#define GPIOE_AHB                       ((GPIOA_Type              *) GPIOE_AHB_BASE)
#define GPIOF_AHB                       ((GPIOA_Type              *) GPIOF_AHB_BASE)
#define EEPROM                          ((EEPROM_Type             *) EEPROM_BASE)
#define SYSEXC                          ((SYSEXC_Type             *) SYSEXC_BASE)
#define HIB                             ((HIB_Type                *) HIB_BASE)
#define FLASH_CTRL                      ((FLASH_CTRL_Type         *) FLASH_CTRL_BASE)
#define SYSCTL                          ((SYSCTL_Type             *) SYSCTL_BASE)
#define UDMA                            ((UDMA_Type               *) UDMA_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group TM4C123GH6PM */
/** @} */ /* End of group Texas Instruments */

#ifdef __cplusplus
}
#endif


#endif  /* TM4C123GH6PM_H */

