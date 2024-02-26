# List of Features (Features as Build System Enties)
<!--
WARNING: This has been auto-generated from features.yaml.
         Do not edit this by hand, but update features.yaml instead.
         Finally, run `make generate-features` in the root of the RIOT repo.
-->

[TOC]



# Architecture Features
These indicate architecture features, such as word size of the CPU, supported instruction sets and so on. All architecture features provided by a given board will always be used, e.g. an 8-bit CPU cannot just stop being an 8-bit CPU on request.

## Word size
Word size of the CPU

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `arch_8bit`                       | CPU has a 8-bits architecture                                                 |
| `arch_16bit`                      | CPU has a 16-bits architecture                                                |
| `arch_32bit`                      | CPU has a 32-bits architecture                                                |
| `arch_64bit`                      | CPU has a 64-bits architecture                                                |

## Architecture grouping
Instruction set of the CPU

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `arch_arm`                        | CPU architecture is classic ARM or Cortex M                                   |
| `arch_arm7`                       | CPU architecture is classic ARM (ARM7)                                        |
| `arch_avr8`                       | CPU architecture is AVR-8                                                     |
| `arch_efm32`                      | FIXME. This is not an architecture. Use cpu_efm32 for this                    |
| `arch_esp`                        | CPU architecture is an ESP. (Fixme: This is not an architecture)              |
| `arch_esp_xtensa`                 | CPU architecture is Xtensa                                                    |
| `arch_esp_riscv`                  | CPU architecture is RISC-V (ESP flavor)                                       |
| `arch_esp32`                      | CPU architecture is an ESP32. (Fixme: This is not an architecture)            |
| `arch_esp32_xtensa`               | CPU architecture is Xtensa (ESP32 flavor)                                     |
| `arch_esp8266`                    | CPU architecture is Xtensa (ESP8266 flavor)                                   |
| `arch_msp430`                     | CPU architecture is MSP430                                                    |
| `arch_native`                     | CPU architecture is `native`                                                  |
| `arch_riscv`                      | CPU architecture is RISC-V                                                    |
| `arch_nuclei`                     | The CPU is based on a Nuclei System Technology design. Hence, RIOT is using the Nuclei Microcontroller Software Interface Standard (NMSIS) vendor independent hardware abstraction layer. |

# CPU Features
These features are related to CPU capabilities or just used to indicated which CPU family is used.

## CPU Capabilities
These correspond to features/capabilities provided by certain CPUs

### Cortex M Specific Features
These features are only available on (some) ARM Cortex M MCUs

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_check_address`               | The @ref cpu_check_address can be used to check if accessing a given address would cause a bus fault. |
| `cortexm_svc`                     | ARM Cortex-M Supervisor Calls are available.                                  |
| `cortexm_fpu`                     | A hardware floating point unit is available.                                  |
| `cortexm_mpu`                     | A memory protection unit (MPU) is available.                                  |

#### nRF Capabilities
These features are only available on (some) nordic nRF MCUs

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `vdd_lc_filter_reg1`              | An LC filter for use with the internal DC/DC converter is present. If this is the case, the DC/DC converter is used over the LDO regulator for improved power efficiency. |
| `vdd_lc_filter_reg0`              | The MCU supports a high voltage supply via an two-stage regulator and the board has an LC filter required for using the two-stage DC/DC converter. This enables the two-stage DC/DC converter. |

### AVR-8 Specific Features
#### ATmega Specific Features
These features are only available on (some) ATmega MCUs.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `atmega_pcint0`                   | Required pin-mapping for pin change interrupt on bank 0 is available. See section on pin change interrupts in @ref cpu_atmega_common |
| `atmega_pcint1`                   | Required pin-mapping for pin change interrupt on bank 1 is available. See section on pin change interrupts in @ref cpu_atmega_common |
| `atmega_pcint2`                   | Required pin-mapping for pin change interrupt on bank 2 is available. See section on pin change interrupts in @ref cpu_atmega_common |
| `atmega_pcint3`                   | Required pin-mapping for pin change interrupt on bank 3 is available. See section on pin change interrupts in @ref cpu_atmega_common |

#### ATxmega Specific Features
These features are only available on (some) ATxmega MCUs.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `atxmega_ebi`                     | MCU supports the external bus interface (EBI) to either extend RAM with external RAM or attach memory mapped peripherals such as (some) displays. |

### EFM32 Specific Features
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `efm32_coretemp`                  | Y R U not using `periph_temperature` for this?                                |
| `gecko_sdk_librail_nonfpu`        | The CPU offers librail support if the FPU is disabled. Librail is shipped as pre-compiled blobs. Thus, we have to adapt to their choice how to process floats. |
| `gecko_sdk_librail_fpu`           | The CPU offers librail support if the FPU is enabled. Librail is shipped as pre-compiled blobs. Thus, we have to adapt to their choice how to process floats. |

### ESP Specific Features
These features are only available on (some) ESP MCUs.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `esp_eth`                         | An ESP Ethernet peripherals is available. (FIXME: `periph_eth` instead.)      |
| `esp_jtag`                        | The MCU supports JTAG for programming and debugging. Enable this feature to expose the interface at the cost of having fewer pins as GPIOs available. |
| `esp_now`                         | An ESP NOW-compatible radio is present.                                       |
| `esp_spiffs`                      | A Serial Peripheral Interface Flash File System can be used.                  |
| `esp_wifi`                        | An ESP WiFi radio is present.                                                 |
| `esp_wifi_ap`                     | ESP WiFi SoftAP support is present.                                           |
| `esp_wifi_enterprise`             | The ESP WiFi interface supports WPA2 enterprise mode.                         |
| `esp_ble_esp32`                   | The ESP32x SoC uses the SDK Bluetooth LE library for the ESP32 variant.       |
| `esp_ble_esp32c3`                 | The ESP32x SoC uses the SDK Bluetooth LE library for the ESP32-C3 or ESP32-S3 variant. |
| `esp_hw_counter`                  | The used ESP32x SoC supports HW counters that can be used as timers.          |
| `esp_rmt`                         | The ESP32x SoC has an RMT (Remote Control Transceiver) peripheral.            |
| `esp_rtc_timer_32k`               | An external 32.768 kHz crystal is connected to the ESP32x Soc on the board.   |
| `esp_spi_ram`                     | An external RAM is connected via the SPI interface to the ESP32x SoC on the board. |
| `esp_spi_oct`                     | Octal SPI mode is used for Flash and SPI RAM. In this case additional GPIOs are used for the SPI interface and cannot be used for other purposes. |
| `esp_ble`                         | An ESP32 Bluetooth LE transceiver is present.                                 |

### nordic nRF Specific Features
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `radio_nrf802154`                 | An nRF MCU with a peripheral radio that supports IEEE 802.15.4 is present.    |
| `radio_nrfble`                    | An nRF MCU with a peripheral radio that supports Bluetooth LE is present.     |
| `radio_nrfmin`                    | An nRF MCU with a peripheral radio that supports nordics proprietary link layer protocol is present. |

## CPU Grouping
These are not actually features/capabilities, but just indicate to which CPU family a certain CPU belongs

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_native`                      | The board is simulated by a native program running on the host                |

### Atmel / Microchip AVR-8 Grouping
#### ATmega Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_core_atmega`                 | The MCU has an ATmega CPU                                                     |
| `cpu_atmega8`                     | The MCU is an ATmega8                                                         |
| `cpu_atmega32u4`                  | The MCU is an ATmega32U4                                                      |
| `cpu_atmega128rfa1`               | The MCU is an ATmega128RFA1                                                   |
| `cpu_atmega256rfr2`               | The MCU is an ATmega256RFR2                                                   |
| `cpu_atmega328p`                  | The MCU is an ATmega328P                                                      |
| `cpu_atmega1281`                  | The MCU is an ATmega1281                                                      |
| `cpu_atmega1284p`                 | The MCU is an ATmega1284P                                                     |
| `cpu_atmega2560`                  | The MCU is an ATmega2560                                                      |

#### ATxmega Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_core_atxmega`                | The MCU has an ATxmega CPU                                                    |
| `cpu_atxmega`                     | The MCU is an ATxmega XYZ CPU                                                 |

### ARM Cortex-M and Classic ARM Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_core_cortexm`                | The MCU has an ARM Cortex-M CPU (any family)                                  |
| `cpu_stm32`                       | The MCU has an STM32 MCU                                                      |

#### Atmel / Microchip SAM Grouping
##### SAM0 Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_samd21`                      | The MCU has an Atmel/Microchip SAM D10/D11/D20/D21 CPU                        |
| `cpu_samd5x`                      | The MCU has an Atmel/Microchip SAM D5x CPU                                    |
| `cpu_saml1x`                      | The MCU has an Atmel/Microchip SAM L1x CPU                                    |
| `cpu_saml21`                      | The MCU has an Atmel/Microchip SAM L2x / L3x CPU                              |

##### SAM3 Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_sam3`                        | The MCU has an Atmel/Microchip SAM 3 CPU                                      |

#### GigaDevice Semiconductor Inc Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_gd32v`                       | The MCU is part of the GigaDevice GD32V family                                |

#### nordic nRF Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_nrf51`                       | The MCU has an nordic nRF51 CPU                                               |
| `cpu_nrf52`                       | The MCU has an nordic nRF52 CPU                                               |
| `cpu_nrf53`                       | The MCU has an nordic nRF53 CPU                                               |
| `cpu_nrf9160`                     | The MCU has an nordic nRF9160 CPU                                             |

#### NXP Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_kinetis`                     | The MCU is part of the NXP Kinetis family                                     |
| `cpu_lpc1768`                     | The MCU is an NXP LPC1768                                                     |
| `cpu_lpc23xx`                     | The MCU is part of the NXP LPC23xx classic ARM family                         |
| `cpu_qn908x`                      | The MCU is part of the NXP QN908x family                                      |

#### Raspberry Pi Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_rpx0xx`                      | The MCU is part of the Raspberry PI RPx0xx family.                            |

#### Silicon Laboratories EFM32 Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_efm32`                       | The MCU is part of the Silicon Labs EFM32 family                              |

#### SiFive, Inc. Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_fe310`                       | The MCU is in SiFive Freedom E310                                             |

#### STMicroelectronics STM32 Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_stm32c0`                     | The MCU has an STM32 C0 MCU                                                   |
| `cpu_stm32f0`                     | The MCU has an STM32 F0 MCU                                                   |
| `cpu_stm32f1`                     | The MCU has an STM32 F1 MCU                                                   |
| `cpu_stm32f2`                     | The MCU has an STM32 F2 MCU                                                   |
| `cpu_stm32f3`                     | The MCU has an STM32 F3 MCU                                                   |
| `cpu_stm32f4`                     | The MCU has an STM32 F4 MCU                                                   |
| `cpu_stm32f7`                     | The MCU has an STM32 F7 MCU                                                   |
| `cpu_stm32g0`                     | The MCU has an STM32 G0 MCU                                                   |
| `cpu_stm32g4`                     | The MCU has an STM32 G4 MCU                                                   |
| `cpu_stm32l0`                     | The MCU has an STM32 L0 MCU                                                   |
| `cpu_stm32l1`                     | The MCU has an STM32 L1 MCU                                                   |
| `cpu_stm32l4`                     | The MCU has an STM32 L4 MCU                                                   |
| `cpu_stm32l5`                     | The MCU has an STM32 L5 MCU                                                   |
| `cpu_stm32mp1`                    | The MCU has an STM32 MP1 MCU                                                  |
| `cpu_stm32u5`                     | The MCU has an STM32 U5 MCU                                                   |
| `cpu_stm32wb`                     | The MCU has an STM32 WB MCU                                                   |
| `cpu_stm32wl`                     | The MCU has an STM32 WL MCU                                                   |

#### Texas Instruments ARM MCU Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_cc26x2_cc13x2`               | The CPU in an TI CC26x2 or an TI CC13x2                                       |
| `cpu_cc26x0_cc13x0`               | The CPU in an TI CC26x0 or an TI CC13x0                                       |
| `cpu_cc2538`                      | The CPU in an TI CC2538                                                       |
| `cpu_lm4f120`                     | The CPU is an TI LM4F120                                                      |

### Expressif ESP Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_esp32`                       | The MCU is part of the ESP32 family                                           |
| `cpu_esp8266`                     | The MCU is an ESP8266                                                         |

### Texas Instruments MSP430 MCU Grouping
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpu_msp430`                      | The MCU is member of the MSP430 family.                                       |
| `cpu_msp430_x1xx`                 | The MCU is member of the MSP430 x1xx family.                                  |
| `cpu_msp430_f2xx_g2xx`            | The MCU is member of the MSP430 F2xx/G2xx family.                             |

# Arduino Features
These features indicate that an Arduino style I/O mapping is available, in which common descriptors refer to a GPIO pin / I²C bus / SPI bus / PWM output / ADC input / etc. at a well-known location on a well-known board form factor.

## Arduino I/O Mapping Features
These features indicate presence of I/O mappings (e.g. well-known preprocessor macro names such as `ARDUINO_PIN_0` for a GPIO pin that is routed to pin `D0` on the board.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `arduino_analog`                  | Indicates that Arduino analog pins mappings are provided.                     |
| `arduino_dac`                     | Indicates that Arduino DAC pins mappings are provided.                        |
| `arduino_i2c`                     | Indicates that Arduino I²C bus mappings are provided.                         |
| `arduino_pins`                    | Indicates that Arduino digital pins mappings are provided.                    |
| `arduino_pwm`                     | Indicates that Arduino digital pin to PWM mappings are provided.              |
| `arduino_spi`                     | Indicates that Arduino SPI bus mappings are provided.                         |
| `arduino_uart`                    | Indicates that Arduino UART device mappings are provided.                     |

## Arduino Form Factor Features
These features indicate compatibility with a specific form factor, e.g. Arduino UNO or Adafruit Feather

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `arduino_shield_isp`              | Indicates that the board is mechanically and electrically compatible with shields that mate with the ISP header for SPI connectivity. |
| `arduino_shield_mega`             | Indicates that the board is mechanically and electrically compatible with shields developed for the Arduino Mega 2560. (Note: Except the ISP header, that requires `arduino_shield_isp` in addition.) |
| `arduino_shield_nano`             | Indicates that the board is mechanically and electrically compatible with shields developed for the Arduino Nano. (Note: Except the ISP header, that requires `arduino_shield_isp` in addition.) |
| `arduino_shield_uno`              | Indicates that the board is mechanically and electrically compatible with shields developed for the Arduino Nano. (Note: Except the ISP header, that requires `arduino_shield_isp` in addition.) |

# RAM Related Features
These features indicate presence of special RAM regions or features

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `backup_ram`                      | A special portion of RAM is retained during deep sleep. Variables can be placed there by annotating them with the `BACKUP_RAM` attribute. |
| `periph_rtc_mem`                  | The RTC peripheral provides storage memory for deep sleep. Unlike `backup_ram`, this memory is not mapped in the address space and requires calls to @ref rtc_mem_read and @ref rtc_mem_write to access. |
| `puf_sram`                        | The @ref sys_puf_sram module can be used to harvest entropy from uninitialized SRAM on cold boot to seed PRNGs. |

# Bluetooth Low Energy Features
The capabilities of the integrated (peripheral) BLE transceiver are modules using these features.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `ble_adv_ext`                     | Support for Bluetooth LE 5 Advertising Extension                              |
| `ble_nimble`                      | Support for the NimBLE stack                                                  |
| `ble_nimble_netif`                | NimBLE supports the netif API, so that network stacks such as GNRC can be used on top. See @ref pkg_nimble_netif for details. |
| `ble_phy_2mbit`                   | The BLE radio supports the 2Mbit PHY mode                                     |
| `ble_phy_coded`                   | The BLE radio supports the CODED PHY mode                                     |

# Toolchain Features
These features are used to indicate which toolchains are supported, which languages they support, which libraries (e.g. picolibc, newlibc, libstdc++, etc.) are supported. Unless using `BUILD_IN_DOCKER=1`, those toolchains/libraries need to be installed on the system for the given platform to actually be usable, though.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `cpp`                             | The C++ programming language is supported. Note that libstdc++ support is not implied and indicated by a separate feature. |
| `libstdcpp`                       | When using C++, a libstdc++ is available.                                     |
| `picolibc`                        | The picolibc C library is available for the platform.                         |
| `newlib`                          | The newlib C library is available for the platform.                           |
| `rust_target`                     | The Rust target definition ("triple") is known. This is a mandatory requirement to build Rust code. |
| `emulator_renode`                 | The platform is compatible with the Renode emulator.                          |

# Peripheral Features
These features indicate presence of peripheral IP block, presence of a corresponding driver in RIOT, and any required board specific configuration needed.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_cpuid`                    | The CPU has identification information available. In the best case this is a globally unique ID, in the worst case it is some calibration parameters to compensate production variance that may or may not allow telling the CPU apart from any given other. |
| `periph_pwm`                      | A Pulse-Width Modulation (PWM) peripheral is present.                         |
| `periph_qdec`                     | A Quadrature Decoder (QDEC) peripheral is present.                            |
| `periph_temperature`              | The MCU has a built-in temperature sensor.                                    |
| `periph_vbat`                     | Backup battery monitoring is supported                                        |
| `periph_freqm`                    | A Frequency Meter peripheral is present.                                      |
| `periph_dma`                      | A DMA peripheral is present. Enabling this feature affects the implementation of other peripheral drivers. E.g. SPI/I²C/UART/... transfers may use the DMA (possible only when transfers are longer than a certain cut-off). The main benefit is that other threads can run while the thread issuing the, say SPI transfer, is blocked until the transfer is completed. It often also speeds up longer transfers. |
| `periph_pm`                       | The MCU supports power management (PM) and RIOT can make use of that.         |
| `periph_ltdc`                     | An LCD/TFT Display Controller (LTDC) peripheral is present. (Currently only provided by some STM32 MCUs.) |
| `periph_pio`                      | A Programmable IO (PIO) is present. (Currently only RP2040)                   |

## General-Purpose Input/Output (GPIO)
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_gpio_fast_read`           | This feature is currently available on Microchip SAM0 based MCUs only. Enabling this feature reduces read latency for an increase in power consumption. It affects both the classic GPIO API driver and the GPIO LL driver. |

### Pin Level Peripheral GPIO API
This is a pin-level API that caters most use cases. Most code should be using this API over GPIO LL.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_gpio`                     | The classic GPIO API is implemented                                           |
| `periph_gpio_irq`                 | The classic GPIO driver supports external interrupts.                         |
| `periph_gpio_tamper_wake`         | This features is currently available on Microchip SAMD5x MCUs only. Enabling this features allows GPIO IRQs to wake the CPU even in deep sleep. The SAMD5x is not the only MCU that can be woken from lower power modes via GPIO, but it is currently the only MCU were this feature is configurable. |

### GPIO LL API
This is a lower level GPIO API that allows port based access. It exposes a number of advanced features and lower latency GPIO access at the cost of a more complex and more frequently changing API. You should only use this if the Pin Level Peripheral GPIO API is not catering your use case well enough.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_gpio_ll`                  | The GPIO LL driver is implemented for the MCU's GPIO peripheral.              |
| `periph_gpio_ll_irq`              | The GPIO LL driver has IRQ support.                                           |
| `periph_gpio_ll_irq_level_triggered_high` | Level triggered IRQs are supported for level high.                            |
| `periph_gpio_ll_irq_level_triggered_low` | Level triggered IRQs are supported for level low.                             |
| `periph_gpio_ll_irq_unmask`       | The GPIO LL driver supports unmasking interrupts without clearing pending IRQs that came in while masked. |
| `periph_gpio_ll_disconnect`       | Some or all GPIO pins can be electrically disconnected from the MCU (high impedance state) with the GPIO LL API. |
| `periph_gpio_ll_input_pull_down`  | Some or all GPIO pins can enable an internal pull down resistor when the GPIO is configured in input mode. |
| `periph_gpio_ll_input_pull_keep`  | Some or all GPIO pins can enable internal pull resistors that pull towards the current bus level (pull down when bus is low, pull up when bus is high). |
| `periph_gpio_ll_input_pull_up`    | Some or all GPIO pins can enable an internal pull up resistor when the GPIO is configured in input mode. |
| `periph_gpio_ll_open_drain`       | Some or all pins can be configured in open drain mode.                        |
| `periph_gpio_ll_open_drain_pull_up` | Some or all GPIO pins can enable an internal pull up resistor when the GPIO is configured in open drain mode. |
| `periph_gpio_ll_open_source`      | Some or all pins can be configured in open source mode.                       |
| `periph_gpio_ll_open_source_pull_down` | Some or all GPIO pins can enable an internal pull down resistor when the GPIO is configured in open source mode. |
| `periph_gpio_ll_switch_dir`       | The GPIO LL driver allows switching the direction between input and (push-pull) output in an efficient manner. The main use case is bit-banging bidirectional protocols when open-drain / open-source mode is not supported. GPIO LL drivers for peripherals that do support open drain mode typically do not bother implementing this, even if the hardware would allow it. |

## Serial Interfaces
Features related to serial interfaces

### UART Features
Features related to the Universal Asynchronous Receiver-Transmitter peripheral

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_uart`                     | An UART peripheral is present.                                                |
| `periph_lpuart`                   | A low-power UART peripheral is present.                                       |
| `periph_uart_collision`           | The UART peripheral supports hardware collision detection.                    |
| `periph_uart_hw_fc`               | The UART peripheral supports hardware flow control.                           |
| `periph_uart_modecfg`             | The UART peripheral allows configuration to non-default modes.                |
| `periph_uart_tx_ondemand`         | Indicates that the UART peripheral can enable / disable the TX line using @ref uart_enable_tx / @ref uart_disable_tx |
| `periph_uart_nonblocking`         | The UART peripheral allows non-blocking operations.                           |
| `periph_uart_reconfigure`         | The UART pins can be made available as regular GPIOS using @ref uart_deinit_pins and be attached back to the UART peripheral using @ref uart_init_pins |
| `periph_uart_rxstart_irq`         | The UART can issue an interrupt when the start condition detected. Use @ref uart_rxstart_irq_configure to associate a callback with the ISR of a given UART peripheral. |

### SPI Features
Features related to the Serial Peripheral Interface peripheral

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_spi`                      | An SPI peripheral is present.                                                 |
| `periph_spi_on_qspi`              | The QSPI peripheral can be used in SPI mode.                                  |
| `periph_spi_reconfigure`          | The SPI pins can be made available as regular GPIOs using @ref spi_deinit_pins and re-attached to the SPI peripheral using @ref spi_init_pins |
| `periph_spi_gpio_mode`            | The SPI peripheral supports specifying the GPIO mode of each SPI pin upon initialization of the peripheral. |

### I²C Features
Features related to the Inter-Integrated Circuit peripheral

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_i2c`                      | An I²C peripheral is present.                                                 |
| `pio_i2c`                         | An I²C bus can be provided via the PIO peripheral.                            |
| `periph_i2c_reconfigure`          | The I²C pins can be made available as regular GPIOs using @ref i2c_deinit_pins and re-attached to the I²C peripheral using @ref i2c_init_pins |

### USB Features
Features related to the Universal Serial Bus

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_usbdev`                   | An USB peripheral is present.                                                 |
| `periph_usbdev_hs`                | The USB peripheral supports High-Speed.                                       |
| `periph_usbdev_hs_utmi`           | An USB high-speed peripheral with internal UTMI+ HS PHY is present.           |
| `periph_usbdev_hs_ulpi`           | An USB high-speed peripheral with ULPI HS PHY is present.                     |

## Analog Features
Features related to ADCs/DACs

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_adc`                      | An ADC peripheral is present.                                                 |
| `periph_adc_continuous`           | The ADC peripheral can be left on between measurements.                       |
| `periph_dac`                      | A DAC peripheral is present.                                                  |

## Integrated Connectivity
Peripheral network and communication interfaces.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_can`                      | A CAN peripheral is present.                                                  |
| `periph_eth`                      | An Ethernet peripheral is present.                                            |

## SD / MMC Card Features
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_sdmmc`                    | An SDIO/SD/MMC peripheral is present and used by the board. This feature shall be provided by the board configuration, if available. |
| `periph_sdmmc_8bit`               | The SDIO/SD/MMC peripheral supports the 8-bit bus width and at least one component of the board is connected with 8 data lines. This feature shall be provided by the board configuration, if available. |
| `periph_sdmmc_auto_clk`           | The SDIO/SD/MMC peripheral supports the Auto-CLK feature, i.e. the automatic activation and deactivation of the SD CLK signal when required. This feature shall be provided by the MCU if supported. |
| `periph_sdmmc_auto_cmd12`         | The SDIO/SD/MMC peripheral supports the Auto-CMD12 feature, i.e. CMD12 is sent automatically to stop the transmission in multiple block operations. This feature shall be provided by the MCU if supported. |
| `periph_sdmmc_clk`                | The SDIO/SD/MMC peripheral has special clock functionality used by the peripheral driver. |
| `periph_sdmmc_hs`                 | The SDIO/SD/MMC peripheral supports the high speed access, that is 50 MHz for SD and 52 MHz for MMC. This feature shall be provided by the MCU. |
| `periph_sdmmc_mmc`                | The SDIO/SD/MMC peripheral supports MMC/eMMCs. This feature shall be provided by the MCU. |
| `periph_sdmmc_sdhc`               | The SDIO/SD/MMC peripheral is compliant with the SD Host Controller Specification. This feature shall be provided by the MCU. |

## Flash Features
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_flashpage`                | A Flashpage peripheral is present.                                            |
| `periph_flashpage_in_address_space` | Static memory sections can fundamentally be turned into flash pages.          |
| `periph_flashpage_pagewise`       | The Flashpage peripheral supports pagewise writing.                           |
| `periph_flashpage_rwee`           | The Flashpage peripheral is of the Read While Write.                          |

## Other Peripheral Storage Features
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_eeprom`                   | An EEPROM peripheral is present.                                              |
| `periph_fmc`                      | An Flexible Memory Controller (FMC) or an Flexible Static Memory Controller (FSMC) is present. It can be used to extend memory or drive display controllers. |
| `periph_fmc_16bit`                | The FMC/FSMC peripheral supports a 16-bit data bus                            |
| `periph_fmc_32bit`                | The FMC/FSMC peripheral supports a 32-bit data bus                            |
| `periph_fmc_sdram`                | A board configuration is provided to attach map the board's SDRAM into the address space using the FMC/FSMC. |
| `periph_fmc_nor_sram`             | A board configuration is provided to attach map the board's NOR flash or (P)SRAM into the address space using the FMC/FSMC. |
| `periph_nvm`                      | A non-volatile memory peripheral is present. This is currently only provided by ATxmega MCUs to read the production signature which then is used as CPU ID. |

## Timer Features
Features related to timers

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_rtc`                      | An Real Time Clock (RTC) peripheral is present. This timer works with time broken down into year, month, day, hour, minute, second. |
| `periph_rtc_ms`                   | The RTC peripheral can provide sub-second timestamps.                         |
| `periph_rtt`                      | An Real Time Timer (RTT) (a.k.a. Real Time Counter) peripheral is present. This timer is similar to `periph_timer`, except for using a different API (for no reason), being low power, slower ticking, and typically less affected by clock drift. |
| `periph_rtt_set_counter`          | The RTT peripheral implements @ref rtt_set_counter                            |
| `periph_rtt_overflow`             | The RTT provides an overflow callback.                                        |
| `periph_wdt`                      | A Watchdog Timer (WDT) peripheral is present.                                 |
| `periph_wdt_cb`                   | The WDT peripheral allows setting a callback function to be called before the reboot is actually triggered. |
| `periph_wdt_warning_period`       | By setting @ref CONFIG_WDT_WARNING_PERIOD the time when the WDT callback is triggered can be set. It specifies how many milliseconds before the reboot the callback should be executed. |

### High Frequency Timers
Features related to high frequency timers, a.k.a. `periph_timer`

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_timer`                    | A high frequency timer peripheral is present.                                 |
| `periph_timer_periodic`           | The Timer peripheral allows setting periodic timeouts in addition to the mandatory one-shot timeouts. |
| `periph_timer_poll`               | The Timer peripheral allows polling if a one-shot timer channel has already been expired using @ref timer_poll_channel to allow high accuracy busy waiting. |
| `periph_timer_query_freqs`        | The timer driver supports querying supported frequencies.                     |

### PTP Timers
These features are related to the hardware requirements to implement the Precision Time Protocol.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_ptp`                      | A PTP clock is present. The clock must have nanoseconds as unit and be at least 64 bit wide. |
| `periph_ptp_speed_adjustment`     | The PTP clock speed can be adjusted. This can be used for clock drift correction and synchronization without "jumping" to the new network time, but rather pace faster/slow for some time until the clocks are back in sync. |
| `periph_ptp_timer`                | The PTP clock can be used as timer (so that (absolute) timeouts can be set on the clock). |
| `periph_ptp_txrx_timestamps`      | The PTP clock can provide exact time stamps of the reception and transmission of frames (typically received at the peripheral Ethernet interface). |

## Platform Specific
Things specific to a single MCU family / MCU vendor

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_ics`                      | An NXP Kinetis Internal Clock Source Controller (ICS peripheral) is present.  |
| `periph_mcg`                      | An Kinetis Multipurpose Clock Generator (MCG peripheral) is present.          |
| `periph_mcg_lite`                 | An Kinetis Multipurpose Clock Generator (MCG peripheral) is present in the lite version. |
| `periph_coretimer`                | A RISC-V CLINT timer is available and usable via the High Speed timer API. The CLINT only supports running at the RTC clock, typically 32.678 kHz, though. |
| `periph_pmp`                      | A RISC-V physical memory protection (PMP) peripheral is present. (Similar to ARM's MPU) |
| `periph_clic`                     | A RISC-V Core-local Interrupt Controller (CLIC) peripheral is present.        |
| `periph_plic`                     | A RISC-V Platform-local Interrupt Controller (PLIC) peripheral is present.    |

## Cryptographic Features
Hardware acceleration for cryptographic primitives, hardware random number generators, and other features useful for cryptography.
Please keep in mind that some of the cryptographic primitives provided by the hardware have not aged well in terms of security. They may still be nifty for use cases other than security, though.

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_cryptocell_310`           | A cryptocell peripheral is present.                                           |
| `periph_hash_md5`                 | MD5 hardware acceleration present.                                            |
| `periph_hash_sha_1`               | SHA-1 hardware acceleration present.                                          |
| `periph_hash_sha_224`             | SHA-224 hardware acceleration present.                                        |
| `periph_hash_sha_256`             | SHA-256 hardware acceleration present.                                        |
| `periph_hash_sha_512`             | SHA-512 hardware acceleration present.                                        |
| `periph_hmac_sha_256`             | HMAC SHA-256 hardware acceleration present.                                   |
| `periph_hwrng`                    | A Hardware Random Number Generator (HWRNG) peripheral is present.             |
| `periph_cipher_aes_128_cbc`       | AES 128 CBC hardware acceleration present                                     |
| `periph_ecc_p192r1`               | ECC P192R1 hardware acceleration peripheral present.                          |
| `periph_ecc_p256r1`               | ECC P256R1 hardware acceleration peripheral present.                          |
| `periph_ecc_ed25519`              | ECC Edwards25519 hardware acceleration peripheral present.                    |

# Other Features
Features that did not fit in any category

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `dbgpin`                          | The platform provides the necessary initialization hooks for the `dbgpin` module. |
| `no_idle_thread`                  | The MCU can idle without an idle thread                                       |
| `riotboot`                        | The `riotboot` bootloader is supported.                                       |
| `ssp`                             | Stack Smashing Protection is supported.                                       |
| `tinyusb_device`                  | The TinyUSB network stack is supported and can be selected with `USEPKG += tinyusb`. |
| `bootloader_stm32`                | The MCU has a STM32 bootloader in ROM that can be used for flashing.          |

# Board Features
These features indicate features of the board

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `ethernet`                        | The board has Ethernet connectivity                                           |
| `highlevel_stdio`                 | A high-level stdio method (such as CDC ACM) is used. This requires a running thread and set-up and will not print during a crash. |
| `motor_driver`                    | A motor_driver configuration is present.                                      |
| `sdcard_spi`                      | An SD-Card SPI configuration is provided.                                     |

## STM32L496G Discovery Board Features
Features available and selectable on the `stm32l496-disco` board only

| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
| `periph_spi_stmod`                | By default, solder bridges SB6, SB7, SB8 are closed and USART1 is connected to the Pmod/STMmod+ connector. If these solder bridges are open and solder bridges SB4, SB5 and SB6 are closed instead, SPI2 is connected to the STMmod+/Pmod connector. Request this feature to use SPI2 with this board configuration. |
