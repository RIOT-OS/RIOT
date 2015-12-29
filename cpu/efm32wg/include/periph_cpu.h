/**
 * @ingroup         cpu_efm32wg
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Kubaszek Mateusz <mathir.km.riot@gmail.com>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#include "cpu.h"
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GPIO drive mode. */
typedef enum {
	/** Default 6mA */
	GPIO_DRIVE_6MA = GPIO_P_CTRL_DRIVEMODE_STANDARD,
	/** 0.5 mA */
	GPIO_DRIVE_05MA = GPIO_P_CTRL_DRIVEMODE_LOWEST,
	/** 20 mA */
	GPIO_DRIVE_20MA = GPIO_P_CTRL_DRIVEMODE_HIGH,
	/** 2 mA */
	GPIO_DRIVE_2MA = GPIO_P_CTRL_DRIVEMODE_LOW
} gpio_drive_strength;

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_T(port,pin) ((uint16_t)((port<<8) | pin))

/**
 * @brief   Available ports on the EFM32 family
 */
typedef enum {
	GPIO_PORT_A = 0, /**< Port A */
	GPIO_PORT_B = 1, /**< Port B */
	GPIO_PORT_C = 2, /**< Port C */
	GPIO_PORT_D = 3, /**< Port D */
	GPIO_PORT_E = 4, /**< Port E */
	GPIO_PORT_F = 5 /**< Port F */
} gpio_port;

#define HAVE_GPIO_DIR_T
typedef enum {
	/** Input disabled. Pullup if DOUT is set. */
	GPIO_DIR_DISABLED = _GPIO_P_MODEL_MODE0_DISABLED,
	/** Input enabled. Filter if DOUT is set */
	GPIO_DIR_INPUT = _GPIO_P_MODEL_MODE0_INPUT,
	/** Input enabled. DOUT determines pull direction */
	GPIO_DIR_INPUT_PULL = _GPIO_P_MODEL_MODE0_INPUTPULL,
	/** Input enabled with filter. DOUT determines pull direction */
	GPIO_DIR_INPUT_PULL_FILTER = _GPIO_P_MODEL_MODE0_INPUTPULLFILTER,
	/** Push-pull output */
	GPIO_DIR_PUSH_PULL = _GPIO_P_MODEL_MODE0_PUSHPULL,
	/** Push-pull output with drive-strength set by DRIVEMODE */
	GPIO_DIR_PUSH_PULL_DRIVE = _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE,
	/** Wired-or output */
	GPIO_DIR_WIRED_OR = _GPIO_P_MODEL_MODE0_WIREDOR,
	/** Wired-or output with pull-down */
	GPIO_DIR_WIRED_OR_PULL_DOWN = _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN,
	/** Open-drain output */
	GPIO_DIR_WIRED_AND = _GPIO_P_MODEL_MODE0_WIREDAND,
	/** Open-drain output with filter */
	GPIO_DIR_WIRED_AND_FILTER = _GPIO_P_MODEL_MODE0_WIREDANDFILTER,
	/** Open-drain output with pullup */
	GPIO_DIR_WIRED_AND_PULLUP = _GPIO_P_MODEL_MODE0_WIREDANDPULLUP,
	/** Open-drain output with filter and pullup */
	GPIO_DIR_WIRED_AND_PULLUP_FILTER = _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER,
	/** Open-drain output with drive-strength set by DRIVEMODE */
	GPIO_DIR_WIRED_AND_DRIVE = _GPIO_P_MODEL_MODE0_WIREDANDDRIVE,
	/** Open-drain output with filter and drive-strength set by DRIVEMODE */
	GPIO_DIR_WIRED_AND_DRIVE_FILTER = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEFILTER,
	/** Open-drain output with pullup and drive-strength set by DRIVEMODE */
	GPIO_DIR_WIRED_AND_DRIVE_PULLUP = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUP,
	/** Open-drain output with filter, pullup and drive-strength set by DRIVEMODE */
	GPIO_DIR_WIRED_AND_DRIVE_PULLUP_FILTER = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUPFILTER,
	/** For back compatibility */
	GPIO_DIR_IN = _GPIO_P_MODEL_MODE0_INPUT,
	/** For back compatibility */
	GPIO_DIR_OUT = _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE
} gpio_dir_t;

/**
 * @brief   Just formality, EFM does not need to set alternate functions
 * 			in GPIO registers.
 */
typedef enum {
	NOP = 0,
} gpio_af_t;

void gpio_drive(gpio_port port, gpio_drive_strength mode);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
