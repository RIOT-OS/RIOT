/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_qn908x
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for NXP QN908x MCUs
 *
 * @author iosabi <iosabi@protonmail.com>
 */

/**
 * @name Interrupt vector definition
 * @{
 */

#include "board.h"
#include "vectors_cortexm.h"
#include "vectors_qn908x.h"

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [EXT_GPIO_WAKEUP_IRQn] = isr_ext_gpio_wakeup,   /* Ext GPIO wakeup */
    [OSC_IRQn         ] = isr_osc,                  /* BLE wakeup */
    [ACMP0_IRQn       ] = isr_acmp0,                /* Analog comparator0 */
    [ACMP1_IRQn       ] = isr_acmp1,                /* Analog comparator1 */
    [RTC_SEC_IRQn     ] = isr_rtc_sec,              /* RTC second */
    [RTC_FR_IRQn      ] = isr_rtc_fr,               /* RTC free running */
    [CS_WAKEUP_IRQn   ] = isr_cs_wakeup,            /* Capacitive sense wakeup */
    [CS_IRQn          ] = isr_cs,                   /* Capacitive sense */
    [GPIOA_IRQn       ] = isr_gpioa,                /* GPIO group A */
    [GPIOB_IRQn       ] = isr_gpiob,                /* GPIO group B */
    [DMA0_IRQn        ] = isr_dma0,                 /* DMA controller */
    [PIN_INT0_IRQn    ] = isr_pin_int0,             /* pin or pattern match engine slice 0 */
    [PIN_INT1_IRQn    ] = isr_pin_int1,             /* pin or pattern match engine slice 1 */
    [PIN_INT2_IRQn    ] = isr_pin_int2,             /* pin or pattern match engine slice 2 */
    [PIN_INT3_IRQn    ] = isr_pin_int3,             /* pin or pattern match engine slice 3 */
    [OSC_INT_LOW_IRQn ] = isr_osc_int_low,          /* Inverse of OSC */
    [USB0_IRQn        ] = isr_usb0,                 /* USB device */
    [FLEXCOMM0_IRQn   ] = isr_flexcomm0,            /* Flexcomm Interface 0 (USART) */
    [FLEXCOMM1_IRQn   ] = isr_flexcomm1,            /* Flexcomm Interface 1 (USART, I2C) */
    [FLEXCOMM2_IRQn   ] = isr_flexcomm2,            /* Flexcomm Interface 2 (SPI, I2C) */
    [FLEXCOMM3_IRQn   ] = isr_flexcomm3,            /* Flexcomm Interface 3 (SPI) */
    [BLE_IRQn         ] = isr_ble,                  /* BLE interrupts */
    [FSP_IRQn         ] = isr_fsp,                  /* FSP */
    [QDEC0_IRQn       ] = isr_qdec0,                /* QDEC0 */
    [QDEC1_IRQn       ] = isr_qdec1,                /* QDEC1 */
    [CTIMER0_IRQn     ] = isr_ctimer0,              /* Standard counter/timer CTIMER0 */
    [CTIMER1_IRQn     ] = isr_ctimer1,              /* Standard counter/timer CTIMER1 */
    [CTIMER2_IRQn     ] = isr_ctimer2,              /* Standard counter/timer CTIMER2 */
    [CTIMER3_IRQn     ] = isr_ctimer3,              /* Standard counter/timer CTIMER3 */
    [WDT_IRQn         ] = isr_wdt,                  /* Watch dog timer */
    [ADC_IRQn         ] = isr_adc,                  /* ADC */
    [DAC_IRQn         ] = isr_dac,                  /* DAC */
    [XTAL_READY_IRQn  ] = isr_xtal_ready,           /* High frequency crystal ready */
    [FLASH_IRQn       ] = isr_flash,                /* Flash */
    [SPIFI0_IRQn      ] = isr_spifi0,               /* SPI flash interface */
    [SCT0_IRQn        ] = isr_sct0,                 /* SCTimer/PWM */
    [RNG_IRQn         ] = isr_rng,                  /* Random number generator */
    [CALIB_IRQn       ] = isr_calib,                /* Calibration */
    [BLE_TX_IRQn      ] = isr_ble_tx,               /* ble tx flag */
    [BLE_RX_IRQn      ] = isr_ble_rx,               /* ble rx flag */
    [BLE_FREQ_HOP_IRQn] = isr_ble_freq_hop,         /* ble frequency hop */
    [BOD_IRQn         ] = isr_bod,                  /* Brown out detect */
};

__attribute__((weak)) const uint32_t isp_configuration = 0;

/** @} */
