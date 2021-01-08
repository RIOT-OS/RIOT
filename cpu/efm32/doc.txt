/**
 * @defgroup        cpu_efm32 Silicon Labs EFM32/EFR32/EZR32
 * @ingroup         cpu
 * @brief           Silicon Labs's EFM32/EFR32/EZR32 MCUs implementation
 *
 * This module contains all code and definitions for the Silicon Labs
 * EFM32/EFR32/EZR32 MCUs. It uses the Gecko SDK (vendor library) for the
 * peripheral drivers.
 *
 *
 * Supported Peripherals
 * =====================
 *
 * The following peripherals are supported (depends on microcontroller):
 *   - ADC
 *   - CPUID
 *   - DAC
 *   - Flash page
 *   - GPIO
 *   - HW RNG
 *   - I2C
 *   - Power Management
 *   - PWM
 *   - RTC
 *   - RTT
 *   - SPI
 *   - Timer
 *   - UART (including low-power)
 *
 *
 * Clock Configuration
 * ===================
 *
 * By default the microcontroller will run on the internal RC-oscillator. If
 * an external crystal is available, you can configure it to use by setting
 * `CLOCK_HF=cmuSelect_HFXO`. The same applies for `CLOCK_LFA`, `CLOCK_LFB` and
 * `CLOCK_LFE` using `cmuSelect_LFXO`.
 *
 * If the internal RC-oscillator is not used, it will be disabled.
 *
 * Refer to the reference manual of the specific microcontroller for the
 * specifics.
 *
 *
 * EMU and CMU Configuration
 * =========================
 *
 * The Energy Management Unit (EMU) and Clock Management Unit (CMU) are
 * initialized using default values provided by the Gecko SDK. You can override
 * any of the following defaults to use other values:
 *
 *   - `CMU_HFXOINIT`
 *   - `CMU_LFXOINIT`
 *   - `EMU_DCDCINIT`
 *   - `EMU_EM23INIT`
 *   - `EMU_EM4INIT`
 *
 * Refer to the Gecko SDK for more information about these values.
 *
 *
 * Low-power Configuration
 * =======================
 *
 * The EFM32/EFR32/EZR32 MCUs have support for low-power peripherals. Support
 * is enabled by default, but can be disabled if not used.
 *
 * - Setting `EFM32_LEUART_ENABLED=0` will disable support for the LEUART
 *   in the UART driver peripheral and save approximately 400 bytes.
 *
 * Refer to `cpu/efm32/efm32-features.mk` for more options.
 */

/**
 * @defgroup        cpu_efm32gg Silicon Labs EFM32GG family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32GG MCUs family implementation
 */

/**
 * @defgroup        cpu_efm32hg Silicon Labs EFM32HG family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32HG MCUs family implementation
 */

/**
 * @defgroup        cpu_efm32lg Silicon Labs EFM32LG family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32LG MCUs family implementation
 */

/**
 * @defgroup        cpu_efm32pg1b Silicon Labs EFM32PG1B family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32PG1B MCUs family implementation
 */

/**
 * @defgroup        cpu_efm32pg12b Silicon Labs EFM32PG12B family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32PG12B MCUs family implementation
 */

/**
 * @defgroup        cpu_efm32zg Silicon Labs EFM32ZG family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFM32ZG MCUs family implementation
 */

/**
 * @defgroup        cpu_efr32mg1b Silicon Labs EFR32MG1P family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFR32MG1B MCUs family implementation
 */

/**
 * @defgroup        cpu_efr32mg1p Silicon Labs EFR32MG1P family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFR32MG1P MCUs family implementation
 */

/**
 * @defgroup        cpu_efr32mg12p Silicon Labs EFR32MG12P family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EFR32MG12P MCUs family implementation
 */

/**
 * @defgroup        cpu_ezr32wg Silicon Labs EZR32WG family
 * @ingroup         cpu_efm32
 * @brief           Silicon Labs's EZR32WG MCUs family implementation
 */