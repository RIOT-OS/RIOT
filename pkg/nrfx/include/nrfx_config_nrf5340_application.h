/*
 * Copyright (c) 2019 - 2024, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFX_CONFIG_NRF5340_APPLICATION_H__
#define NRFX_CONFIG_NRF5340_APPLICATION_H__

#ifndef NRFX_CONFIG_H__
#error "This file should not be included directly. Include nrfx_config.h instead."
#endif

/*
 * The MDK provides macros for accessing the peripheral register structures
 * by using their secure and non-secure address mappings (with the names
 * containing the suffix _S or _NS, respectively). Because the nrfx drivers
 * use the macros without any suffixes, you must translate the names.
 * The following section provides configuration for the name translation.
 * It must be modified to reflect the actual configuration set in NRF_SPU.
 */
#define NRF_CLOCK        NRF_CLOCK_S
#define NRF_COMP         NRF_COMP_S
#define NRF_DCNF         NRF_DCNF_S
#define NRF_DPPIC        NRF_DPPIC_S
#define NRF_EGU0         NRF_EGU0_S
#define NRF_EGU1         NRF_EGU1_S
#define NRF_EGU2         NRF_EGU2_S
#define NRF_EGU3         NRF_EGU3_S
#define NRF_EGU4         NRF_EGU4_S
#define NRF_EGU5         NRF_EGU5_S
#define NRF_FPU          NRF_FPU_S
#define NRF_I2S0         NRF_I2S0_S
#define NRF_IPC          NRF_IPC_S
#define NRF_KMU          NRF_KMU_S
#define NRF_LPCOMP       NRF_LPCOMP_S
#define NRF_MUTEX        NRF_MUTEX_S
#define NRF_NFCT         NRF_NFCT_S
#define NRF_NVMC         NRF_NVMC_S
#define NRF_OSCILLATORS  NRF_OSCILLATORS_S
#define NRF_P0           NRF_P0_S
#define NRF_P1           NRF_P1_S
#define NRF_PDM0         NRF_PDM0_S
#define NRF_POWER        NRF_POWER_S
#define NRF_PWM0         NRF_PWM0_S
#define NRF_PWM1         NRF_PWM1_S
#define NRF_PWM2         NRF_PWM2_S
#define NRF_PWM3         NRF_PWM3_S
#define NRF_QDEC0        NRF_QDEC0_S
#define NRF_QDEC1        NRF_QDEC1_S
#define NRF_QSPI         NRF_QSPI_S
#define NRF_REGULATORS   NRF_REGULATORS_S
#define NRF_RESET        NRF_RESET_S
#define NRF_RTC0         NRF_RTC0_S
#define NRF_RTC1         NRF_RTC1_S
#define NRF_SAADC        NRF_SAADC_S
#define NRF_SPIM0        NRF_SPIM0_S
#define NRF_SPIM1        NRF_SPIM1_S
#define NRF_SPIM2        NRF_SPIM2_S
#define NRF_SPIM3        NRF_SPIM3_S
#define NRF_SPIM4        NRF_SPIM4_S
#define NRF_SPIS0        NRF_SPIS0_S
#define NRF_SPIS1        NRF_SPIS1_S
#define NRF_SPIS2        NRF_SPIS2_S
#define NRF_SPIS3        NRF_SPIS3_S
#define NRF_TIMER0       NRF_TIMER0_S
#define NRF_TIMER1       NRF_TIMER1_S
#define NRF_TIMER2       NRF_TIMER2_S
#define NRF_TWIM0        NRF_TWIM0_S
#define NRF_TWIM1        NRF_TWIM1_S
#define NRF_TWIM2        NRF_TWIM2_S
#define NRF_TWIM3        NRF_TWIM3_S
#define NRF_TWIS0        NRF_TWIS0_S
#define NRF_TWIS1        NRF_TWIS1_S
#define NRF_TWIS2        NRF_TWIS2_S
#define NRF_TWIS3        NRF_TWIS3_S
#define NRF_UARTE0       NRF_UARTE0_S
#define NRF_UARTE1       NRF_UARTE1_S
#define NRF_UARTE2       NRF_UARTE2_S
#define NRF_UARTE3       NRF_UARTE3_S
#define NRF_USBD         NRF_USBD_S
#define NRF_USBREGULATOR NRF_USBREGULATOR_S
#define NRF_VMC          NRF_VMC_S
#define NRF_WDT0         NRF_WDT0_S
#define NRF_WDT1         NRF_WDT1_S

/*
 * The following section provides the name translation for peripherals with
 * only one type of access available. For these peripherals, you cannot choose
 * between secure and non-secure mapping.
 */
#if !defined(NRF_TRUSTZONE_NONSECURE)
#define NRF_CACHE        NRF_CACHE_S
#define NRF_CACHEINFO    NRF_CACHEINFO_S
#define NRF_CACHEDATA    NRF_CACHEDATA_S
#define NRF_CRYPTOCELL   NRF_CRYPTOCELL_S
#define NRF_CTI          NRF_CTI_S
#define NRF_FICR         NRF_FICR_S
#define NRF_SPU          NRF_SPU_S
#define NRF_TAD          NRF_TAD_S
#define NRF_UICR         NRF_UICR_S
#endif

/* Fixups for the GPIOTE driver. */
#define NRF_GPIOTE0      NRF_GPIOTE0_S
#define NRF_GPIOTE1      NRF_GPIOTE1_NS

/**
 * @brief NRFX_DEFAULT_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_DEFAULT_IRQ_PRIORITY
#define NRFX_DEFAULT_IRQ_PRIORITY 7
#endif

/**
 * @brief NRFX_CLOCK_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_CLOCK_ENABLED
#define NRFX_CLOCK_ENABLED 0
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_HFCLK192M_SRC
 *
 * Integer value.
 * Supported values:
 * - HFINT = 0
 * - HFXO  = 1
 */
#ifndef NRFX_CLOCK_CONFIG_HFCLK192M_SRC
#define NRFX_CLOCK_CONFIG_HFCLK192M_SRC 1
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_LF_SRC
 *
 * Integer value.
 * Supported values:
 * - RC    = 1
 * - XTAL  = 2
 * - Synth = 3
 */
#ifndef NRFX_CLOCK_CONFIG_LF_SRC
#define NRFX_CLOCK_CONFIG_LF_SRC 2
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_LF_CAL_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_CLOCK_CONFIG_LF_CAL_ENABLED
#define NRFX_CLOCK_CONFIG_LF_CAL_ENABLED 0
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_LFXO_TWO_STAGE_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_CLOCK_CONFIG_LFXO_TWO_STAGE_ENABLED
#define NRFX_CLOCK_CONFIG_LFXO_TWO_STAGE_ENABLED 0
#endif

/**
 * @brief NRFX_CLOCK_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_CLOCK_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_CLOCK_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_CLOCK_CONFIG_LOG_ENABLED
#define NRFX_CLOCK_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_CLOCK_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_CLOCK_CONFIG_LOG_LEVEL
#define NRFX_CLOCK_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_COMP_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_COMP_ENABLED
#define NRFX_COMP_ENABLED 0
#endif

/**
 * @brief NRFX_COMP_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_COMP_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_COMP_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_COMP_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_COMP_CONFIG_LOG_ENABLED
#define NRFX_COMP_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_COMP_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_COMP_CONFIG_LOG_LEVEL
#define NRFX_COMP_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_DPPI_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_DPPI_ENABLED
#define NRFX_DPPI_ENABLED 0
#endif

/**
 * @brief NRFX_DPPI_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_DPPI_CONFIG_LOG_ENABLED
#define NRFX_DPPI_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_DPPI_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_DPPI_CONFIG_LOG_LEVEL
#define NRFX_DPPI_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_EGU_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU_ENABLED
#define NRFX_EGU_ENABLED 0
#endif

/**
 * @brief NRFX_EGU_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_EGU_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_EGU_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_EGU0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU0_ENABLED
#define NRFX_EGU0_ENABLED 0
#endif

/**
 * @brief NRFX_EGU1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU1_ENABLED
#define NRFX_EGU1_ENABLED 0
#endif

/**
 * @brief NRFX_EGU2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU2_ENABLED
#define NRFX_EGU2_ENABLED 0
#endif

/**
 * @brief NRFX_EGU3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU3_ENABLED
#define NRFX_EGU3_ENABLED 0
#endif

/**
 * @brief NRFX_EGU4_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU4_ENABLED
#define NRFX_EGU4_ENABLED 0
#endif

/**
 * @brief NRFX_EGU5_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_EGU5_ENABLED
#define NRFX_EGU5_ENABLED 0
#endif

/**
 * @brief NRFX_GPIOTE_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_GPIOTE_ENABLED
#define NRFX_GPIOTE_ENABLED 0
#endif

/**
 * @brief NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_GPIOTE_CONFIG_NUM_OF_EVT_HANDLERS
 *
 * Integer value. Minimum: 0. Maximum: 15.
 */
#ifndef NRFX_GPIOTE_CONFIG_NUM_OF_EVT_HANDLERS
#define NRFX_GPIOTE_CONFIG_NUM_OF_EVT_HANDLERS 2
#endif

/**
 * @brief NRFX_GPIOTE_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_GPIOTE_CONFIG_LOG_ENABLED
#define NRFX_GPIOTE_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_GPIOTE_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_GPIOTE_CONFIG_LOG_LEVEL
#define NRFX_GPIOTE_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_GPIOTE0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_GPIOTE0_ENABLED
#define NRFX_GPIOTE0_ENABLED 0
#endif

/**
 * @brief NRFX_GPIOTE1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_GPIOTE1_ENABLED
#define NRFX_GPIOTE1_ENABLED 0
#endif

/**
 * @brief NRFX_I2S_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_I2S_ENABLED
#define NRFX_I2S_ENABLED 0
#endif

/**
 * @brief NRFX_I2S_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_I2S_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_I2S_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_I2S_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_I2S_CONFIG_LOG_ENABLED
#define NRFX_I2S_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_I2S_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_I2S_CONFIG_LOG_LEVEL
#define NRFX_I2S_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_I2S0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_I2S0_ENABLED
#define NRFX_I2S0_ENABLED 0
#endif

/**
 * @brief NRFX_IPC_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_IPC_ENABLED
#define NRFX_IPC_ENABLED 0
#endif

/**
 * @brief NRFX_LPCOMP_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_LPCOMP_ENABLED
#define NRFX_LPCOMP_ENABLED 0
#endif

/**
 * @brief NRFX_LPCOMP_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_LPCOMP_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_LPCOMP_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_LPCOMP_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_LPCOMP_CONFIG_LOG_ENABLED
#define NRFX_LPCOMP_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_LPCOMP_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_LPCOMP_CONFIG_LOG_LEVEL
#define NRFX_LPCOMP_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_NFCT_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_NFCT_ENABLED
#define NRFX_NFCT_ENABLED 0
#endif

/**
 * @brief NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_NFCT_CONFIG_TIMER_INSTANCE_ID - Timer instance used for workarounds in the driver.
 *
 * Integer value. Minimum: 0. Maximum: 5.
 */
#ifndef NRFX_NFCT_CONFIG_TIMER_INSTANCE_ID
#define NRFX_NFCT_CONFIG_TIMER_INSTANCE_ID 2
#endif

/**
 * @brief NRFX_NFCT_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_NFCT_CONFIG_LOG_ENABLED
#define NRFX_NFCT_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_NFCT_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_NFCT_CONFIG_LOG_LEVEL
#define NRFX_NFCT_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_NVMC_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_NVMC_ENABLED
#define NRFX_NVMC_ENABLED 0
#endif

/**
 * @brief NRFX_PDM_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PDM_ENABLED
#define NRFX_PDM_ENABLED 0
#endif

/**
 * @brief NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_PDM_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PDM_CONFIG_LOG_ENABLED
#define NRFX_PDM_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_PDM_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_PDM_CONFIG_LOG_LEVEL
#define NRFX_PDM_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_POWER_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_POWER_ENABLED
#define NRFX_POWER_ENABLED 0
#endif

/**
 * @brief NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_PRS_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_ENABLED
#define NRFX_PRS_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_CONFIG_LOG_ENABLED
#define NRFX_PRS_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_PRS_CONFIG_LOG_LEVEL
#define NRFX_PRS_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_PRS_BOX_0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_BOX_0_ENABLED
#define NRFX_PRS_BOX_0_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_BOX_1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_BOX_1_ENABLED
#define NRFX_PRS_BOX_1_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_BOX_2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_BOX_2_ENABLED
#define NRFX_PRS_BOX_2_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_BOX_3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_BOX_3_ENABLED
#define NRFX_PRS_BOX_3_ENABLED 0
#endif

/**
 * @brief NRFX_PRS_BOX_4_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PRS_BOX_4_ENABLED
#define NRFX_PRS_BOX_4_ENABLED 0
#endif

/**
 * @brief NRFX_PWM_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM_ENABLED
#define NRFX_PWM_ENABLED 0
#endif

/**
 * @brief NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_PWM_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM_CONFIG_LOG_ENABLED
#define NRFX_PWM_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_PWM_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_PWM_CONFIG_LOG_LEVEL
#define NRFX_PWM_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_PWM0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM0_ENABLED
#define NRFX_PWM0_ENABLED 0
#endif

/**
 * @brief NRFX_PWM1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM1_ENABLED
#define NRFX_PWM1_ENABLED 0
#endif

/**
 * @brief NRFX_PWM2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM2_ENABLED
#define NRFX_PWM2_ENABLED 0
#endif

/**
 * @brief NRFX_PWM3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_PWM3_ENABLED
#define NRFX_PWM3_ENABLED 0
#endif

/**
 * @brief NRFX_QDEC_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_QDEC_ENABLED
#define NRFX_QDEC_ENABLED 0
#endif

/**
 * @brief NRFX_QDEC_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_QDEC_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_QDEC_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_QDEC_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_QDEC_CONFIG_LOG_ENABLED
#define NRFX_QDEC_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_QDEC_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_QDEC_CONFIG_LOG_LEVEL
#define NRFX_QDEC_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_QDEC0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_QDEC0_ENABLED
#define NRFX_QDEC0_ENABLED 0
#endif

/**
 * @brief NRFX_QDEC1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_QDEC1_ENABLED
#define NRFX_QDEC1_ENABLED 0
#endif

/**
 * @brief NRFX_QSPI_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_QSPI_ENABLED
#define NRFX_QSPI_ENABLED 0
#endif

/**
 * @brief NRFX_QSPI_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_QSPI_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_QSPI_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_RTC_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_RTC_ENABLED
#define NRFX_RTC_ENABLED 0
#endif

/**
 * @brief NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_RTC_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_RTC_CONFIG_LOG_ENABLED
#define NRFX_RTC_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_RTC_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_RTC_CONFIG_LOG_LEVEL
#define NRFX_RTC_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_RTC0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_RTC0_ENABLED
#define NRFX_RTC0_ENABLED 0
#endif

/**
 * @brief NRFX_RTC1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_RTC1_ENABLED
#define NRFX_RTC1_ENABLED 0
#endif

/**
 * @brief NRFX_SAADC_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SAADC_ENABLED
#define NRFX_SAADC_ENABLED 0
#endif

/**
 * @brief NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_SAADC_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SAADC_CONFIG_LOG_ENABLED
#define NRFX_SAADC_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_SAADC_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_SAADC_CONFIG_LOG_LEVEL
#define NRFX_SAADC_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_SPIM_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM_ENABLED
#define NRFX_SPIM_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_SPIM_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM_CONFIG_LOG_ENABLED
#define NRFX_SPIM_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_SPIM_CONFIG_LOG_LEVEL
#define NRFX_SPIM_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_SPIM0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM0_ENABLED
#define NRFX_SPIM0_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM1_ENABLED
#define NRFX_SPIM1_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM4_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM4_ENABLED
#define NRFX_SPIM4_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM2_ENABLED
#define NRFX_SPIM2_ENABLED 0
#endif

/**
 * @brief NRFX_SPIM3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIM3_ENABLED
#define NRFX_SPIM3_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS_ENABLED
#define NRFX_SPIS_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_SPIS_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS_CONFIG_LOG_ENABLED
#define NRFX_SPIS_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_SPIS_CONFIG_LOG_LEVEL
#define NRFX_SPIS_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_SPIS0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS0_ENABLED
#define NRFX_SPIS0_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS1_ENABLED
#define NRFX_SPIS1_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS2_ENABLED
#define NRFX_SPIS2_ENABLED 0
#endif

/**
 * @brief NRFX_SPIS3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SPIS3_ENABLED
#define NRFX_SPIS3_ENABLED 0
#endif

/**
 * @brief NRFX_SYSTICK_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_SYSTICK_ENABLED
#define NRFX_SYSTICK_ENABLED 0
#endif

/**
 * @brief NRFX_TIMER_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TIMER_ENABLED
#define NRFX_TIMER_ENABLED 0
#endif

/**
 * @brief NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_TIMER_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TIMER_CONFIG_LOG_ENABLED
#define NRFX_TIMER_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_TIMER_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_TIMER_CONFIG_LOG_LEVEL
#define NRFX_TIMER_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_TIMER0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TIMER0_ENABLED
#define NRFX_TIMER0_ENABLED 0
#endif

/**
 * @brief NRFX_TIMER1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TIMER1_ENABLED
#define NRFX_TIMER1_ENABLED 0
#endif

/**
 * @brief NRFX_TIMER2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TIMER2_ENABLED
#define NRFX_TIMER2_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM_ENABLED
#define NRFX_TWIM_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_TWIM_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM_CONFIG_LOG_ENABLED
#define NRFX_TWIM_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_TWIM_CONFIG_LOG_LEVEL
#define NRFX_TWIM_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_TWIM0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM0_ENABLED
#define NRFX_TWIM0_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM1_ENABLED
#define NRFX_TWIM1_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM2_ENABLED
#define NRFX_TWIM2_ENABLED 0
#endif

/**
 * @brief NRFX_TWIM3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIM3_ENABLED
#define NRFX_TWIM3_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS_ENABLED
#define NRFX_TWIS_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_TWIS_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_TWIS_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_TWIS_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS_CONFIG_LOG_ENABLED
#define NRFX_TWIS_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS_ASSUME_INIT_AFTER_RESET_ONLY - Assume that any instance would be initialized only once.
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS_ASSUME_INIT_AFTER_RESET_ONLY
#define NRFX_TWIS_ASSUME_INIT_AFTER_RESET_ONLY 0
#endif

/**
 * @brief NRFX_TWIS_NO_SYNC_MODE - Remove support for synchronous mode.
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS_NO_SYNC_MODE
#define NRFX_TWIS_NO_SYNC_MODE 0
#endif

/**
 * @brief NRFX_TWIS_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_TWIS_CONFIG_LOG_LEVEL
#define NRFX_TWIS_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_TWIS0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS0_ENABLED
#define NRFX_TWIS0_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS1_ENABLED
#define NRFX_TWIS1_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS2_ENABLED
#define NRFX_TWIS2_ENABLED 0
#endif

/**
 * @brief NRFX_TWIS3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_TWIS3_ENABLED
#define NRFX_TWIS3_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE_ENABLED
#define NRFX_UARTE_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE_CONFIG_SKIP_GPIO_CONFIG - If enabled, support for configuring GPIO pins is removed from the driver
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE_CONFIG_SKIP_GPIO_CONFIG
#define NRFX_UARTE_CONFIG_SKIP_GPIO_CONFIG 0
#endif

/**
 * @brief NRFX_UARTE_CONFIG_SKIP_PSEL_CONFIG - If enabled, support for configuring PSEL registers is removed from the driver
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE_CONFIG_SKIP_PSEL_CONFIG
#define NRFX_UARTE_CONFIG_SKIP_PSEL_CONFIG 0
#endif

/**
 * @brief NRFX_UARTE_CONFIG_TX_LINK - If enabled, driver supports linking of TX transfers.
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE_CONFIG_TX_LINK
#define NRFX_UARTE_CONFIG_TX_LINK 1
#endif

/**
 * @brief NRFX_UARTE_RX_FIFO_FLUSH_WORKAROUND_MAGIC_BYTE
 *
 * Integer value. Minimum: 0. Maximum: 255.
 */
#ifndef NRFX_UARTE_RX_FIFO_FLUSH_WORKAROUND_MAGIC_BYTE
#define NRFX_UARTE_RX_FIFO_FLUSH_WORKAROUND_MAGIC_BYTE 171
#endif

/**
 * @brief NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_UARTE_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE_CONFIG_LOG_ENABLED
#define NRFX_UARTE_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_UARTE_CONFIG_LOG_LEVEL
#define NRFX_UARTE_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_UARTE0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE0_ENABLED
#define NRFX_UARTE0_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE1_ENABLED
#define NRFX_UARTE1_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE2_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE2_ENABLED
#define NRFX_UARTE2_ENABLED 0
#endif

/**
 * @brief NRFX_UARTE3_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_UARTE3_ENABLED
#define NRFX_UARTE3_ENABLED 0
#endif

/**
 * @brief NRFX_USBD_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_USBD_ENABLED
#define NRFX_USBD_ENABLED 0
#endif

/**
 * @brief NRFX_USBD_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_USBD_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_USBD_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_USBD_CONFIG_DMASCHEDULER_ISO_BOOST - Give priority to isochronous transfers
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_USBD_CONFIG_DMASCHEDULER_ISO_BOOST
#define NRFX_USBD_CONFIG_DMASCHEDULER_ISO_BOOST 1
#endif

/**
 * @brief NRFX_USBD_CONFIG_ISO_IN_ZLP - Respond to an IN token on ISO IN endpoint with ZLP when no data is ready.
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_USBD_CONFIG_ISO_IN_ZLP
#define NRFX_USBD_CONFIG_ISO_IN_ZLP 0
#endif

/**
 * @brief NRFX_USBD_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_USBD_CONFIG_LOG_ENABLED
#define NRFX_USBD_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_USBD_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_USBD_CONFIG_LOG_LEVEL
#define NRFX_USBD_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_USBREG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_USBREG_ENABLED
#define NRFX_USBREG_ENABLED 0
#endif

/**
 * @brief NRFX_USBREG_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_USBREG_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_USBREG_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_WDT_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_WDT_ENABLED
#define NRFX_WDT_ENABLED 0
#endif

/**
 * @brief NRFX_WDT_DEFAULT_CONFIG_IRQ_PRIORITY
 *
 * Integer value. Minimum: 0. Maximum: 7.
 */
#ifndef NRFX_WDT_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_WDT_DEFAULT_CONFIG_IRQ_PRIORITY NRFX_DEFAULT_IRQ_PRIORITY
#endif

/**
 * @brief NRFX_WDT_CONFIG_NO_IRQ - Remove WDT IRQ handling from WDT driver
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_WDT_CONFIG_NO_IRQ
#define NRFX_WDT_CONFIG_NO_IRQ 0
#endif

/**
 * @brief NRFX_WDT_CONFIG_LOG_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_WDT_CONFIG_LOG_ENABLED
#define NRFX_WDT_CONFIG_LOG_ENABLED 0
#endif

/**
 * @brief NRFX_WDT_CONFIG_LOG_LEVEL
 *
 * Integer value.
 * Supported values:
 * - Off     = 0
 * - Error   = 1
 * - Warning = 2
 * - Info    = 3
 * - Debug   = 4
 */
#ifndef NRFX_WDT_CONFIG_LOG_LEVEL
#define NRFX_WDT_CONFIG_LOG_LEVEL 3
#endif

/**
 * @brief NRFX_WDT0_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_WDT0_ENABLED
#define NRFX_WDT0_ENABLED 0
#endif

/**
 * @brief NRFX_WDT1_ENABLED
 *
 * Boolean. Accepted values: 0 and 1.
 */
#ifndef NRFX_WDT1_ENABLED
#define NRFX_WDT1_ENABLED 0
#endif

#endif // NRFX_CONFIG_NRF5340_APPLICATION_H__
