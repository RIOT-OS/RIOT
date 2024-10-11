/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Leon M. George <leon@georgemail.eu>
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CPU_CONF_CC26XX_CC13XX_H
#define CPU_CONF_CC26XX_CC13XX_H

#include "kernel_defines.h"

#include "cpu_conf_common.h"

#include "cc26xx_cc13xx.h"

#include "cc26xx_cc13xx_adi.h"
#include "cc26xx_cc13xx_ccfg.h"
#include "cc26xx_cc13xx_gpio.h"
#include "cc26xx_cc13xx_gpt.h"
#include "cc26xx_cc13xx_hard_api.h"
#include "cc26xx_cc13xx_i2c.h"
#include "cc26xx_cc13xx_ioc.h"
#include "cc26xx_cc13xx_rfc.h"
#include "cc26xx_cc13xx_uart.h"
#include "cc26xx_cc13xx_vims.h"
#include "cc26xx_cc13xx_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   IRQN_COUNT
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief   CC26xx/CC13xx specific CPU configuration
 * @{
 */
/**
 * @brief   This includes the CCFG configuration in the binary for flashing
 *          onto the micro-controller.
 */
#ifndef CONFIG_CC26XX_CC13XX_UPDATE_CCFG
#define CONFIG_CC26XX_CC13XX_UPDATE_CCFG            0
#endif

/**
 * @brief   Force VDDR high setting, enables higher output power but also higher
 *          power consumption.
 *
 * This is also called "boost mode".
 */
#ifndef CONFIG_CC26XX_CC13XX_FORCE_VDDR_HH
#define CONFIG_CC26XX_CC13XX_FORCE_VDDR_HH          0
#endif

/**
 * @brief   Enable GPRAM and use 8K VIMS RAM as GPRAM (instead of cache).
 *
 * @note Enabling GPRAM disables CACHE and will reduce CPU execution speed
         (up to 60%).
 * @note GPRAM is 8KB in size and located at 0x11000000-0x11001FFF if
 *       enabled.
 */
#ifndef CONFIG_CC26XX_CC13XX_GPRAM
#define CONFIG_CC26XX_CC13XX_GPRAM                  0
#endif

/**
 * @brief   This configures the level need to enter the bootloader backdoor at
 *          boot time.
 */
#if IS_ACTIVE(CONFIG_CC26XX_CC13XX_BL_LEVEL_ACTIVE_HIGH)
#define CONFIG_CC26XX_CC13XX_BL_LEVEL               0x1
#elif IS_ACTIVE(CONFIG_CC26XX_CC13XX_BL_LEVEL_ACTIVE_LOW)
#define CONFIG_CC26XX_CC13XX_BL_LEVEL               0x0
#endif

#ifndef CONFIG_CC26XX_CC13XX_BL_LEVEL
#define CONFIG_CC26XX_CC13XX_BL_LEVEL               0x1
#endif

/**
 * @brief   DIO (pin) number used to enter the bootloader backdoor at
 *          boot time.
 */
#ifndef CONFIG_CC26XX_CC13XX_BL_PIN
#define CONFIG_CC26XX_CC13XX_BL_PIN                 0xFF
#endif

/* high VDDR is available only on CC13xx CPUs */
#if IS_ACTIVE(CONFIG_CPU_FAM_CC13XX)

#if IS_ACTIVE(CONFIG_CC26XX_CC13XX_FORCE_VDDR_HH)
#define SET_MODE_CONF_1_ALT_DCDC_VMIN               0xC  /**< 2.5V */
#define SET_MODE_CONF_VDDR_EXT_LOAD                 0x1  /**< Special setting */
#endif

#endif /* IS_ACTIVE(CONFIG_CPU_FAM_CC13XX) */

#if !IS_ACTIVE(CONFIG_CC26XX_CC13XX_GPRAM)
#define SET_SIZE_AND_DIS_FLAGS_DIS_GPRAM            0x1  /**< Disable GPRAM */
#endif

#if IS_ACTIVE(CONFIG_CC26XX_CC13XX_ROM_BOOTLOADER)
#define SET_BL_CONFIG_BOOTLOADER_ENABLE             0xC5 /**< Enable */
#define SET_BL_CONFIG_BL_ENABLE                     0xC5 /**< Enable */

#if defined(CONFIG_CC26XX_CC13XX_BL_LEVEL)
#define SET_BL_CONFIG_BL_LEVEL                      CONFIG_CC26XX_CC13XX_BL_LEVEL
#endif

#if IS_ACTIVE(CONFIG_CC26XX_CC13XX_BL_PIN_EN)
#define SET_BL_CONFIG_BL_PIN_NUMBER                 CONFIG_CC26XX_CC13XX_BL_PIN
#endif

#endif /* IS_USED(CONFIG_CC26XX_CC13XX_ROM_BOOTLOADER) */

/* when GPRAM is not disabled, use it as a backup RAM */
#if IS_ACTIVE(CONFIG_CC26XX_CC13XX_DIS_GPRAM)
#define NUM_HEAPS (1)
#else
#define NUM_HEAPS (2)
#endif
/** @} */

/**
 * @brief   Customer Configuration (CCFG)
 * @{
 */
/**
 * @brief   Selects the DIO to supply external 32kHz clock as SCLK_LF when
 *          @ref SET_MODE_CONF_SCLK_LF_OPTION is set to "external LF". The
 */
#ifndef SET_EXT_LF_CLK_DIO
#define SET_EXT_LF_CLK_DIO                          0x01
#endif

/**
 * @brief   The input frequency of the external clock and is written to
 *          `AON_RTC:SUBSECINC.VALUEINC`.
 *
 * Defined as follows:
 *
 * `EXT_LF_CLK.RTC_INCREMENT = 2^38/InputClockFrequency in Hertz`
 *
 * For example:
 *
 * `RTC_INCREMENT=0x800000` for `InputClockFrequency=32768 Hz`
 */
#ifndef SET_EXT_LF_CLK_RTC_INCREMENT
#define SET_EXT_LF_CLK_RTC_INCREMENT                0x800000
#endif

#if defined(CPU_VARIANT_X2) || defined(DOXYGEN)
/**
 * @brief   Selects the TCXO type.
 * @details 0h = CMOS type. Internal common-mode bias will not be enabled.
 *          1h = Clipped-sine type. Internal common-mode bias will be enabled
 *               when TCXO is used.
 *
 * @note x2 CPUs only.
 *
 * @note Value is only valid if @ref SET_MODE_CONF_XOSC_FREQ is equal to 0.
 */
#ifndef SET_MODE_CONF_1_TCXO_TYPE
#define SET_MODE_CONF_1_TCXO_TYPE                   0x01
#endif

/**
 * @brief   Maximum TCXO startup time in units of 100us.
 *
 * @note x2 CPUs only.
 *
 * @note Value is only valid if @ref SET_MODE_CONF_XOSC_FREQ is equal to 0.
 */
#ifndef SET_MODE_CONF_1_TCXO_MAX_START
#define SET_MODE_CONF_1_TCXO_MAX_START              0x7F
#endif

#endif /* defined(CPU_VARIANT_X2) || defined(DOXYGEN) */

/**
 * @brief   Minimum voltage for when DC/DC should be used if alternate DC/DC
 *          setting is enabled.
 *
 * The VMIN voltage is defnied as follows:
 *
 * `Voltage = (28 + ALT_DCDC_VMIN) / 16`
 *
 * For example:
 *
 * 0 = 1.75 V
 * 1 = 1.8125 V
 * ...
 * 8 = 2.25 V
 * ...
 * 14 = 2.625 V
 * 15 = 2.6875 V
 */
#ifndef SET_MODE_CONF_1_ALT_DCDC_VMIN
#define SET_MODE_CONF_1_ALT_DCDC_VMIN               0x8
#endif

/**
 * @brief   Enable DC/DC dithering if alternate DC/DC setting is enabled.
 * @details 0h = Dither disable
 *          1h = Dither enable
 */
#ifndef SET_MODE_CONF_1_ALT_DCDC_DITHER_EN
#define SET_MODE_CONF_1_ALT_DCDC_DITHER_EN          0x0
#endif

/**
 * @brief   Inductor peak current if alternate DC/DC setting is enabled.
 *
 * @note Assuming 10uH external inductor!
 *
 * @note Values changes between x2 and x0 CPUs.
 */
#ifndef SET_MODE_CONF_1_ALT_DCDC_IPEAK
#define SET_MODE_CONF_1_ALT_DCDC_IPEAK              0x0
#endif

/**
 * @brief   Signed delta value for IBIAS_INIT.
 */
#ifndef SET_MODE_CONF_1_DELTA_IBIAS_INIT
#define SET_MODE_CONF_1_DELTA_IBIAS_INIT            0x0
#endif

/**
 * @brief   Signed delta value for IBIAS_OFFSET.
 */
#ifndef SET_MODE_CONF_1_DELTA_IBIAS_OFFSET
#define SET_MODE_CONF_1_DELTA_IBIAS_OFFSET          0x0
#endif

/**
 * @brief   Maximum XOSC startup time (worst case) in units of 100us.
 */
#ifndef SET_MODE_CONF_1_XOSC_MAX_START
#define SET_MODE_CONF_1_XOSC_MAX_START              0x10
#endif

/**
 * @brief   Total size of the CCFG in bytes
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG
#define SET_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG         0x0058
#endif

/**
 * @brief   Reserved by Texas Instruments for future use.
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS
#define SET_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS    \
    (CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_m >> \
     CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_s)
#endif

/**
 * @brief   Disable TCXO.
 * @details 0h = TCXO functionality enabled.
 *          1h = TCXO functionality disabled.
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_DIS_TCXO
#define SET_SIZE_AND_DIS_FLAGS_DIS_TCXO             0x1
#endif

/**
 * @brief   Disable GPRAM (or use the 8K VIMS RAM as CACHE RAM).
 * @details 0h = GPRAM is enabled and hence CACHE disabled.
 *          1h =  GPRAM is disabled and instead CACHE is enabled (default).
 *
 * @note Disabling CACHE will reduce CPU execution speed (up to 60%).
 * @note GPRAM is 8 K-bytes in size and located at 0x11000000-0x11001FFF if
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_DIS_GPRAM
#define SET_SIZE_AND_DIS_FLAGS_DIS_GPRAM            0x0
#endif

/**
 * @brief   Disable alternate DC/DC settings.
 * @details 0h = Enable alternate DC/DC settings.
 *          1h = Disable alternate DC/DC settings.
 *
 * @see @ref SET_MODE_CONF_1_ALT_DCDC_VMIN
 * @see @ref SET_MODE_CONF_1_ALT_DCDC_DITHER_EN
 * @see @ref SET_MODE_CONF_1_ALT_DCDC_IPEAK
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING
#define SET_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING 0x0
#endif

/**
 * @brief   Disable XOSC override functionality.
 * @details 0h = Enable XOSC override functionality.
 *          1h = Disable XOSC override functionality.
 *
 * @see @ref SET_MODE_CONF_1_DELTA_IBIAS_INIT
 * @see @ref SET_MODE_CONF_1_DELTA_IBIAS_OFFSET
 * @see @ref SET_MODE_CONF_1_XOSC_MAX_START
 */
#ifndef SET_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR
#define SET_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR         0x1
#endif

/**
 * @brief   Signed delta value to apply to the VDDR_TRIM_SLEEP target, minus one.
 *
 * 0x8 (-8) : Delta = -7
 * ...
 * 0xF (-1) : Delta = 0
 * 0x0 (0) : Delta = +1
 * ...
 * 0x7 (7) : Delta = +8
 */
#ifndef SET_MODE_CONF_VDDR_TRIM_SLEEP_DELTA
#define SET_MODE_CONF_VDDR_TRIM_SLEEP_DELTA         0xF
#endif

/**
 * @brief   DC/DC during recharge in powerdown.
 * @details 0h = Use the DC/DC during recharge in powerdown.
 *          1h = Do not use the DC/DC during recharge in powerdown (default).
 */
#ifndef SET_MODE_CONF_DCDC_RECHARGE
#define SET_MODE_CONF_DCDC_RECHARGE                 0x0
#endif

/**
 * @brief   DC/DC in active mode.
 * @details 0h = Use the DC/DC during active mode.
 *          1h = Do not use the DC/DC during active mode (default).
 */
#ifndef SET_MODE_CONF_DCDC_ACTIVE
#define SET_MODE_CONF_DCDC_ACTIVE                   0x0
#endif

/**
 * @brief   Reserved for future use byte TI. However it's used to
 *          enable VDDR_HH setting, with an "special value".
 */
#ifndef SET_MODE_CONF_VDDR_EXT_LOAD
#define SET_MODE_CONF_VDDR_EXT_LOAD                 0x0
#endif

/**
 * @brief   VDDS BOD level.
 * @details 0h = VDDS BOD level is 2.0V (necessary for external load mode, or
 *               for maximum PA output power on CC13xx).
 *          1h = VDDS BOD level is 1.8V (or 1.65V for external regulator mode)
 *               (default).
 */
#ifndef SET_MODE_CONF_VDDS_BOD_LEVEL
#define SET_MODE_CONF_VDDS_BOD_LEVEL                0x1
#endif

/**
 * @brief   LF clock option
 * @details 0h = LF clock derived from HF clock. Note: using this configuration
 *               will block the device from entering Standby mode.
 *          1h = External LF clock.
 *          2h = LF XOSC.
 *          3h = LF RCOSC.
 */
#ifndef SET_MODE_CONF_SCLK_LF_OPTION
#define SET_MODE_CONF_SCLK_LF_OPTION                0x2
#endif

/**
 * @brief   VDDR_TRIM_SLEEP_DELTA temperature compensation.
 * @details 1h = VDDR_TRIM_SLEEP_DELTA is not temperature compensated.
 *          0h = RTOS/driver temperature compensates VDDR_TRIM_SLEEP_DELTA
 *               every time standby mode is entered. This improves
 *               low-temperature RCOSC_LF frequency stability in standby mode.
 *
 * When temperature compensation is performed, the delta is calculates this way:
 *
 * Delta = max (delta, min(8, floor(62-temp)/8))
 *
 * Here, delta is given by VDDR_TRIM_SLEEP_DELTA, and temp is the current
 * temperature in degrees C.
 */
#ifndef SET_MODE_CONF_VDDR_TRIM_SLEEP_TC
#define SET_MODE_CONF_VDDR_TRIM_SLEEP_TC            0x1
#endif

/**
 * @brief   Reserved for future use by TI.
 */
#ifndef SET_MODE_CONF_RTC_COMP
#define SET_MODE_CONF_RTC_COMP                      0x1
#endif

/**
 * @brief   External crystal frequency.
 * @details 1h = HPOSC
 *          2h = 48 MHz
 *          3h = 24 MHz
 *
 * On x2 chips 48 MHz is the default, on x0 chips it's 24 MHz
 */
#ifndef SET_MODE_CONF_XOSC_FREQ
#define SET_MODE_CONF_XOSC_FREQ                     0x2
#endif

/**
 * @brief   Enable modification (delta) to XOSC cap-array. Value specified in
 *          XOSC_CAPARRAY_DELTA.
 * @details 0h = Apply cap-array delta.
 *          1h = Do not apply cap-array delta (default).
 */
#ifndef SET_MODE_CONF_XOSC_CAP_MOD
#define SET_MODE_CONF_XOSC_CAP_MOD                  0x1
#endif

/**
 * @brief   Reserved for future use by TI.
 */
#ifndef SET_MODE_CONF_HF_COMP
#define SET_MODE_CONF_HF_COMP                       0x1
#endif

/**
 * @brief   Modifies trimmed XOSC cap-array step value.
 *
 * Enabled by @ref SET_MODE_CONF_XOSC_CAP_MOD.
 */
#ifndef SET_MODE_CONF_XOSC_CAPARRAY_DELTA
#define SET_MODE_CONF_XOSC_CAPARRAY_DELTA           0xFF
#endif

/**
 * @brief   Represents the minimum decoupling capacitance (worst case) on VDDR,
 *          in units of 100nF.
 *
 * This should take into account capacitor tolerance and voltage dependent
 * capacitance variation. This bit affects the recharge period calculation when
 * going into powerdown or standby.
 */
#ifndef SET_MODE_CONF_VDDR_CAP
#define SET_MODE_CONF_VDDR_CAP                      0x3A
#endif

/**
 * @brief   Bootloader enable. Boot loader can be accessed if
 *          IMAGE_VALID_CONF.IMAGE_VALID is non-zero or BL_ENABLE is enabled
 *          (and conditions for boot loader backdoor are met).
 * @details C5h = Boot loader is enabled.
 *          Any other value = Boot loader is disabled.
 */
#ifndef SET_BL_CONFIG_BOOTLOADER_ENABLE
#define SET_BL_CONFIG_BOOTLOADER_ENABLE             0x00
#endif

/**
 * @brief   Sets the active level of the selected DIO number BL_PIN_NUMBER if
 *          boot loader backdoor is enabled by the BL_ENABLE field.
 * @details 0h = Active low.
 *          1h = Active high.
 */
#ifndef SET_BL_CONFIG_BL_LEVEL
#define SET_BL_CONFIG_BL_LEVEL                      0x1
#endif

/**
 * @brief   DIO number that is level checked if the boot loader backdoor is
 *          enabled by the @ref SET_BL_CONFIG_BL_ENABLE setting.
 */
#ifndef SET_BL_CONFIG_BL_PIN_NUMBER
#define SET_BL_CONFIG_BL_PIN_NUMBER                 0xFF
#endif

/**
 * @brief   Enables the boot loader backdoor.
 * @details C5h = Boot loader backdoor is enabled.
 *          Any other value = Boot loader backdoor is disabled.
 */
#ifndef SET_BL_CONFIG_BL_ENABLE
#define SET_BL_CONFIG_BL_ENABLE                     0xFF
#endif

/**
 * @brief   Enable CPU DAP.
 * @details C5h = Main CPU DAP access is enabled.
 *          Any other value = Main CPU DAP access will remain disabled.
 */
#ifndef SET_CCFG_TAP_DAP_0_CPU_DAP_ENABLE
#define SET_CCFG_TAP_DAP_0_CPU_DAP_ENABLE           0xC5
#endif

/**
 * @brief   Enable PWRPROF TAP (PRCM on x0 CPUs).
 * @details C5h = PWRPROF TAP access is enabled.
 *          Any other value = PWRPROF TAP access will remain disabled.
 */
#ifndef SET_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE
#define SET_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE       0xC5
#endif

/**
 * @brief   Enable Test TAP.
 * @details C5h = TEST TAP access is enabled.
 *          Any other value = TEST TAP access will remain disabled.
 */
#ifndef SET_CCFG_TAP_DAP_0_TEST_TAP_ENABLE
#define SET_CCFG_TAP_DAP_0_TEST_TAP_ENABLE          0x00
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CONF_CC26XX_CC13XX_H */
