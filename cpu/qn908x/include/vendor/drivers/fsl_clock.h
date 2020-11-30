/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2017 , NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CLOCK_H_
#define _FSL_CLOCK_H_

#include "fsl_common.h"

/*! @addtogroup clock */
/*! @{ */

/*! @file */

/*******************************************************************************
 * Definitions
 *****************************************************************************/

/* To calculate flexcomm clock for baud rate correction */
#define FLEXCOMM_CLK(srcClock_Hz, baudrate_Bps) ((((srcClock_Hz) / 8) / baudrate_Bps) * baudrate_Bps * 8)

/*! @brief Configure whether driver controls clock
 *
 * When set to 0, peripheral drivers will enable clock in initialize function
 * and disable clock in de-initialize function. When set to 1, peripheral
 * driver will not control the clock, application could contol the clock out of
 * the driver.
 *
 * @note All drivers share this feature switcher. If it is set to 1, application
 * should handle clock enable and disable for all drivers.
 */
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL))
#define FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL 0
#endif

/*! @brief Clock ip name array for LPUART. */
#define LPUART_CLOCKS                      \
    {                                      \
        kCLOCK_Flexcomm0, kCLOCK_Flexcomm1 \
    }
/*! @brief Clock ip name array for BI2C. */
#define BI2C_CLOCKS                        \
    {                                      \
        kCLOCK_Flexcomm1, kCLOCK_Flexcomm2 \
    }
/*! @brief Clock ip name array for FLEXCOMM. */
#define FLEXCOMM_CLOCKS                                                        \
    {                                                                          \
        kCLOCK_Flexcomm0, kCLOCK_Flexcomm1, kCLOCK_Flexcomm2, kCLOCK_Flexcomm3 \
    }
/*! @brief Clock ip name array for CRC. */
#define CRC_CLOCKS \
    {              \
        kCLOCK_Crc \
    }
/*! @brief Clock ip name array for CTIMER. */
#define CTIMER_CLOCKS                                                  \
    {                                                                  \
        kCLOCK_Ctimer0, kCLOCK_Ctimer1, kCLOCK_Ctimer2, kCLOCK_Ctimer3 \
    }
/*! @brief Clock ip name array for SCTimer. */
#define SCT_CLOCKS  \
    {               \
        kCLOCK_Sct0 \
    }
/*! @brief Clock ip name array for GPIO. */
#define GPIO_CLOCKS \
    {               \
        kCLOCK_Gpio \
    }
/*! @brief Clock ip name array for Calibration. */
#define CAL_CLOCKS \
    {              \
        kCLOCK_Cal \
    }
/*! @brief Clock ip name array for USBD. */
#define USBD_CLOCKS  \
    {                \
        kCLOCK_Usbd0 \
    }
/*! @brief Clock ip name array for WDT. */
#define WDT_CLOCKS \
    {              \
        kCLOCK_Wdt \
    }
/*! @brief Clock ip name array for BIV(including RTC and SYSCON clock). Enabled as default */
#define BIV_CLOCKS \
    {              \
        kCLOCK_Biv \
    }
/*! @brief Clock ip name array for ADC. */
#define ADC_CLOCKS \
    {              \
        kCLOCK_Adc \
    }
/*! @brief Clock ip name array for DAC. */
#define DAC_CLOCKS \
    {              \
        kCLOCK_Dac \
    }
/*! @brief Clock ip name array for CS. */
#define CS_CLOCKS \
    {             \
        kCLOCK_Cs \
    }
/*! @brief Clock ip name array for FSP. */
#define FSP_CLOCKS \
    {              \
        kCLOCK_Fsp \
    }
/*! @brief Clock ip name array for DMA. */
#define DMA_CLOCKS \
    {              \
        kCLOCK_Dma \
    }
/*! @brief Clock ip name array for QDEC. */
#define QDEC_CLOCKS                \
    {                              \
        kCLOCK_Qdec0, kCLOCK_Qdec1 \
    }
/*! @brief Clock ip name array for DP. */
#define DP_CLOCKS \
    {             \
        kCLOCK_Dp \
    }
/*! @brief Clock ip name array for SPIFI. */
#define SPIFI_CLOCKS \
    {                \
        kCLOCK_Spifi \
    }
/*! @brief Clock ip name array for BLE. */
#define BLE_CLOCKS \
    {              \
        kCLOCK_Ble \
    }
/*! @brief Clock ip name array for PROP. */
#define PROP_CLOCKS \
    {               \
        kCLOCK_Prop \
    }

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
/*------------------------------------------------------------------------------
 clock_ip_name_t definition:
------------------------------------------------------------------------------*/

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
typedef enum _clock_ip_name
{
    kCLOCK_IpInvalid = 33U,
    kCLOCK_Flexcomm0 = 0U,
    kCLOCK_Flexcomm1 = 1U,
    kCLOCK_Flexcomm2 = 2U,
    kCLOCK_Flexcomm3 = 3U,
    kCLOCK_Ctimer0 = 4U,
    kCLOCK_Ctimer1 = 5U,
    kCLOCK_Ctimer2 = 6U,
    kCLOCK_Ctimer3 = 7U,
    kCLOCK_Sct0 = 8U,
    kCLOCK_Wdt = 9U,
    kCLOCK_Usbd0 = 10U,
    kCLOCK_Gpio = 11U,
    kCLOCK_Biv = 12U,
    kCLOCK_Adc = 13U,
    kCLOCK_Dac = 14U,
    kCLOCK_Cs = 15U,
    kCLOCK_Crc = 16U,
    kCLOCK_Fsp = 16U,
    kCLOCK_Dma = 17U,
    kCLOCK_Pint = 17U,
    kCLOCK_InputMux = 17U,
    kCLOCK_Qdec0 = 19U,
    kCLOCK_Qdec1 = 20U,
    kCLOCK_Dp = 21U,
    kCLOCK_Spifi = 22U,
    kCLOCK_Cal = 25U,
    kCLOCK_Ble = 27U,
    kCLOCK_Prop = 29U,
} clock_ip_name_t;

/*! @brief Clock name used to get clock frequency. */
typedef enum _clock_name
{
    kCLOCK_CoreSysClk, /*!< Core/system clock (aka MAIN_CLK) */
    kCLOCK_BusClk,     /*!< Bus clock (AHB clock) */
    kCLOCK_ApbClk,     /*!< Apb clock */
    kCLOCK_WdtClk,     /*!< Wdt clock*/
    kCLOCK_FroHf,      /*!< FRO */
    kCLOCK_Xin,        /*!< 16/32 MHz XIN */
    kCLOCK_32KClk      /*!< 32K clock */
} clock_name_t;

/*! @brief Clock Mux Switches
 *
 *  [4 bits for choice] [8 bits mux ID]
 */

#define MUX_A(m, choice) (((m) << 0) | ((choice + 1) << 8))

#define CM_32KCLKSEL 0
#define CM_SYSCLKSEL 1
#define CM_WDTCLKSEL 2
#define CM_BLECLKSEL 3
#define CM_XTALCLKSEL 4

typedef enum _clock_attach_id
{
    kXTAL32K_to_32K_CLK = MUX_A(CM_32KCLKSEL, 0), /*!< XTAL 32K clock */
    kRCO32K_to_32K_CLK = MUX_A(CM_32KCLKSEL, 1),  /*!< RCO 32KHz clock */

    kOSC32M_to_SYS_CLK = MUX_A(CM_SYSCLKSEL, 0), /*!< OSC 32MHz clock */
    kXTAL_to_SYS_CLK = MUX_A(CM_SYSCLKSEL, 1),   /*!< XTAL 16MHz/32MHz clock */
    k32K_to_SYS_CLK = MUX_A(CM_SYSCLKSEL, 2),    /*!< 32KHz clock */

    k32K_to_WDT_CLK = MUX_A(CM_WDTCLKSEL, 0), /*!< 32KHz clock */
    kAPB_to_WDT_CLK = MUX_A(CM_WDTCLKSEL, 1), /*!< APB clock */

    k8M_to_BLE_CLK = MUX_A(CM_BLECLKSEL, 0),  /*!< 8M CLOCK */
    k16M_to_BLE_CLK = MUX_A(CM_BLECLKSEL, 1), /*!< 16M CLOCK */

    k16M_to_XTAL_CLK = MUX_A(CM_XTALCLKSEL, 0), /*!< 16M XTAL */
    k32M_to_XTAL_CLK = MUX_A(CM_XTALCLKSEL, 1), /*!< 32M XTAL */
    kNONE_to_NONE = 0x80000000U,
} clock_attach_id_t;

/*  Clock dividers */
typedef enum _clock_div_name
{
    kCLOCK_DivXtalClk,
    kCLOCK_DivOsc32mClk,
    kCLOCK_DivAhbClk,
    kCLOCK_DivApbClk,
    kCLOCK_DivFrg0,
    kCLOCK_DivFrg1,
    kCLOCK_DivClkOut
} clock_div_name_t;

/*! @brief USB clock source definition. */
typedef enum _clock_usb_src
{
    kCLOCK_UsbSrcFro =
        (uint32_t)kCLOCK_FroHf, /*!< Fake USB src clock, temporary fix until USB clock control is done properly */
} clock_usb_src_t;

/* Clock clock out source */
typedef enum _clock_clkout_src
{
    kCLOCK_Clkout_32K = SYSCON_CLK_CTRL_CLK_32K_OE_MASK,  /*!< 32KHz clock out */
    kCLOCK_Clkout_XTAL = SYSCON_CLK_CTRL_CLK_XTAL_OE_MASK /*!< XTAL clock out */
} clock_clkout_src_t;

/* Clock clock out pin */
typedef enum _clock_clkout_pin
{
    kCLOCK_Clkout_PA04_32K = SYSCON_PIO_WAKEUP_EN1_PA04_32K_OE_MASK,
    kCLOCK_Clkout_PA05_XTAL = SYSCON_PIO_WAKEUP_EN1_PA05_XTAL_OE_MASK,
    kCLOCK_Clkout_PA10_32K = SYSCON_PIO_WAKEUP_EN1_PA10_32K_OE_MASK,
    kCLOCK_Clkout_PA11_XTAL = SYSCON_PIO_WAKEUP_EN1_PA11_XTAL_OE_MASK,
    kCLOCK_Clkout_PA18_32K = SYSCON_PIO_WAKEUP_EN1_PA18_32K_OE_MASK,
    kCLOCK_Clkout_PA19_XTAL = SYSCON_PIO_WAKEUP_EN1_PA19_XTAL_OE_MASK,
    kCLOCK_Clkout_PA24_32K = SYSCON_PIO_WAKEUP_EN1_PA24_32K_OE_MASK,
    kCLOCK_Clkout_PA25_XTAL = SYSCON_PIO_WAKEUP_EN1_PA25_XTAL_OE_MASK
} clock_clkout_pin_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Enable the specified peripheral clock
 */
void CLOCK_EnableClock(clock_ip_name_t clk);

/*!
 * @brief Disable the specified peripheral clock
 */
void CLOCK_DisableClock(clock_ip_name_t clk);

/*!
 * @brief Configure the clock selection muxes.
 *
 * @param connection: Clock to be configured.
 */
void CLOCK_AttachClk(clock_attach_id_t connection);

/*!
 * @brief Setup peripheral clock dividers.
 *
 * @param div_name: Clock divider name
 * @param divided_by_value: Value to be divided
 */
void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divided_by_value);

/*!
 * @brief  Get frequency of selected clock
 *
 * @return Frequency of selected clock
 */
uint32_t CLOCK_GetFreq(clock_name_t clk);

/*!
 * @brief Disable USB FS clock.
 *
 * Disable USB FS clock.
 */
static inline void CLOCK_DisableUsbfs0Clock(void)
{
    CLOCK_DisableClock(kCLOCK_Usbd0);
}

bool CLOCK_EnableUsbfs0DeviceClock(clock_usb_src_t src, uint32_t freq);
/*!
 * @brief Enable/Disable clock out source.
 *
 * @param mask Mask value for the clock source, See "clock_clkout_src_t".
 * @param enable Enable/Disable the clock out source.
 */
void CLOCK_EnableClkoutSource(uint32_t mask, bool enable);
/*!
 * @brief Enable/Disable clock out pin.
 *
 * @param mask Mask value for the clock source, See "clock_clkout_pin_t".
 * @param enable Enable/Disable the clock out pin.
 */
void CLOCK_EnableClkoutPin(uint32_t mask, bool enable);

/*! @brief  Return Input frequency for the Fractional baud rate generator
 *  @return Input Frequency for FRG
 */
uint32_t CLOCK_GetFRGInputClock(void);

/*!
 * @brief Set output of the Fractional baud rate generator
 *
 * @param  div_name: Clock divider name: kCLOCK_DivFrg0 and kCLOCK_DivFrg1
 * @param  freq: Desired output frequency
 * @return Error Code 0 - fail 1 - success
 */
uint32_t CLOCK_SetFRGClock(clock_div_name_t div_name, uint32_t freq);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_CLOCK_H_ */
