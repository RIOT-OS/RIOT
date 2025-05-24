/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_particle-mesh
 * @{
 *
 * @file
 * @brief       Common board specific configuration for the Particle Mesh
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @name    Bootloader configuration options
 * @{
 */

/** @brief Build a firmware suitable for the Particle bootloader
 *
 * If this is defined, additional metadata about the firmware is included in
 * the firmware (called the module_info in Particle), and additional code is
 * inserted in board setup.
 *
 * Do not define this manually; instead, set `PARTICLE_MONOFIRMWARE=1` as an
 * variable in the build scripts like `BOARD` is defined, and the particle
 * common make file defines it and configures suitable postprocessing of the
 * binary.
 *
 * @see @ref boards_common_particle-mesh
 *
 */
#ifdef DOXYGEN
#define PARTICLE_MONOFIRMWARE
#endif

/** @brief Limit Particle bootloader checksumming to the binary start
 *
 * If this define is set in the Makefile, the binary size announced to the
 * bootloader is limited to the reset vector and the firmware metadata, and
 * only that part is checksummed.
 *
 * This is useful when @ref drivers_periph_flashpage is used, as otherwise the
 * firmware's writes on itself would invalidate its checksum.
 *
 * @see @ref boards_common_particle-mesh
 */
#ifdef DOXYGEN
#define PARTICLE_MONOFIRMWARE_CHECKSUMLIMIT
#endif

/** @brief Platform ID of the board for the Particle bootloader
 *
 * This is set by the individual board's build configuration, and gets used
 * when building with @ref PARTICLE_MONOFIRMWARE; then, it is put into the
 * module information for the board bootloader to verify that the firmware was
 * built for the right device.
 *
 * The individual values are documented in the Particle DeviceOS source code in
 * `build/platform-id.mk`.
 */
#ifdef DOXYGEN
#define PARTICLE_PLATFORM_ID
#endif

/** @} */

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 13)
#define LED1_PIN            GPIO_PIN(0, 14)
#define LED2_PIN            GPIO_PIN(0, 15)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 14)
#define LED2_MASK           (1 << 15)
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK)

/* The typical SAUL setup for this board uses PWM to make the LEDs (really a
 * single RGB LED) into a PWM controlled RGB LED entry. As a consequence of the
 * PWM configuration, toggling the GPIO has no effect any more, and thus we do
 * not define the macros so that no LEDs get picked up for LEDn_IS_PROVIDED.
 * (The LEDn_ON etc macros will still be present and no-op as usual, but those
 * explicitly checking for IS_PROVIDED will get an accurate picture).
 *
 * Both conditions are typically true when saul_default is on, but strictly, it
 * is those two that in combination make LEDs effectively unavailable to users.
 * */
#if !(IS_USED(MODULE_AUTO_INIT_SAUL) && IS_USED(MODULE_SAUL_PWM))

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)

#endif /* !(IS_USED(MODULE_AUTO_INIT_SAUL) && IS_USED(MODULE_SAUL_PWM)) */

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 11)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Antenna selection configuration
 * @{
 */

/** Choices in antenna outputs for the board's nRF radio
 *
 * @see board_nrfantenna_select */
enum board_nrfantenna_selection {
    /** The board's built-in antenna */
    BOARD_NRFANTENNA_BUILTIN,
    /** The board's uFL connector */
    BOARD_NRFANTENNA_EXTERNAL,
};

/** @brief Antenna output selection
 *
 * Drive the on-board antenna switch to connect the nRF radio to a given @p
 * choice of antenna output.
 *
 * This can be called to change the antenna selection at runtime; for the
 * default configuration that gets set during board initialization, see @ref
 * boards_common_particle-mesh.
 * */
void board_nrfantenna_select(enum board_nrfantenna_selection choice);

#if defined(BOARD_PARTICLE_XENON) || defined(DOXYGEN)
/** The GPIO pin used to drive the VCTL1 pin of antenna switch
 *
 * Rather than actuating this directly, consider using the @ref
 * board_nrfantenna_select function.
 */
#define VCTL1_PIN           GPIO_PIN(0, 24)
/** The GPIO pin used to drive the VCTL2 pin of antenna switch
 *
 * This definition is left out for boards whose VCTL2 is driven by an inverter
 * from VCTL1.
 *
 * Rather than actuating this directly, consider using the @ref
 * board_nrfantenna_select function.
 */
#define VCTL2_PIN           GPIO_PIN(0, 25)
#endif

#ifdef BOARD_PARTICLE_ARGON
#define VCTL1_PIN           GPIO_PIN(0, 25)
#define VCTL2_PIN           GPIO_PIN(0, 2)
#endif

#ifdef BOARD_PARTICLE_BORON
#define VCTL1_PIN           GPIO_PIN(0, 7)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
