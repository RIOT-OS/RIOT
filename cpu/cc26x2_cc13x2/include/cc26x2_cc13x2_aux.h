/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 AUX register definitions
 */


#include <stdbool.h>

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AUX_AIODIO registers
 */
typedef struct {
    reg32_t IOMODE; /**< Input output mode */
    reg32_t GPIODIE; /**< GPIO data input enable */
    reg32_t IOPOE; /**< I/O peripheral output enable */
    reg32_t GPIODOUT; /**< GPIO data output */
    reg32_t GPIODIN; /**< GPIO data input */
    reg32_t GPIODOUTSET; /**< GPIO data out set */
    reg32_t GPIODOUTCLR; /**< GPIO data out clear */
    reg32_t GPIODOUTTGL; /**< GPIO data out toggle */
    reg32_t IO0PSEL; /**< I/O 0 peripheral select */
    reg32_t IO1PSEL; /**< I/O 1 peripheral select */
    reg32_t IO2PSEL; /**< I/O 2 peripheral select */
    reg32_t IO3PSEL; /**< I/O 3 peripheral select */
    reg32_t IO4PSEL; /**< I/O 4 peripheral select */
    reg32_t IO5PSEL; /**< I/O 5 peripheral select */
    reg32_t IO6PSEL; /**< I/O 6 peripheral select */
    reg32_t IO7PSEL; /**< I/O 7 peripheral select */
    reg32_t IOMODEH; /**< Input output mode high */
    reg32_t IOMODEL; /**< Input output mode low */
} aux_aiodio_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_AIODIO0 base address
 */
#define AUX_AIODIO0_BASE     (PERIPH_BASE + 0xCC000)
/**
 * @brief   AUX_AIODIO1 base address
 */
#define AUX_AIODIO1_BASE     (PERIPH_BASE + 0xCD000)
/**
 * @brief   AUX_AIODIO2 base address
 */
#define AUX_AIODIO2_BASE     (PERIPH_BASE + 0xCE000)
/**
 * @brief   AUX_AIODIO3 base address
 */
#define AUX_AIODIO3_BASE     (PERIPH_BASE + 0xCF000)
/** @} */

/**
 * @brief   AUX_AIODIO0 register bank
 */
#define AUX_AIODIO0          ((aux_aiodio_regs_t *) (AUX_AIODIO0_BASE))
/**
 * @brief   AUX_AIODIO1 register bank
 */
#define AUX_AIODIO1          ((aux_aiodio_regs_t *) (AUX_AIODIO1_BASE))
/**
 * @brief   AUX_AIODIO2 register bank
 */
#define AUX_AIODIO2          ((aux_aiodio_regs_t *) (AUX_AIODIO2_BASE))
/**
 * @brief   AUX_AIODIO3 register bank
 */
#define AUX_AIODIO3          ((aux_aiodio_regs_t *) (AUX_AIODIO3_BASE))

/**
 * @brief   AUX_TDC registers
 */
typedef struct {
    reg32_t CTL; /**< Control */
    reg32_t STAT; /**< Status */
    reg32_t RESULT; /**< Result */
    reg32_t SATCFG; /**< Saturaion configuration */
    reg32_t TRIGSRC; /**< Trigger source */
    reg32_t TRIGCNT; /**< Trigger counter */
    reg32_t TRIGCNTLOAD; /**< Trigger counter load */
    reg32_t TRIGCNTCFG; /**< Trigger counter config */
    reg32_t PRECTL; /**< Prescaler control */
    reg32_t PRECNTR; /**< Prescaler counter */
} aux_tdc_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TDC base address
 */
#define AUX_TDC_BASE         (PERIPH_BASE + 0xC4000)
/** @} */

/**
 * @brief   AUX_TDC register bank
 */
#define AUX_TDC              ((aux_tdc_regs_t *) (AUX_TDC_BASE))

/**
 * @brief   AUX_EVCTL registers
 */
typedef struct {
    reg32_t EVSTAT0; /**< Event Status 0 */
    reg32_t EVSTAT1; /**< Event Status 1 */
    reg32_t EVSTAT2; /**< Event Status 2 */
    reg32_t EVSTAT3; /**< Event Status 3 */
    reg32_t SCEWEVCFG0; /**< Sensor Controller Engine Wait Event Configuration 0 */
    reg32_t SCEWEVCFG1; /**< Sensor Controller Engine Wait Event Configuration 1 */
    reg32_t DMACTL; /**< Direct Memory Access Control */
    reg32_t __reserved1; /**< Reserved */
    reg32_t SWEVSET; /**< Software Event Set */
    reg32_t EVTOAONFLAGS; /**< Events To AON Flags */
    reg32_t EVTOAONPOL; /**< Events To AON Polarity */
    reg32_t EVTOAONFLAGSCLR; /**< Events To AON Clear */
    reg32_t EVTOMCUFLAGS; /**< Events to MCU Flags */
    reg32_t EVTOMCUPOL; /**< Event To MCU Polarity */
    reg32_t EVTOMCUFLAGSCLR; /**< Events To MCU Flags Clear */
    reg32_t COMBEVTOMCUMASK; /**< Combined Event To MCU Mask */
    reg32_t EVOBSCFG; /**< Event Observation Configuration */
    reg32_t PROGDLY; /**< Programmable Delay */
    reg32_t MANUAL; /**< Manual */
    reg32_t EVSTAT0L; /**< Event Status 0 Low */
    reg32_t EVSTAT0H; /**< Event Status 0 High */
    reg32_t EVSTAT1L; /**< Event Status 1 Low */
    reg32_t EVSTAT1H; /**< Event Status 1 High */
    reg32_t EVSTAT2L; /**< Event Status 2 Low */
    reg32_t EVSTAT2H; /**< Event Status 2 High */
    reg32_t EVSTAT3L; /**< Event Status 3 Low */
    reg32_t EVSTAT3H; /**< Event Status 3 High */
} aux_evctl_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_EVCTL base address
 */
#define AUX_EVCTL_BASE       (PERIPH_BASE + 0xC5000)
/** @} */

/**
 * @brief   AUX_EVCTL register bank
 */
#define AUX_EVCTL            ((aux_evctl_regs_t *) (AUX_EVCTL_BASE))

/**
 * @brief   AUX_SYSIF registers
 */
typedef struct {
    reg32_t OPMODEREQ; /**< Operational Mode Request */
    reg32_t OPMODEACK; /**< Operational Mode Acknowledgement */
    reg32_t PROGWU0CFG; /**< Programmable Wakeup 0 Configuration */
    reg32_t PROGWU1CFG; /**< Programmable Wakeup 1 Configuration */
    reg32_t PROGWU2CFG; /**< Programmable Wakeup 2 Configuration */
    reg32_t PROGWU3CFG; /**< Programmable Wakeup 3 Configuration */
    reg32_t SWWUTRIG; /**< Software Wakeup Triggers */
    reg32_t WUFLAGS; /**< Wakeup Flags */
    reg32_t WUFLAGSCLR; /**< Wakeup Flags Clear */
    reg32_t WUGATE; /**< Wakeup Gate */
    reg32_t VECCFG0; /**< Vector Configuration 0 */
    reg32_t VECCFG1; /**< Vector Configuration 1 */
    reg32_t VECCFG2; /**< Vector Configuration 2 */
    reg32_t VECCFG3; /**< Vector Configuration 3 */
    reg32_t VECCFG4; /**< Vector Configuration 4 */
    reg32_t VECCFG5; /**< Vector Configuration 5 */
    reg32_t VECCFG6; /**< Vector Configuration 6 */
    reg32_t VECCFG7; /**< Vector Configuration 7 */
    reg32_t EVSYNCRATE; /**< Event Synchronization Rate */
    reg32_t PEROPRATE; /**< Peripheral Operational Rate */
    reg32_t ADCCLKCTL; /**< ADC Clock Control */
    reg32_t TDCCLKCTL; /**< TDC Counter Clock Control */
    reg32_t TDCREFCLKCTL; /**< TDC Reference Clock Control */
    reg32_t TIMER2CLKCTL; /**< AUX_TIMER2 Clock Control */
    reg32_t TIMER2CLKSTAT; /**< AUX_TIMER2 Clock Status */
    reg32_t TIMER2CLKSWITCH; /**< AUX_TIMER2 Clock Switch */
    reg32_t TIMER2DBGCTL; /**< AUX_TIMER2 Debug Control */
    reg32_t __reserved1; /**< Reserved */
    reg32_t CLKSHIFTDET; /**< Clock Shift Detection */
    reg32_t RECHARGETRIG; /**< VDDR Recharge Trigger */
    reg32_t RECHARGEDET; /**< VDDR Recharge Detection */
    reg32_t RTCSUBSECINC0; /**< Real Time Counter Sub Second Increment 0 */
    reg32_t RTCSUBSECINC1; /**< Real Time Counter Sub Second Increment 1 */
    reg32_t RTCSUBSECINCCTL; /**< Real Time Counter Sub Second Increment Control */
    reg32_t RTCSEC; /**< Real Time Counter Second */
    reg32_t RTCSUBSEC; /**< Real Time Counter Sub-Second */
    reg32_t RTCEVCLR; /**< AON_RTC Event Clear */
    reg32_t BATMONBAT; /**< AON_BATMON Battery Voltage Value */
    reg32_t __reserved2; /**< Reserved */
    reg32_t BATMONTEMP; /**< AON_BATMON Temperature Value */
    reg32_t TIMERHALT; /**< Timer Halt */
    reg32_t __reserved3[0x3]; /**< Reserved */
    reg32_t TIMER2BRIDGE; /**< AUX_TIMER2 Bridge */
    reg32_t SWPWRPROF; /**< Software Power Profiler */
} aux_sysif_regs_t;

/**
 * @brief   AUX_SYSIF register values
 * @{
 */
#define AUX_SYSIF_OPMODEREQ_REQ_PDLP 0x00000003
#define AUX_SYSIF_OPMODEREQ_REQ_PDA  0x00000002
#define AUX_SYSIF_OPMODEREQ_REQ_LP   0x00000001
#define AUX_SYSIF_OPMODEREQ_REQ_A    0x00000000
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_SYSIF base address
 */
#define AUX_SYSIF_BASE       (PERIPH_BASE + 0xC6000)
/** @} */

/**
 * @brief   AUX_SYSIF register bank
 */
#define AUX_SYSIF            ((aux_sysif_regs_t *) (AUX_SYSIF_BASE))

/**
 * @brief   AUX_SYSIF functions
 * @{
 */
/**
 * @brief    Changes the AUX operational mode
 *
 * @note Only this function should be used to change the operational mode,
 * because it needs to be done in order.
 *
 * @param[in] target_opmode The opmode we want to change to.
 */
void aux_sysif_opmode_change(uint32_t target_opmode);
/** @} */

/**
 * @brief   AUX_TIMER01 registers
 */
typedef struct {
    reg32_t T0CFG; /**< Timer 0 Configuration */
    reg32_t T0CTL; /**< Timer 0 Control */
    reg32_t T0TARGET; /**< Timer 0 Target */
    reg32_t T0CNTR; /**< Timer 0 Counter */
    reg32_t T1CFG; /**< Timer 1 Configuration */
    reg32_t T1TARGET; /**< Timer 1 Target */
    reg32_t T1CTL; /**< Timer 1 Control */
    reg32_t T1CNTR; /**< Timer 0 Counter */
} aux_timer01_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TIMER01 base address
 */
#define AUX_TIMER01_BASE     (PERIPH_BASE + 0xC7000)
/** @} */

/**
 * @brief   AUX_TIMER01 register bank
 */
#define AUX_TIMER01          ((aux_timer01_regs_t *) (AUX_TIMER01_BASE))

/**
 * @brief   AUX_TIMER2 registers
 */
typedef struct {
    reg32_t CTL; /**< Timer 2 Control */
    reg32_t TARGET; /**< Timer 2 Target */
    reg32_t SHDWTARGET; /**< Timer 2 Shadow Target */
    reg32_t CNTR; /**< Timer 2 Counter */
    reg32_t PRECFG; /**< Timer 2 Prescaler Config */
    reg32_t EVCTL; /**< Timer 2 Event Control */
    reg32_t PULSETRIG; /**< Timer 2 Pulse Trigger */
    reg32_t __reserved1[0x19]; /**< Reserved */
    reg32_t CH0EVCFG; /**< Timer 2 Channel 0 Event Configuration */
    reg32_t CH0CCFG; /**< Timer 2 Channel 0 Capture Configuration */
    reg32_t CH0PCC; /**< Timer 2 Channel 0 Pipeline Capture Compare */
    reg32_t CH0CC; /**< Timer 2 Channel 0 Capture Compare */
    reg32_t CH1EVCFG; /**< Timer 2 Channel 1 Event Configuration */
    reg32_t CH1CCFG; /**< Timer 2 Channel 1 Capture Configuration */
    reg32_t CH1PCC; /**< Timer 2 Channel 1 Pipeline Capture Compare */
    reg32_t CH1CC; /**< Timer 2 Channel 1 Capture Compare */
    reg32_t CH2EVCFG; /**< Timer 2 Channel 2 Event Configuration */
    reg32_t CH2CCFG; /**< Timer 2 Channel 2 Capture Configuration */
    reg32_t CH2PCC; /**< Timer 2 Channel 2 Pipeline Capture Compare */
    reg32_t CH2CC; /**< Timer 2 Channel 2 Capture Compare */
    reg32_t CH3EVCFG; /**< Timer 2 Channel 3 Event Configuration */
    reg32_t CH3CCFG; /**< Timer 2 Channel 3 Capture Configuration */
    reg32_t CH3PCC; /**< Timer 2 Channel 3 Pipeline Capture Compare */
    reg32_t CH3CC; /**< Timer 2 Channel 3 Capture Compare */
} aux_timer2_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TIMER2 base address
 */
#define AUX_TIMER2_BASE      (PERIPH_BASE + 0xC3000)
/** @} */

/**
 * @brief   AUX_TIMER2 register bank
 */
#define AUX_TIMER2           ((aux_timer2_regs_t *) (AUX_TIMER2_BASE))

/**
 * @brief   AUX_SMPH registers
 */
typedef struct {
    reg32_t SMPH0; /**< Semaphore 0 */
    reg32_t SMPH1; /**< Semaphore 1 */
    reg32_t SMPH2; /**< Semaphore 2 */
    reg32_t SMPH3; /**< Semaphore 3 */
    reg32_t SMPH4; /**< Semaphore 4 */
    reg32_t SMPH5; /**< Semaphore 5 */
    reg32_t SMPH6; /**< Semaphore 6 */
    reg32_t SMPH7; /**< Semaphore 7 */
    reg32_t AUTOTAKE; /**< Sticky Request For Single Semaphore */
} aux_smph_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief  AUX_SMPH base address
 */
#define AUX_SMPH_BASE        (PERIPH_BASE + 0xC8000)
/** @} */

/**
 * @brief   AUX_SMPH register bank
 */
#define AUX_SMPH             ((aux_smph_regs_t *) (AUX_SMPH_BASE))

/**
 * @brief   AUX_ANAIF registers
 */
typedef struct {
    reg32_t __reserved1[0x4]; /**< Reserved */
    reg32_t ADCCTL; /**< ADC Control */
    reg32_t ADCFIFOSTAT; /**< ADC FIFO status */
    reg32_t ADCFIFO; /**< ADC FIFO */
    reg32_t ADCTRIG; /**< ADC Trigger */
    reg32_t ISRCCTL; /**< Current Source Control */
    reg32_t __reserved2[0x3]; /**< Reserved */
    reg32_t DACCTL; /**< DAC Control */
    reg32_t LPMBIASCTL; /**< Low-Power Mode Bias Control */
    reg32_t DACSMPLCTL; /**< DAC Sample Control */
    reg32_t DACSMPLCFG0; /**< DAC Sample Configuration 0 */
    reg32_t DACSMPLCFG1; /**< DAC Sample Configuration 1 */
    reg32_t DACVALUE; /**< DAC Value */
    reg32_t DACSTAT; /**< DAC Status */
} aux_anaif_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_ANAIF base address
 */
#define AUX_ANAIF_BASE       (PERIPH_BASE + 0xC9000)
/** @} */

/**
 * @brief   AUX_ANAIF register bank
 */
#define AUX_ANAIF            ((aux_anaif_regs_t *) (AUX_ANAIF_BASE))

/**
 * @brief   ADI_4_AUX registers
 */
typedef struct {
    reg8_t MUX0; /**< Multiplexer 0 */
    reg8_t MUX1; /**< Multiplexer 1 */
    reg8_t MUX2; /**< Multiplexer 2 */
    reg8_t MUX3; /**< Multiplexer 3 */
    reg8_t ISRC; /**< Current Source */
    reg8_t COMP; /**< Comparator */
    reg8_t MUX4; /**< Multiplexer 4 */
    reg8_t ADC0; /**< ADC Control 0 */
    reg8_t ADC1; /**< ADC Control 1 */
    reg8_t ADCREF0; /**< ADC Reference 0 */
    reg8_t ADCREF1; /**< ADC Reference 1 */
    reg8_t __reserved1[0x3]; /**< Reserved */
    reg8_t LPMBIAS; /**< Internal */
} adi_4_aux_regs_t;

/**
 * @brief   ADI_4_AUX registers using masked 8-bit access
 */
typedef struct {
    reg8_m8_t MUX0; /**< Multiplexer 0 */
    reg8_m8_t MUX1; /**< Multiplexer 1 */
    reg8_m8_t MUX2; /**< Multiplexer 2 */
    reg8_m8_t MUX3; /**< Multiplexer 3 */
    reg8_m8_t ISRC; /**< Current Source */
    reg8_m8_t COMP; /**< Comparator */
    reg8_m8_t MUX4; /**< Multiplexer 4 */
    reg8_m8_t ADC0; /**< ADC Control 0 */
    reg8_m8_t ADC1; /**< ADC Control 1 */
    reg8_m8_t ADCREF0; /**< ADC Reference 0 */
    reg8_m8_t ADCREF1; /**< ADC Reference 1 */
    reg8_m8_t __reserved1[0x3]; /**< Reserved */
    reg8_m8_t LPMBIAS; /**< Internal */
} adi_4_aux_regs_m8_t;

/**
 * @brief   ADI_4_AUX register values
 * @{
 */
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_m 0x00000038
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_s 3
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_m    0x0000003F
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_s    0
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   ADI_4_AUX base address
 */
#define ADI_4_AUX_BASE       (PERIPH_BASE + 0xCB000)
/**
 * @brief   ADI_4_AUX base address for masked 8-bit access
 */
#define ADI_4_AUX_BASE_M8    (ADI_4_AUX_BASE + ADI_MASK8B)
/** @} */

/**
 * @brief   ADI_4_AUX register bank
 */
#define ADI_4_AUX            ((adi_4_aux_regs_t *) (ADI_4_AUX_BASE))
/**
 * @brief   ADI_4_AUX register bank
 */
#define ADI_4_AUX_M8         ((adi_4_aux_regs_m8_t *) (ADI_4_AUX_BASE_M8))

/**
 * @brief   Semamphore used for ADDI
 */
#define ADDI_SEM             AUX_SMPH->SMPH0

#ifdef __cplusplus
} /* end extern "C" */
#endif


/** @}*/
