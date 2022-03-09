/**
 * @defgroup    boards_stm32f429i-disco STM32F429I-DISCO
 * @ingroup     boards
 * @brief       Support for the STM32F429I-DISCO board
 */

### General information

The board is identical to the @ref boards_stm32f429i-disc1 except that it uses
an older on-board ST-LINK debugger that does not provide a serial UART interface.

To mitigate that, stdio is instead provided through the micro-USB connector using
RIOT's CDC ACM functionality.
