/**
 * @ingroup     boards_rf_ism_mod_2_1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the rf_ism_mod_2_1 board
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include "./include/board.h"
#include "../../cpu/efm32wg/emlib/inc/em_cmu.h"
#include "../../cpu/efm32wg/emlib/inc/em_gpio.h"
#include "em_cmu.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs.
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 */
static void leds_init(void)
{
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
//	GPIO_PinModeSet(LED_RED_PORT, LED_RED_PIN, gpioModePushPull, 0);
//	GPIO_PinModeSet(LED_GREEN_PORT, LED_GREEN_PIN, gpioModePushPull, 0);
//	GPIO_PinModeSet(LED_YELLOW_PORT, LED_YELLOW_PIN, gpioModePushPull, 0);
//	LED_RED_ON;
//	LED_GREEN_ON;
//	LED_YELLOW_ON;
}
