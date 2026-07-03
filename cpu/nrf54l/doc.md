@defgroup        cpu_nrf54l Nordic nRF54L MCU
@ingroup         cpu
@brief           Nordic nRF54L family of CPUs

## Overview

The nRF54L family is Nordic Semiconductor's ultra-low-power wireless MCU
family, featuring an ARM Cortex-M33 application core running at 128 MHz, a
RISC-V coprocessor (FLPR), RRAM based non-volatile memory (instead of
classical flash) and a 2.4 GHz multiprotocol radio.

Currently, only the application core is supported by RIOT, with the following
peripherals available:

| Peripheral          | Support status                                  |
|:------------------- |:----------------------------------------------- |
| GPIO                | supported (P0, P1, P2)                          |
| GPIO IRQ            | supported (GPIOTE20 for P1, GPIOTE30 for P0)    |
| TIMER               | supported (TIMER2x instances)                   |
| UART                | supported (UARTE2x/UARTE30 instances)           |
| SPI                 | supported (SPIM00, SPIM2x, SPIM30 instances)    |
| I2C                 | supported (TWIM2x, TWIM30 instances)            |
| PWM                 | supported (PWM2x instances)                     |
| RTT (GRTC)          | supported (1 MHz, lower 32 bit of SYSCOUNTER)   |
| Radio               | supported (802.15.4 nrf802154, BLE adv nrfble)  |
| ADC (SAADC)         | not yet supported                               |
| Flash (RRAMC)       | not yet supported (no periph_flashpage)         |
| WDT                 | not yet supported                               |
| RNG/crypto (CRACEN) | not yet supported                               |
| Temperature         | not yet supported                               |
| QDEC                | not yet supported                               |
| NFC (NFCT)          | not yet supported                               |
| I2S                 | not yet supported                               |
| PDM                 | not yet supported                               |

Non-blocking UART is not supported because the UARTE lacks the TXSTARTED
event.

Note that the family provides no classical RTC peripheral (low-power timing
is covered by the GRTC based RTT) and no USB device peripheral.
