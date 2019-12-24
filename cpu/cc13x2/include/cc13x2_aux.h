/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC13x2 AUX register definitions
 */

#ifndef CC13X2_AUX_H
#define CC13X2_AUX_H

#include <stdbool.h>

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * AUX_AIODIO registers
 */
typedef struct {
    reg32_t IOMODE; /**< input output mode */
    reg32_t GPIODIE; /**< gpio data input enable */
    reg32_t IOPOE; /**< io peripheral output enable */
    reg32_t GPIODOUT; /**< gpio data output */
    reg32_t GPIODIN; /**< gpio data input */
    reg32_t GPIODOUTSET; /**< gpio data out set */
    reg32_t GPIODOUTCLR; /**< gpio data out clear */
    reg32_t GPIODOUTTGL; /**< gpio data out toggle */
    reg32_t IO0PSEL; /**< i/o 0 peripheral select */
    reg32_t IO1PSEL; /**< i/o 1 peripheral select */
    reg32_t IO2PSEL; /**< i/o 2 peripheral select */
    reg32_t IO3PSEL; /**< i/o 3 peripheral select */
    reg32_t IO4PSEL; /**< i/o 4 peripheral select */
    reg32_t IO5PSEL; /**< i/o 5 peripheral select */
    reg32_t IO6PSEL; /**< i/o 6 peripheral select */
    reg32_t IO7PSEL; /**< i/o 7 peripheral select */
    reg32_t IOMODEH; /**< input output mode high */
    reg32_t IOMODEL; /**< input output mode low */
} aux_aiodio_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_AIODIO0_BASE      0x400CC000 /**< AUX_AIODIO0 base address */
#define AUX_AIODIO1_BASE      0x400CD000 /**< AUX_AIODIO1 base address */
#define AUX_AIODIO2_BASE      0x400CE000 /**< AUX_AIODIO2 base address */
#define AUX_AIODIO3_BASE      0x400CF000 /**< AUX_AIODIO3 base address */
/** @} */

#define AUX_AIODIO0          ((aux_aiodio_regs_t *) (AUX_AIODIO0_BASE)) /**< AUX_AIODIO0 register bank */
#define AUX_AIODIO1          ((aux_aiodio_regs_t *) (AUX_AIODIO1_BASE)) /**< AUX_AIODIO1 register bank */
#define AUX_AIODIO2          ((aux_aiodio_regs_t *) (AUX_AIODIO2_BASE)) /**< AUX_AIODIO2 register bank */
#define AUX_AIODIO3          ((aux_aiodio_regs_t *) (AUX_AIODIO3_BASE)) /**< AUX_AIODIO3 register bank */


/**
 * AUX_TDC registers
 */
typedef struct {
    reg32_t CTL; /**< control */
    reg32_t STAT; /**< status */
    reg32_t RESULT; /**< result */
    reg32_t SATCFG; /**< saturaion configuration */
    reg32_t TRIGSRC; /**< trigger source */
    reg32_t TRIGCNT; /**< trigger counter */
    reg32_t TRIGCNTLOAD; /**< trigger counter load */
    reg32_t TRIGCNTCFG; /**< trigger counter config */
    reg32_t PRECTL; /**< prescaler control */
    reg32_t PRECNT; /**< prescaler counter */
} aux_tdc_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_TDC_BASE      0x400C4000 /**< AUX_TDC base address */
/** @} */

#define AUX_TDC           ((aux_tdc_regs_t *) (AUX_TDC_BASE)) /**< AUX_TDC register bank */


/**
 * AUX_EVCTL registers
 */
typedef struct {
    reg32_t EVSTAT0; /**< event status 0 */
    reg32_t EVSTAT1; /**< event status 1 */
    reg32_t EVSTAT2; /**< event status 2 */
    reg32_t EVSTAT3; /**< event status 3 */
    reg32_t SCEWEVSEL0; /**< sensor controller engine wait event configuration 0 */
    reg32_t SCEWEVSEL1; /**< sensor controller engine wait event configuration 1 */
    reg32_t DMACTL; /**< direct memoty access control */
    reg32_t __reserved; /**< meh */
    reg32_t SWEVSET; /**< software event set */
    reg32_t EVTOAONFLAGS; /**< events to AON domain flags */
    reg32_t EVTOAONPOL; /**< events to AON domain polarity */
    reg32_t EVTOAONFLAGSCLR; /**< events to AON domain clear */
    reg32_t EVTOMCUFLAGS; /**< event to MCU domain flags */
    reg32_t EVTOMCUPOL; /**< event to MCU domain polarity */
    reg32_t EVTOMCUFLAGSCLR; /**< events to MCU domain flags clear */
    reg32_t COMBEVTOMCUMASK; /**< combined event to MCU domain mask */
    reg32_t PROGDLY; /**< programmable delay */
    reg32_t MANUAL; /**< manual */
    reg32_t EVSTAT0L; /**< event status 0 low */
    reg32_t EVSTAT0H; /**< event status 0 high */
    reg32_t EVSTAT1L; /**< event status 1 low */
    reg32_t EVSTAT1H; /**< event status 1 high */
    reg32_t EVSTAT2L; /**< event status 2 low */
    reg32_t EVSTAT2H; /**< event status 2 high */
    reg32_t EVSTAT3L; /**< event status 3 low */
    reg32_t EVSTAT3H; /**< event status 3 high */

} aux_evtcl_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_EVCTL_BASE      0x400C5000 /**< AUX_EVCTL base address */
/** @} */

#define AUX_EVCTL           ((aux_evctl_regs_t *) (AUX_EVCTL_BASE)) /**< AUX_EVCTL register bank */


/**
 * AUX_SYSIF registers
 */
typedef struct {
    reg32_t OPMODEREQ; /**< operational mode request */
    reg32_t OPMODEACK; /**< operational mode acknowledgement */
    reg32_t PROGWU0CFG; /**< programmanble wakeup 0 configuration */
    reg32_t PROGWU1CFG; /**< programmanble wakeup 0 configuration */
    reg32_t PROGWU2CFG; /**< programmanble wakeup 0 configuration */
    reg32_t PROGWU3CFG; /**< programmanble wakeup 0 configuration */
    reg32_t SWUTRIG; /**< software wakeup triggers */
    reg32_t WUFLAGS; /**< wakeup flags */
    reg32_t WUFLAGSCLR; /**< wakeup flags clear */
    reg32_t WUGATE; /**< wakeup gate */
    reg32_t VECCFG0; /**< vector configuration 0 */
    reg32_t VECCFG1; /**< vector configuration 1 */
    reg32_t VECCFG2; /**< vector configuration 2 */
    reg32_t VECCFG3; /**< vector configuration 3 */
    reg32_t VECCFG4; /**< vector configuration 4 */
    reg32_t VECCFG5; /**< vector configuration 5 */
    reg32_t VECCFG6; /**< vector configuration 6 */
    reg32_t VECCFG7; /**< vector configuration 7 */
    reg32_t EVSYNCRATE; /**< event synchronization rate */
    reg32_t PEROPRATE; /**< peripheral operational rate */
    reg32_t ADCCLKCTL; /**< ADC clock control */
    reg32_t TDCCLKCTL; /**< TDC clock control */
    reg32_t TDCREFCLKCTL; /**< TDC reference clock control */
    reg32_t TIMER2CLKCTL; /**< AUX_TIMER2 clock control */
    reg32_t TIMER2CLKSTAT; /**< AUX_TIMER2 clock status */
    reg32_t TIMER2CLKSWITCH; /**< AUX_TIMER2 clock switch */
    reg32_t TIMER2DBGCTL; /**< AUX_TIMER2 debug control */
    reg32_t __reserved1; /**< meh */
    reg32_t CLKSHIFTDET; /**< clock shift detection */
    reg32_t RECHARGETRIG; /**< recharge trigger */
    reg32_t RECHARGEDET; /**< recharge detector */
    reg32_t RTCSUBSECINC0; /**< RTC sub-second increment 0 */
    reg32_t RTCSUBSECINC1; /**< RTC sub-second increment 1 */
    reg32_t RTCSUBSECINCCTL; /**< RTC sub-second increment control */
    reg32_t RTCSEC; /**< RTC second */
    reg32_t RTCSUBSEC; /**< RTC sub-second */
    reg32_t RTCEVCLR; /**< RTC event clear */
    reg32_t BATMONBAT; /**< battery voltage */
    reg32_t __reserved2; /**< meh */
    reg32_t BATMONTEMP; /**< batmon temperature */
    reg32_t TIMERHALT; /**< timer halt */
    reg32_t __reserved3[3]; /**< meh */
    reg32_t TIMER2BRIDGE; /**< timer2 bridge */
    reg32_t SWPWRPROF; /**< software power profiler */
} aux_sysif_regs_t;

/** @} */

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_SYSIF_BASE      0x400C6000 /**< AUX_SYSIF base address */
/** @} */

#define AUX_SYSIF           ((aux_wuc_regs_t *) (AUX_SYSIF_BASE)) /**< AUX_SYSIF register bank */


/**
 * AUX_TIMER registers
 */
typedef struct {
    reg32_t T0CFG; /**< timer 0 config */
    reg32_t T0CTL; /**< timer 0 control */
    reg32_t T0TARGET; /**< timer 0 target */
    reg32_t T0CNTR; /**< timer 0 counter */
    reg32_t T1CFG; /**< timer 1 config */
    reg32_t T1TARGET; /**< timer 1 target */
    reg32_t T1CTL; /**< timer 1 control */
    reg32_t T1CNTR; /**< timer 0 counter */
} aux_timer01_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_TIMER01_BASE      0x400C7000 /**< AUX_WUC base address */
/** @} */

#define AUX_TIMER01           ((aux_timer01_regs_t *) (AUX_TIMER01_BASE)) /**< AUX_TIMER01 register bank */


/**
 * AUX_TIMER2 registers
 */
typedef struct {
    reg32_t CTL; /**< timer 2 control */
    reg32_t TARGET; /**< timer 2 target */
    reg32_t SHDWTARGET; /**< timer 2 shadow target */
    reg32_t CNTR; /**< timer 2 counter */
    reg32_t PRECFG; /**< timer 2 prescaler config */
    reg32_t EVCTL; /**< timer 2 event control */
    reg32_t PULSETRIG; /**< timer 2 pulse trigger */
    reg32_t __reserved[25]; /**< meh */
    reg32_t CH0EVCFG; /**< timer 2 channel 0 event configuration */
    reg32_t CH0CCFG; /**< timer 2 channel 0 capture configuration */
    reg32_t CH0PCC; /**< timer 2 channel 0 pipeline capture compare */
    reg32_t CH0CC; /**< timer 2 channel 0 capture compare */
    reg32_t CH1EVCFG; /**< timer 2 channel 1 event configuration */
    reg32_t CH1CCFG; /**< timer 2 channel 1 capture configuration */
    reg32_t CH1PCC; /**< timer 2 channel 1 pipeline capture compare */
    reg32_t CH1CC; /**< timer 2 channel 1 capture compare */
    reg32_t CH2EVCFG; /**< timer 2 channel 2 event configuration */
    reg32_t CH2CCFG; /**< timer 2 channel 2 capture configuration */
    reg32_t CH2PCC; /**< timer 2 channel 2 pipeline capture compare */
    reg32_t CH2CC; /**< timer 2 channel 2 capture compare */
    reg32_t CH3EVCFG; /**< timer 2 channel 3 event configuration */
    reg32_t CH3CCFG; /**< timer 2 channel 3 capture configuration */
    reg32_t CH3PCC; /**< timer 2 channel 3 pipeline capture compare */
    reg32_t CH3CC; /**< timer 2 channel 3 capture compare */
} aux_timer2_regs_t;


/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_TIMER2_BASE      0x400C3000 /**< AUX_WUC base address */
/** @} */
#define AUX_TIMER2           ((aux_timer2_regs_t *) (AUX_TIMER2_BASE)) /**< AUX_TIMER2 register bank */



/**
 * AUX_SMPH registers
 */
typedef struct {
    reg32_t SMPH0; /**< semaphore 0 */
    reg32_t SMPH1; /**< semaphore 1 */
    reg32_t SMPH2; /**< semaphore 2 */
    reg32_t SMPH3; /**< semaphore 3 */
    reg32_t SMPH4; /**< semaphore 4 */
    reg32_t SMPH5; /**< semaphore 5 */
    reg32_t SMPH6; /**< semaphore 6 */
    reg32_t SMPH7; /**< semaphore 7 */
    reg32_t AUTOTAKE; /**< sticky request for single semaphore */
} aux_smph_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_SMPH_BASE               0x400C8000 /**< AUX_WUC base address */
/* @} */

#define AUX_SMPH ((aux_smph_regs_t *) (AUX_SMPH_BASE)) /**< AUX_SMPH register bank */


/**
 * AUX_ANAIF registers
 */
typedef struct {
    reg32_t __reserved1[4]; /**< meh */
    reg32_t ADCCTL; /**< ADC control */
    reg32_t ADCFIFOSTAT; /**< ADC fifo status */
    reg32_t ADCFIFO; /**< ADC fifo */
    reg32_t ADCTRIG; /**< ADC trigger */
    reg32_t ISRCCTL; /**< current source control */
    reg32_t __reserved2[3]; /**< meh */
    reg32_t DACCTL; /**< DAC control */
    reg32_t LPMBIASCTL; /**< low-power mode bias control */
    reg32_t DACSMPLCTL; /**< DAC sample control */
    reg32_t DACSMPLCFG0; /**< DAC sample configuration 0 */
    reg32_t DACSMPLCFG1; /**< DAC sample configuration 1 */
    reg32_t DACVALUE; /**< DAC value */
    reg32_t DACSTAT; /**< DAC status */
} aux_anaif_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define AUX_ANAIF_BASE      0x400C9000 /**< AUX_WUC base address */
/** @} */

#define AUX_ANAIF           ((aux_anaif_regs_t *) (AUX_ANAIF_BASE)) /**< AUX_ANAIF register bank */


/**
 * ADI_4_AUX registers
 */
typedef struct {
    reg8_t MUX0; /**< multiplexer 0 */
    reg8_t MUX1; /**< multiplexer 1 */
    reg8_t MUX2; /**< multiplexer 2 */
    reg8_t MUX3; /**< multiplexer 3 */
    reg8_t ISRC; /**< current source */
    reg8_t COMP; /**< comparator */
    reg8_t MUX4; /**< multiplexer 4 */
    reg8_t ADC0; /**< ADC control 0 */
    reg8_t ADC1; /**< ADC control 1 */
    reg8_t ADCREF0; /**< ADC reference 0 */
    reg8_t ADCREF1; /**< ADC reference 1 */
} adi_4_aux_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define ADI_4_AUX_BASE      0x400CB000 /**< AUX_WUC base address */
/** @} */

#define ADI_4_AUX           ((adi_4_aux_regs_t *) (ADI_4_AUX_BASE)) /**< ADI_4_AUX register bank */


#define ADDI_SEM AUX_SMPH->SMPH0 /**< the semamphore used for ADDI */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_AUX_H */

/** @}*/
