#ifndef __BOARD_H
#define __BOARD_H

/**
 * @file    boards.h
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @defgroup    mbed_lpc1768 mbed NXP LPC1768 development kit
 * @ingroup     boards
 * @brief       Support for the mbed NXP LPC1768 board.
 * @{
 */

#include <stdint.h>
#include "bitarithm.h"

#define F_CPU                   (96000000)


#define PIN_LED1 (BIT18)
#define PIN_LED2 (BIT20)
#define PIN_LED3 (BIT21)
#define PIN_LED4 (BIT23)

#define LED_ON(led_nr)      (LPC_GPIO1->FIOSET = PIN_LED##led_nr)
#define LED_OFF(led_nr)     (LPC_GPIO1->FIOCLR = PIN_LED##led_nr)
#define LED_TOGGLE(led_nr)  (LPC_GPIO1->FIOPIN ^= PIN_LED##led_nr)

typedef uint8_t radio_packet_length_t;

/**
 * @brief Busy waiting function (while hwtimer is not available at boot time)
 *
 * @param[in] t The waiting cycles
 */
void loop_delay(uint32_t t);

/** @} */
#endif /* __BOARD_H */
