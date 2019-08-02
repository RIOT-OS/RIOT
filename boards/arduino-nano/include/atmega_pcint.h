#ifndef ATMEGA_PCINT_H
#define ATMEGA_PCINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* PB5 is connected to an LED on the Arduino Nano */
#define ATMEGA_PCINT_MAP_PCINT0 GPIO_PIN(PORT_B, 0), GPIO_PIN(PORT_B, 1), GPIO_PIN(PORT_B, 2), GPIO_PIN(PORT_B, 3), GPIO_PIN(PORT_B, 4), GPIO_UNDEF, GPIO_PIN(PORT_B, 6), GPIO_PIN(PORT_B, 7)
/* PC6 is not available on the Arduino Nano */
#define ATMEGA_PCINT_MAP_PCINT1 GPIO_PIN(PORT_C, 0), GPIO_PIN(PORT_C, 1), GPIO_PIN(PORT_C, 2), GPIO_PIN(PORT_C, 3), GPIO_PIN(PORT_C, 4), GPIO_PIN(PORT_C, 5), GPIO_UNDEF, GPIO_UNDEF
/* Pin PD2 is both INT0 and PCINT18, RIOT is using it as INT0 */
/* Pin PD3 is both INT1 and PCINT19, RIOT is using it as INT1 */
/* Pins PD6 and PD7 are not available on the Arduino Nano */
#define ATMEGA_PCINT_MAP_PCINT2 GPIO_PIN(PORT_D, 0), GPIO_PIN(PORT_D, 1), GPIO_UNDEF, GPIO_UNDEF, GPIO_PIN(PORT_D, 4), GPIO_PIN(PORT_D, 5), GPIO_UNDEF, GPIO_UNDEF

#ifdef __cplusplus
}
#endif

#endif /* ATMEGA_PCINT_H */
