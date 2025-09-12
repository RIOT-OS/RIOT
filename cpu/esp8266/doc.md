<!--
SPDX-FileCopyrightText: 2025 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup        cpu_esp8266 ESP8266 / ESP8285
@ingroup         cpu
@brief           RIOT-OS port for Espressif's ESP8266 / ESP8285 MCUs

\section esp8266_riot  RIOT-OS on ESP8266 and ESP8285 boards

## Table of Contents {#esp8266_toc}

1. [Overview](#esp8266_overview)
2. [Short Configuration Reference](#esp8266_short_configuration_reference)
3. [MCU ESP8266](#esp8266_mcu_esp8266)
4. [Toolchain](#esp8266_toolchain)
    1. [RIOT Docker Toolchain (riotdocker)](#esp8266_riot_docker_toolchain)
    2. [Manual Toolchain Installation](#esp8266_manual_toolchain_installation)
5. [Flashing the Device](#esp8266_flashing_the_device)
    1. [Toolchain Usage](#esp8266_toolchain_usage)
    2. [Compile Options](#esp8266_compile_options)
    3. [Flash Modes](#esp8266_flash_modes)
    4. [Erasing the Device](#esp8266_erasing)
6. [Peripherals](#esp8266_peripherals)
    1. [GPIO pins](#esp8266_gpio_pins)
    2. [ADC Channels](#esp8266_adc_channels)
    3. [SPI Interfaces](#esp8266_spi_interfaces)
    4. [I2C Interfaces](#esp8266_i2c_interfaces)
    5. [PWM Channels](#esp8266_pwm_channels)
    6. [Timers](#esp8266_timers)
    7. [SPIFFS Device](#esp8266_spiffs_device)
    8. [Other Peripherals](#esp8266_other_peripherals)
7. [Network Interfaces](#esp8266_network_interfaces)
    1. [WiFi Network Interface](#esp8266_wifi_network_interface)
    2. [WiFi SoftAP Network Interface](#esp8266_wifi_ap_network_interface)
    3. [ESP-NOW Network Interface](#esp8266_esp_now_network_interface)
8. [Preconfigured Devices](#esp8266_preconfigured_devices)
    1. [Network Devices](#esp8266_network_devices)
    2. [SD-Card Device](#esp8266_sd_card_device)
9. [Application-Specific Configurations](#esp8266_application_specific_configurations)
    1. [Application-Specific Board Configuration](#esp8266_application_specific_board_configuration)
    2. [Application-Specific Driver Configuration](#esp8266_application_specific_driver_configuration)
10. [SDK Specific Information](#esp8266_sdk_specifics)
    1. [Tasks](#esp8266_sdk_tasks)
    2. [Heap](#esp8266_esp_idf_heap_implementation)
11. [Debugging](#esp8266_debugging)
    1. [QEMU Mode and GDB](#esp8266_qemu_mode_and_gdb)
    2. [Module esp_gdbstub](#esp8266_esp_gdbstub)

# Overview {#esp8266_overview}

The **RIOT port for Xtensa-ESP** is a bare metal implementation of **RIOT-OS**
for **ESP8266 / ESP8285** SOCs which supports most features of RIOT-OS. The
peripheral SPI and I2C interfaces allow to connect all external hardware
modules supported by RIOT-OS, such as sensors and actuators. SPI interface
can also be used to connect external IEEE802.15.4 modules to integrate
ESP8266 boards into a GNRC network.

The RIOT-OS port for ESP8266 supports ESP8266 as well as ESP8285 MCUs and requires
the [ESP8266 RTOS SDK v3.x](https://github.com/gschorcht/RIOT-Xtensa-ESP8266-RTOS-SDK.git).
To build a RIOT application, simply use the `make` command and specify an
existing ESP8266 board, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=esp8266-esp-12x -C tests/sys/shell/ ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For more information about the `make` command variables and specific compile
options, see section [Compile Options](#esp8266_compile_options).

Although the port does not use the official **ESP8266 RTOS SDK** directly,
it must be installed for compilation. The reason is that the port uses most of
the **ESP8266 SOC definitions** provided by SDK header files. In addition,
it needs the hardware abstraction library (libhal), and **ESP8266 WiFi stack
binary** libraries which are part of the SDK.

[Back to table of contents](#esp8266_toc)

# MCU ESP8266 {#esp8266_mcu_esp8266}

ESP8266 is a low-cost, ultra-low-power, single-core SoCs with an integrated
WiFi module from Espressif Systems. The processor core is based on the
Tensilica Xtensa Diamond Standard 106Micro 32-bit Controller Processor Core,
which Espressif calls L106. The key features of ESP8266 are:

[Back to table of contents](#esp8266_toc)

# Short Configuration Reference {#esp8266_short_configuration_reference}

The following table gives a short reference of all board configuration
parameters used by the ESP8266 port in alphabetical order.

<center>

Parameter                            | Short Description                             | Type[1]
-------------------------------------|-----------------------------------------------|--------
[I2C0_SPEED](#esp8266_i2c_interfaces)| Bus speed of I2C_DEV(0)                       | o
[I2C0_SCL](#esp8266_i2c_interfaces)  | GPIO used as SCL for I2C_DEV(0)               | o
[I2C0_SDA](#esp8266_i2c_interfaces)  | GPIO used as SCL for I2C_DEV(0)               | o
[I2C1_SPEED](#esp8266_i2c_interfaces)| Bus speed of I2C_DEV(1)                       | o
[I2C1_SCL](#esp8266_i2c_interfaces)  | GPIO used as SCL for I2C_DEV(1)               | o
[I2C1_SDA](#esp8266_i2c_interfaces)  | GPIO used as SCL for I2C_DEV(1)               | o
[PWM0_GPIOS](#esp8266_pwm_channels)  | GPIOs that can be used as PWM_DEV(0) channels | o
[SPI0_CS0](#esp8266_spi_interfaces)  | GPIO used as default CS for SPI_DEV(0)        | o

</center>

1. **Type:** m - mandatory, o - optional

The following table gives a short reference  in alphabetical order of modules that can be enabled/disabled by board configurations and/or application's makefile using `USEMODULE` and `DISABLE_MODULE`.

<center>

Module    | Default  | Short description
----------|----------|-------------------
[esp_gdb](#esp8266_qemu_mode_and_gdb) | not used | enable the compilation with debug information for debugging
[esp_gdbstub](#esp8266_esp_gdbstub) | not used | enable the compilation of the `gdbstub` interface
[esp_idf_heap](#esp8266_esp_idf_heap_implementation) | not used | use SDK heap implementation
[esp_now](#esp8266_esp_now_network_interface) | not used  | enable the ESP-NOW network device
[esp_qemu](#esp8266_qemu_mode_and_gdb) | not used  | generate image for `QEMU` and `GDB` debugging
[esp_spiffs](#esp8266_spiffs_device) | not used  |  enable SPIFFS for on-board flash memory
[esp_wifi](#esp8266_wifi_network_interface) | not used  | enable the Wifi network device
[esp_wifi_ap](#esp8266_wifi_ap_network_interface) | not used  | enable the Wifi SoftAP network device

</center>

<center>

MCU         | ESP8266EX
------------|----------------------------
Vendor      | Espressif
Cores       | 1 x Tensilica Xtensa LX106
FPU         | no
RAM         | 80 kByte user-data RAM <br> 32 kByte instruction RAM <br> 32 kByte instruction cache <br/> 16 kByte EST system-data RAM
Flash       | 512 kByte ... 16 MByte
Frequency   | 80 MHz or 160 MHz
Power Consumption | 70 mA in normal operating mode <br> 20 uA in deep sleep mode
Timers      | 1 x 32 bit
ADCs        | 1 x 10 bit (1 channel)
GPIOs       | 16
I2Cs        | 2 (software implementation)
SPIs        | 2
UARTs       | 1 (console) + 1 transmit-only
WiFi        | IEEE 802.11 b/g/n built in
Vcc         | 2.5 - 3.6 V
Datasheet   | [Datasheet](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf)
Technical Reference | [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

</center><br>

@note ESP8285 is simply an ESP8266 SoC with 1 MB built-in flash. Therefore, the
documentation also applies to the SoC ESP8285, even if only the ESP8266 SoC is
described below.

[Back to table of contents](#esp8266_toc)

# Toolchain {#esp8266_toolchain}

The following software components are required for compilation:

- **Xtensa GCC** compiler suite for ESP8266
- Modified **ESP8266 RTOS SDK** which includes all SOC definitions and
  some binary libraries
- Modified version of ESP flash programmer tool **`esptool.py`**

There are two options to install the toolchain:

- using RIOT Docker (**riotdocker**), see section
  [RIOT Docker Toolchain (riotdocker)](#esp8266_riot_docker_toolchain)
- **manual installation**, see section [Manual Toolchain Installation](#esp8266_manual_toolchain_installation)

In both cases, the ESP flash programmer tool `esptool.py` is required,
see section [Installation of `esptool.py`](#esp8266_installation_of_esptool).

[Back to table of contents](#esp8266_toc)

## RIOT Docker Toolchain (riotdocker) {#esp8266_riot_docker_toolchain}

The easiest way to install the toolchain is to use RIOT Docker
`riotdocker`. The compilation process using RIOT Docker consists of two steps

1. making the RIOT application in RIOT Docker with command `make BOARD=...`
2. flashing the RIOT application on the host computer with command
   `make flash-only BOARD=...`

where step 2 requires that the ESP flash programmer tool `esptool.py` is
installed. Both steps can also be performed with a single command on the host
system using the `BUILD_IN_DOCKER` variable:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BUILD_IN_DOCKER=1 make BOARD=... flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

### Preparing the Environment {#esp8266_preparing_the_environment}

Using RIOT Docker requires at least the following software components:

- **Docker** container virtualization software
- RIOT Docker image (**`riotdocker`**)
- ESP flash programmer tool **`esptool.py`**

For information about installing Docker on your host, refer to the appropriate
manuals for your operating system. The easiest way to install Docker on an
Ubuntu/Debian system is for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sudo apt-get install docker.io
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For information on how to install `esptool.py`, see section
[Installation of esptool.py](#esp8266_installation_of_esptool).

[Back to table of contents](#esp8266_toc)

### Using an Existing RIOT Docker Image {#esp8266_using_existing_docker_image}

The easiest way to use RIOT Docker is to use an existing `riotdocker` image.
You can either pull and start the
[schorcht/riotbuild_esp8266_rtos](https://hub.docker.com/r/schorcht/riotbuild_esp8266_rtos)
Docker image which only contains the toolchain for ESP8266 RTOS SDK using

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /path/to/RIOT
docker run -i -t -u $UID -v $(pwd):/data/riotbuild schorcht/riotbuild_esp8266_rtos_sdk
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

or the [riot/riotbuild](https://hub.docker.com/r/riot/riotbuild/) Docker image
(size is about 1.5 GB) which contains the toolchains for all platforms using

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /path/to/RIOT
docker run -i -t -u $UID -v $(pwd):/data/riotbuild riot/riotbuild
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

### Generating a riotdocker Image {#esp8266_generating_docker_image}

Alternatively, you can generate the `riotdocker` image by yourself.
A `riotdocker` fork that only installs the toolchain for ESP8266 RTOS SDK is
available at [GitHub](https://github.com/gschorcht/riotdocker-Xtensa-ESP.git).
After cloning this repository, checkout branch `esp8266_only_rtos_sdk` to
generate a Docker image with a size of "only" 890 MByte:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd $HOME/esp
git clone https://github.com/gschorcht/riotdocker-Xtensa-ESP.git
cd riotdocker-Xtensa-ESP
git checkout esp8266_only_rtos_sdk
docker build -t riotbuild .
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A `riotdocker` version that contains toolchains for all platforms supported
by RIOT can be found at [GitHub](https://github.com/RIOT-OS/riotdocker).
However, the Docker image generated from this Docker file has a size of
about 1.5 GByte.

Once the Docker image has been created, it can be started with the following
commands while in the RIOT root directory:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /path/to/RIOT
docker run -i -t -u $UID -v $(pwd):/data/riotbuild riotbuild
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note RIOT's root directory `/path/to/RIOT` becomes visible as the home
directory of the `riotbuild` user in the Docker. That is, the output
of compilations in RIOT Docker are also accessible on the host system.

Please refer the [RIOT wiki](https://github.com/RIOT-OS/RIOT/wiki/Use-Docker-to-build-RIOT)
on how to use the Docker image to compile RIOT OS.

[Back to table of contents](#esp8266_toc)

### Make Process with RIOT Docker {#esp8266_flashing_using_docker}

Using RIOT Docker, the make process consists of the following two steps:

1. **making** the RIOT binary **within RIOT Docker**
2. **flashing** the RIOT binary using a flasher program **on the host system**

Once the according RIOT Docker image has been started from RIOT's root
directory, a RIOT application can be compiled inside RIOT Docker using the
make command as usual, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=esp8266-esp-12x -C tests/sys/shell/ ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This will generate a RIOT binary in ELF format.

@note You can't use the `flash` target inside RIOT Docker.

The RIOT binary has to be flash outside RIOT Docker on the host system. Since
the RIOT Docker image was started while in RIOT's root directory, the output
of the compilations is also accessible on the host system. On the host system,
the `flash-only` target can then be used to flash the binary.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash-only BOARD=esp8266-esp-12x -C tests/sys/shell/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note Both steps can also be performed with a single command on the host
system using the `BUILD_IN_DOCKER` variable:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
`BUILD_IN_DOCKER=1 make BOARD=... flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## Manual Toolchain Installation {#esp8266_manual_toolchain_installation}

A more simple way to install the toolchain is the manual installation of
all required components as described below.

@note To install the toolchain manually, a 64-bit Linux system is required.
Furthermore, the following packages (Debian/Ubuntu) have to be installed:<br>
`build-essential`, `cppcheck`, `coccinelle`, `curl`, `doxygen`, `git`,
`graphviz`, `make`, `pcregrep`, `python`, `python-serial`, `python3`,
`python3-flake8`, `unzip`, `wget`

[Back to table of contents](#esp8266_toc)

### Installation of Xtensa GCC compiler suite {#esp8266_installation_of_xtensa_gcc}

The Xtensa GCC compiler for ESP8266 configured for use with RIOT-OS can
be downloaded and installed as precompiled binary archive from
[GitHub](https://github.com/gschorcht/xtensa-esp8266-elf):

The shell script `$RIOTBASE/dist/tools/esptools/install.sh` is used to
install the custom ESP8266 toolchain:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ dist/tools/esptools/install.sh esp8266
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the compiler is installed, you have to expand your `PATH` variable by
the directory with Xtensa GCC binaries:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ . dist/tools/esptools/export.sh esp8266
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

### ESP8266 RTOS SDK{#esp8266_esp_sdk}

RIOT-OS uses the ESP8266 RTOS SDK as part of the build. This is downloaded as a
package at build-time and there is no need to install it separately.

[Back to table of contents](#esp8266_toc)

### Installation of esptool.py (ESP flash programmer tool) {#esp8266_installation_of_esptool}


The RIOT port uses the ESP flasher program `esptool.py` as a package directly
from [GitHub](https://github.com/espressif/esptool) within a virtual Python
environment.

For more information on `esptool.py`, please refer to the
[git repository](https://github.com/espressif/esptool).

[Back to table of contents](#esp8266_toc)

# Flashing the Device {#esp8266_flashing_the_device}

## Toolchain Usage {#esp8266_toolchain_usage}

Once you have installed all required components, you should have the following
directories.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/path/to/esp/xtensa-esp8266-elf
/path/to/esp/ESP8266_RTOS_SDK
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To use the toolchain, please ensure that your environment variables are set
correctly to

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
export PATH=/path/to/esp/xtensa-esp8266-elf/bin:$PATH
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## Compile Options {#esp8266_compile_options}

The compilation process can be controlled by following make variables:

<center>

Option | Values | Default | Description
-------|--------|---------|------------
FLASH_MODE | dout, dio, qout, qio | dout | Set the flash mode, please take care with your module, see section [Flash Modes](#esp8266_flash_modes)
PORT | /dev/&lt;port&gt; | /dev/USB0 | Set the USB port for flashing the firmware

</center><br>

Optional features of ESP8266 can be enabled using `USEMODULE` definitions
in the makefile of the application. These are:

<center>

Module | Description
-------|------------
[esp_gdb](#esp8266_qemu_mode_and_gdb) | Enable the compilation with debug information
[esp_gdbstub](#esp8266_esp_gdbstub) | Enable the compilation of the `gdbstub` interface for debugging with `GDB`
[esp_idf_heap](#esp8266_esp_idf_heap_implementation) | Enable SDK heap implementation which provides remaining IRAM as additional heap region
[esp_now](#esp8266_esp_now_network_interface) | Enable the built-in WiFi module with the ESP-NOW protocol as `netdev` network device
[esp_qemu](#esp8266_qemu_mode_and_gdb) | Enable the compilation of an `QEMU` image for debugging with `GDB`
[esp_spiffs](#esp8266_spiffs_device) | Enable the SPIFFS drive in on-board flash memory
[esp_sw_timer](#esp8266_timers) | Enable software timer implementation
[esp_wifi](#esp8266_wifi_network_interface) | Enable the built-in WiFi module in infrastructure mode as `netdev` network device
[esp_wifi_ap](#esp8266_wifi_ap_network_interface) | Enable the built-in WiFi SoftAP module as `netdev` network device

</center><br>

For example, to activate the SPIFFS drive in on-board flash memory, the
makefile of application has simply to add the `esp_spiffs` module to
`USEMODULE` make variable:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_spiffs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modules can also be activated temporarily at the command line when calling
the make command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE="esp_spiffs" make BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## Flash Modes {#esp8266_flash_modes}

The `FLASH_MODE` make command variable determines the mode that is used for
flash access in normal operation.

The flash mode determines whether 2 data lines (`dio` and `dout`) or 4 data
lines (`qio` and `qout`) for addressing and data access. For each data line,
one GPIO is required. Therefore, using `qio` or `qout` increases the
performance of SPI Flash data transfers, but uses two additional GPIOs
(GPIO9 and GPIO10). That is, in this flash modes these GPIOs are not
available for other purposes. If you can live with lower flash data
transfer rates, you should always use `dio` or `dout` to keep GPIO9 and
GPIO10 free for other purposes.

For more information about these flash modes, refer the documentation of
[esptool.py](https://github.com/espressif/esptool/wiki/SPI-Flash-Modes).

@note While ESP8266 modules can be flashed with `qio`, `qout`, `dio` and
`dout`, ESP8285 modules have to be always flashed in `dout` mode. The
default flash mode is `dout`.

[Back to table of contents](#esp8266_toc)

## Erasing the Device {#esp8266_erasing}

The flash memory of ESP8266 can be erased completely with following command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
esptool.py erase_flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

# Peripherals {#esp8266_peripherals}

## GPIO pins {#esp8266_gpio_pins}

ESP8266 has 17 GPIO pins, which are all digital pins. Some of them can not be
used at all or have bootstrapping capabilities and are therefore not available on all boards.

<center>

Pin    | Remarks
-------|--------
GPIO0  | usually pulled up
GPIO1  | UART TxD
GPIO2  | usually pulled up
GPIO3  | UART RxD
GPIO4  | |
GPIO5  | |
GPIO6  | Flash SPI
GPIO7  | Flash SPI
GPIO8  | Flash SPI
GPIO9  | Flash SPI in `qout` and `qio` mode, see section [Flash Modes](#esp8266_flash_modes)
GPIO10 | Flash SPI in `qout` and `qio` mode, see section [Flash Modes](#esp8266_flash_modes)
GPIO11 | Flash SPI
GPIO12 | |
GPIO13 | |
GPIO14 | |
GPIO15 | usually pulled down
GPIO16 | RTC pin and wake up signal in deep sleep mode

</center>

GPIO0, GPIO2, and GPIO15 are bootstrapping pins which are used
to boot ESP8266 in different modes:

<center>

GPIO0 | GPIO2 | GPIO15 (MTDO) | Mode
:----:|:-----:|:-------------:|:------------------
1     | X     | X             | boot in SDIO mode to start OCD
0     | 0     | 1             | boot in UART mode for flashing the firmware
0     | 1     | 1             | boot in FLASH mode to boot the firmware from flash (default mode)

</center>

[Back to table of contents](#esp8266_toc)

## ADC Channels {#esp8266_adc_channels}

ESP8266 has **one dedicated ADC** pin with a resolution of 10 bits. This ADC
pin can measure voltages in the range of **0 V ... 1.1 V**.

@note Some boards have voltage dividers to scale this range to a maximum of
3.3 V. For more information, see the hardware manual for the board.

[Back to table of contents](#esp8266_toc)

## SPI Interfaces {#esp8266_spi_interfaces}

ESP8266 has two SPI controllers:

- _CSPI_ for caching and accessing the flash memory<br>
- _HSPI_ for peripherals

Thus, _HSPI_ is the only SPI interface that is available for peripherals.
It is exposed as RIOT's SPI_DEV(0). The pin configuration of the _HSPI_
interface is fixed as shown in following table.

<center>

Signal       | Pin
-------------|-------
`#SPI0_MISO` | GPIO12
`#SPI0_MOSI` | GPIO13
`#SPI0_SCK`  | GPIO14
`#SPI0_CS0`  | GPIOn with n = 0, 2, 4, 5, 15, 16 (additionally 9, 10 in DOUT flash mode)

</center>

The only pin definition that can be overridden by an application-specific
board configuration is the CS signal defined by `#SPI0_CS0`.

When the SPI is enabled using module `periph_spi`, these GPIOs cannot be
used for any other purpose. GPIOs 0, 2, 4, 5, 15, and 16 can be used as CS
signal. In `dio` and `dout` flash modes (see section
[Flash Modes](#esp8266_flash_modes)), GPIOs 9 and 10 can also be used as
CS signal.

[Back to table of contents](#esp8266_toc)

## I2C Interfaces {#esp8266_i2c_interfaces}

Since the ESP8266 does not or only partially support the I2C in hardware,
I2C interfaces are realized as **bit-banging protocol in software**. The
maximum usable bus speed is therefore `#I2C_SPEED_FAST_PLUS`. The maximum
number of buses that can be defined is 2, `#I2C_DEV(0)` and `#I2C_DEV(1)`.

GPIO pins (`#I2C0_SCL`, `#I2C0_SDA` and/or `I2C1_SCL`, `I2C1_SDA`)
have to be defined in the board-specific peripheral configuration in
`$BOARD/periph_conf.h`. Furthermore, the default I2C bus speed (#I2C0_SPEED
and/or `I2C1_SPEED`) that is used for I2C bus(ses) has to be defined. The
number of configured buses `#I2C_NUMOF` is then determined automatically
from these definitions.

In the following example, only one I2C bus is defined:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define I2C0_SPEED     I2C_SPEED_FAST
#define I2C0_SDA       GPIO4
#define I2C0_SCL       GPIO5
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A configuration with two I2C buses would look like the following:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define I2C0_SPEED     I2C_SPEED_FAST
#define I2C0_SDA       GPIO4
#define I2C0_SCL       GPIO5

#define I2C1_SPEED     I2C_SPEED_NORMAL
#define I2C1_SDA       GPIO2
#define I2C1_SCL       GPIO14
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All these configurations can be overridden by an
[application-specific board configuration](#esp8266_application_specific_board_configuration).

[Back to table of contents](#esp8266_toc)

## PWM Channels {#esp8266_pwm_channels}

The hardware implementation of ESP8266 PWM supports only frequencies as power
of two. Therefore, a **software implementation** of **one PWM device**
(#PWM_DEV(0)) with up to **8 PWM channels** (#PWM_CHANNEL_NUM_MAX) is used.

@note The minimum PWM period that can be realized with this software
implementation is 10 us or 100.000 PWM clock cycles per second. Therefore,
the product of frequency and resolution should not be greater than 100.000.
Otherwise the frequency is scaled down automatically.

The GPIOs that can be used as channels of the PWM device `#PWM_DEV(0)` are
defined by `#PWM0_GPIOS`. By default, all GPIOs that are not used as I2C, SPI,
or UART signals are defined as PWM channels in board definition. As long as
these channels are not initialized with function `#pwm_init`, they can be used as
normal GPIOs for other purposes. Even if they are already initialized, but
have a duty cycle value of 0 can be used as output GPIOs for other purposes.
GPIOs in `#PWM0_GPIOS` that are used for other purposes, e.g., I2C or SPI, are
no longer available as PWM channels.

To define other GPIOs as PWM channels, just overwrite the definition of
`#PWM0_GPIOS` in an
[application-specific board configuration](#esp8266_application_specific_board_configuration)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define PWM0_GPIOS { GPIO12, GPIO13, GPIO14, GPIO15 }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## Timers {#esp8266_timers}

There are two timer implementations:

- **hardware timer** implementation with **1 timer device** and
  only **1 channel** (default)
- **software timer** implementation with **1 timer device** and **10 channels**

By default, the **hardware timer implementation** is used.

Software timers use SDK's timers to implement the timer device and the channels.
Although these SDK timers usually have a precision of a few microseconds, they
can deviate up to 500 microseconds. So if you need a timer with high accuracy,
you'll need to use the hardware timer with only one timer channel.

[Back to table of contents](#esp8266_toc)

## SPIFFS Device {#esp8266_spiffs_device}

If SPIFFS module is enabled (`USEMODULE += esp_spiffs`), the implemented
MTD system drive `#mtd0` for the on-board SPI flash memory is used together
with modules `spiffs` and `vfs` to realize a persistent file system.

For this purpose, the flash memory is formatted as SPIFFS starting at the
address `0x80000` (512 kByte) on first boot. All sectors up to the last
5 sectors of the flash memory are then used for the file system. With a
fixed sector size of 4096 bytes, the top address of the SPIFF is
`flash_size - 5 * 4096`, e.g., `0xfb000` for a flash memory of 1 MByte.
The size of the SPIFF then results from:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
flash_size - 5 * 4096 - 512 kByte
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Please refer file `$RIOTBASE/tests/unittests/test-spiffs/tests-spiffs.c`
for more information on how to use SPIFFS and VFS together with a MTD
device `#mtd0` alias `MTD_0`.

[Back to table of contents](#esp8266_toc)

## Other Peripherals {#esp8266_other_peripherals}

The ESP8266 port of RIOT also supports

- one hardware number generator with 32 bit,
- one UART interface (GPIO1 and GPIO3),
- a CPU-ID function, and
- power management functions.

RTC is not yet implemented.

[Back to table of contents](#esp8266_toc)

# Network Interfaces {#esp8266_network_interfaces}

ESP8266 provides different built-in possibilities to realize network devices:

- **ESP WiFi**, usual AP-based wireless LAN (not yet supported)
- **ESP-NOW**, a WiFi based AP-less and connectionless peer to
  peer communication protocol

[Back to table of contents](#esp8266_toc)

## WiFi Network Interface {#esp8266_wifi_network_interface}

The RIOT port for ESP8266 implements in module `esp_wifi` a `netdev`
driver for
the built-in WiFi interface.

@note Module `esp_wifi` is not automatically enabled when module
`netdev_default` is used. Instead, if necessary, the application has to add
the module `esp_wifi` in the Makefile.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_wifi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Furthermore, the following configuration parameters have to be defined:

<center>

Parameter           | Default                   | Description
:-------------------|:--------------------------|:------------
#WIFI_SSID          | "RIOT_AP"                 | SSID of the AP to be used.
#WIFI_PASS          | -                         | Passphrase used for the AP as clear text (max. 64 chars).
#ESP_WIFI_STACKSIZE | #THREAD_STACKSIZE_DEFAULT |Stack size used for the WiFi netdev driver thread.

</center>

These configuration parameter definitions, as well as enabling the `esp_wifi`
module, can be done either in the makefile of the project or at make command
line, e.g.:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_wifi \
CFLAGS='-DWIFI_SSID=\"MySSID\" -DWIFI_PASS=\"MyPassphrase\"' \
make -C examples/networking/gnrc/gnrc_networking BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- Module `esp_wifi` is not enabled automatically when module
`netdev_default` is used.
- Leave `WIFI_PASS` undefined to connect to an open WiFi access point.
- The Wifi network interface (module `esp_wifi`) and the
[ESP-NOW network interface](#esp8266_esp_now_network_interface)
(module `esp_now`) can be used simultaneously, for example, to realize a
border router for a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP asvalue for the parameter 'ESP_NOW_CHANNEL'.

[Back to table of contents](#esp8266_toc)

## WiFi SoftAP Network Interface {#esp8266_wifi_ap_network_interface}

The RIOT port for the ESP8266 supports a `netdev` interface for the ESP32 WiFi
SoftAP mode. Module `esp_wifi_ap` has to be enabled to use it.

The following parameters can be configured:

<center>

Parameter                 | Default                   | Description
:-------------------------|:--------------------------|:-------------
#WIFI_SSID                | "RIOT_AP"                 | Static SSID definition for the SoftAP
#WIFI_PASS                | none                      | The password for the WiFi SoftAP network interface.[1]
#ESP_WIFI_SSID_DYNAMIC    | 0                         | Defines whether dynamic SSID is used for the SoftAP [2].
#ESP_WIFI_SSID_HIDDEN     | 0                         | Defines whether the SoftAP SSID should be hidden.
#ESP_WIFI_MAX_CONN        | 4                         | The maximum number of connections for the SoftAP.
#ESP_WIFI_BEACON_INTERVAL | 100                       | The beacon interval time in milliseconds for the SoftAP.
#ESP_WIFI_STACKSIZE       | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center><br>

[1] If no password is provided, the interface will be "open", otherwise it
    uses WPA2-PSK authentication mode.<br>
[2] If #ESP_WIFI_SSID_DYNAMIC is set to 1, a dynamic SSID is generated for the
    SoftAP by extending the defined SSID (`WIFI_SSID`) with the MAC address
    of the SoftAP interface used, e.g.: `RIOT_AP_aabbccddeeff`

These configuration parameter definitions, as well as enabling the `esp_wifi_ap`
module, can be done either in the makefile of the project or at make command
line, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_wifi_ap \
CFLAGS='-DWIFI_SSID=\"MySSID\" -DWIFI_PASS=\"MyPassphrase\" -DESP_WIFI_MAX_CONN=1' \
make -C examples/networking/gnrc/gnrc_networking BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- The `esp_wifi_ap` module is not used by default when `netdev_default` is used.
- Supports open and WPA2-PSK authentication modes.
- The ESP-NOW network interface and the WiFi SoftAP network interface can not
be used simultaneously.

@warning
The SoftAP may or may not be at all reliable sometimes, this is a known
problem with the Wi-Fi network interface, even on the official ESP-IDF.
The problem is that the AP doesn't cache multicast data for connected
stations, and if stations connected to the AP are power save enabled,
they may experience multicast packet loss. This affects RIOT, because
NDP relies on multicast packets to work correctly.
Refer to the SDK documentation from Espressif on [AP Sleep](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#ap-sleep) for more information.

[Back to table of contents](#esp8266_toc)

## ESP-NOW Network Interface {#esp8266_esp_now_network_interface}

With ESP-NOW, the ESP8266 provides a connectionless communication technology,
featuring short packet transmission. It applies the IEEE802.11 Action Vendor
frame technology, along with the IE function developed by Espressif, and CCMP
encryption technology, realizing a secure, connectionless communication
solution.

The RIOT port for ESP8266 implements in module `esp_now` a `netdev` driver
which uses ESP-NOW to provide a link layer interface to a meshed network of
ESP8266 nodes. In this network, each node can send short packets with up to
250 data bytes to all other nodes that are visible in its range.

@note Module `esp_now` is not enabled automatically if the `netdev_default`
module is used. Instead, the application has to add the `esp_now` module in
its makefile when needed.<br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_now
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For ESP-NOW, ESP8266 nodes are used in WiFi SoftAP + Station mode to advertise
their SSID and become visible to other ESP8266 nodes. The SSID of an ESP8266
node is the concatenation of the prefix `RIOT_ESP_` with the MAC address of
its SoftAP WiFi interface. The driver periodically scans all visible ESP8266
nodes.

The following parameters are defined for ESP-NOW nodes. These parameters can
be overridden by [application-specific board configurations](#esp8266_application_specific_board_configuration).

<center>

Parameter | Default | Description
:---------|:--------|:-----------
#ESP_NOW_SCAN_PERIOD_MS | 10000UL | Defines the period in ms at which an node scans for other nodes in its range. The default period is 10 s.
#ESP_NOW_SOFT_AP_PASS | "ThisistheRIOTporttoESP" | Defines the passphrase as clear text (max. 64 chars) that is used for the SoftAP interface of ESP-NOW nodes. It has to be same for all nodes in one network.
#ESP_NOW_CHANNEL | 6 | Defines the channel that is used as the broadcast medium by all nodes together.
#ESP_NOW_KEY | NULL | Defines a key that is used for encrypted communication between nodes. If it is NULL, encryption is disabled. The key has to be of type `uint8_t[16]` and has to be exactly 16 bytes long.

</center>

@note The ESP-NOW network interface (module `esp_now`) and the
[Wifi network interface](#esp8266_wifi_network_interface) (module `esp_wifi`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP asvalue for the parameter 'ESP_NOW_CHANNEL'.

[Back to table of contents](#esp8266_toc)

# Preconfigured Devices {#esp8266_preconfigured_devices}

The ESP8266 port of RIOT has been tested with several common external
devices that can be connected to ESP8266 boards and are preconfigured
accordingly.

[Back to table of contents](#esp8266_toc)

## Network Devices {#esp8266_network_devices}

RIOT provides a number of driver modules for different types of network
devices, e.g., IEEE 802.15.4 radio modules and Ethernet modules. The RIOT
port for ESP8266 has been tested with the following network devices:

- \ref drivers_mrf24j40 "mrf24j40"
  (driver for Microchip MRF24j40 based IEEE 802.15.4)
- \ref drivers_enc28j60 "enc28j60"
  (driver for Microchip ENC28J60 based Ethernet modules)

[Back to table of contents](#esp8266_toc)

### Using MRF24J40 (module mrf24j40) {#esp8266_using_mrf24j40}

To use MRF24J40 based IEEE 802.15.4 modules as network device, the
`mrf24j40` driver module has to be added to the makefile of the application:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += mrf24j40
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `mrf24j40` driver module uses the following preconfigured interface
parameters for ESP8266 boards:

<center>

Parameter                | Default         | Remarks
-------------------------|-----------------|----------------------------
`#MRF24J40_PARAM_SPI`    | `#SPI_DEV(0)`   | fixed, see section [SPI Interfaces](#esp8266_spi_interfaces)
`#MRF24J40_PARAM_SPI_CLK`| `#SPI_CLK_1MHZ` | fixed
`#MRF24J40_PARAM_CS`     | `GPIO16`        | can be overridden
`#MRF24J40_PARAM_INT`    | `GPIO0`         | can be overridden
`#MRF24J40_PARAM_RESET`  | `GPIO2`         | can be overridden

</center><br>

The GPIOs in this configuration can be overridden by
[application-specific board configurations](#esp8266_application_specific_board_configuration).

[Back to table of contents](#esp8266_toc)

### Using ENC28J60 (module enc28j60) {#esp8266_using_enc28j60}

To use ENC28J60 Ethernet modules as network device, the `enc28j60` driver
module has to be added to the makefile of the application:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += enc28j60
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `enc28j60` driver module uses the following preconfigured interface parameters for ESP8266 boards:

<center>

Parameter              | Default       | Remarks
-----------------------|---------------|----------------------------
`#ENC28J60_PARAM_SPI`  | `#SPI_DEV(0)` | fixed, see section [SPI Interfaces](#esp8266_spi_interfaces)
`#ENC28J60_PARAM_CS`   | `GPIO4`       | can be overridden
`#ENC28J60_PARAM_INT`  | `GPIO9`       | can be overriddn
`#ENC28J60_PARAM_RESET`| `GPIO10`      | can be overridden

</center>

The GPIOs in this configuration can be overridden by
[application-specific board configurations](#esp8266_application_specific_board_configuration).

[Back to table of contents](#esp8266_toc)

## SD-Card Device {#esp8266_sd_card_device}

ESP8266 port of RIOT is preconfigured for RIOT applications that use the
\ref drivers_sdcard_spi "SPI SD-Card driver.
To use SPI SD-Card driver, the `sdcard_spi` module has to be added to
a makefile:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += sdcard_spi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `sdcard_spi` driver module uses the following preconfigured interface
parameters for ESP8266 boards:

<center>

Parameter              | Default      | Remarks
-----------------------|--------------|----------------------------
`#SDCARD_SPI_PARAM_SPI`| `#SPI_DEV(0)`| fix, see section [SPI Interfaces](#esp8266_spi_interfaces)
`#SDCARD_SPI_PARAM_CS` | `#SPI0_CS0`  | can be overridden

</center>

The GPIO used as CS signal can be overridden by
[application-specific board configurations](#esp8266_application_specific_board_configuration).

[Back to table of contents](#esp8266_toc)

# Application-Specific Configurations {#esp8266_application_specific_configurations}

The board-specific configuration files `board.h` and `periph_conf.h` as
well as the driver parameter configuration files `<driver>_params.h`
define the default configurations for peripherals and device driver
modules. These are, for example, the GPIOs used, bus interfaces used
or available bus speeds. Because there are many possible configurations
and many different application requirements, these default configurations
are usually only a compromise between different requirements.

Therefore, it is often necessary to change some of these default
configurations for individual applications. For example, while many
PWM channels are needed in one application, another application does
not need PWM channels, but many ADC channels.

[Back to table of contents](#esp8266_toc)

## Application-Specific Board Configuration {#esp8266_application_specific_board_configuration}

To override default board configurations, simply create an
application-specific board configuration file `$APPDIR/board.h` in
the source directory `$APPDIR` of the application and add the
definitions to be overridden. To force the preprocessor to include
board's original `board.h` after that, add the `include_next`
preprocessor directive as the **last** line.

For example to override the default definition of the GPIOs that are
used as PWM channels, the application-specific board configuration
file `$APPDIR/board.h` could look like the following:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_ESP8266
#define PWM0_GPIOS { GPIO12, GPIO13, GPIO14, GPIO15 }
#endif

#include_next "board.h"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is important to ensure that the application-specific board
configuration `$APPDIR/board.h` is included first. Insert the following
line as the **first** line to the application makefile `$APPDIR/Makefile`.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INCLUDES += -I$(APPDIR)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note To make such application-specific board configurations dependent
on the ESP8266 MCU or a particular ESP8266 board, you should always
enclose these definitions in the following constructs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_ESP8266
...
#endif

#ifdef BOARD_ESP8266_ESP-12X
...
#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## Application-Specific Driver Configuration {#esp8266_application_specific_driver_configuration}

Using the approach for overriding board configurations, the parameters
of drivers that are typically defined in
`drivers/<device>/include/<device>_params.h` can be overridden. For that
purpose just create an application-specific driver parameter file
`$APPDIR/<device>_params.h` in the source directory `$APPDIR` of the
application and add the definitions to be overridden. To force the
preprocessor to include driver's original `<device>_params.h` after
that, add the `include_next` preprocessor directive as the **last** line.

For example, to override a GPIO used for LIS3DH sensor, the
application-specific driver parameter file `$APPDIR/<device>_params.h`
could look like the following:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_ESP8266
#define LIS3DH_PARAM_INT2           (GPIO_PIN(0, 4))
#endif

#include_next "lis3dh_params.h"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is important to ensure that the application-specific driver parameter
file `$APPDIR/<device>_params.h` is included first. Insert the following
line as the **first** line to the application makefile `$APPDIR/Makefile`.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INCLUDES += -I$(APPDIR)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note To make such application-specific board configurations
dependent on the ESP8266 MCU or a particular ESP8266 board, you should
always enclose these definitions in the following constructs:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_ESP8266
...
#endif

#ifdef BOARD_ESP8266_ESP-12X
...
#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

# SDK Specific Information {#esp8266_sdk_specifics}

## SDK Tasks {#esp8266_sdk_tasks}

ESP8266 RTOS SDK libraries create a number of high-priority threads, see the
listing below, which handle high priority interrupts from SoC and WiFi
hardware. These threads are also created, if the WiFi hardware is not used.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	pid | name                 | state    Q | pri | stack  ( used) | base addr  | current
	  - | isr_stack            | -        - |   - |   2048 (  832) | 0x3ffe8420 | 0x3ffe8c20
	  1 | ppT                  | bl rx    _ |   2 |   3632 ( 1296) | 0x3fff5df0 | 0x3fff6ac0
	  2 | pmT                  | bl rx    _ |   4 |   1072 (  320) | 0x3fff6c70 | 0x3fff6f70
	  3 | rtT                  | bl rx    _ |   3 |   2096 ( 1376) | 0x3fff70b0 | 0x3fff77b0
	  4 | esp_events           | bl rx    _ |   5 |   2096 (  864) | 0x3fff7f20 | 0x3fff8600
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

## SDK Heap Implementation {#esp8266_esp_idf_heap_implementation}

Using module `esp_idf_heap` enables the compilation of SDK heap handling
instead of memory management provided by `newlibc`.

Normally, the remaining ESP8266 DRAM, which is not used by static variables,
is provided as heap memory. The SDK heap implementation also provides the unused
ESP8266 IRAM (Command RAM) as the additional heap memory region.

@note The heap in IRAM allows only 32-bit-aligned word access.

The following example shows the heap when the `esp_idf_heap` is used:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Heap region 0 @40107690: 18800 (used 8, free 18792) [bytes]
Heap region 1 @3fff1760: 59552 (used 8520, free 51032) [bytes]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example, heap region 0 at address `0x401xxxxx` is located in IRAM and
heap region 1 at address `0x3fffxxxx` in DRAM. While memory management
functions of `newlibc` use always heap region 1 in DRAM, functions of
binary SDK libraries like the WiFi stack can also also heap region 0
for 32-bit aligned data.

[Back to table of contents](#esp8266_toc)

# Debugging {#esp8266_debugging}

There are two options to debug your RIOT application for ESP8266 either

- using `QEMU` and module `esp_gdb`, see [QEMU Mode andB](#esp8266_qemu_mode_and_gdb) or
- using module `esp_gdbstub`, see [Module esp_gdbstub](#esp8266_esp_gdbstub).

## QEMU Mode and GDB {#esp8266_qemu_mode_and_gdb}

### Compilation for `QEMU`

When `QEMU` mode is enabled by using the `esp_qemu` module, `make flash` does
not try to download the image of your RIOT application to the target hardware.
Instead, a binary image `$ELFFILE.bin` as well as the 1 MByte flash image
`esp8266flash.bin` is created in the application build directory `$(BINDIR)`.
This flash image can be used together with `QEMU` and `GDB` to debug the
application.

For debugging purposes, the application should be compiled with debugging
information. This can either be done by using the `esp_gdb` module, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_gdb make flash BOARD=esp8266-esp-12x -C tests/sys/shell/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

### Installation of `QEMU`

To use `QEMU`, you have to install `QEMU` for Xtensa with ESP8266 machine
implementation first as following.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd /my/source/dir
git clone https://github.com/gschorcht/qemu-xtensa
cd qemu-xtensa/
git checkout xtensa-esp8266
export QEMU=/path/to/esp/qemu
./configure --prefix=$QEMU --target-list=xtensa-softmmu --disable-werror
make
make install
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp8266_toc)

### Start Debugging with `QEMU`

Once the compilation has been finished, `QEMU` for Xtensa with ESP8266
machine implementation should be available in `/path/to/esp/qemu/bin` and
you can start it in first terminal with

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
term1> $QEMU/bin/qemu-system-xtensa -M esp8266 -nographic -serial stdio -monitor none -s -S \
                                    -kernel /path/to/build/dir/esp8266flash.bin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

where `/path/to/build/dir` is the path to the application build directory
`$(BINDIR)` where `$ELFFILE.bin` is generated by the `make` command, for example
`$(RIOTBASE)/tests/sys/shell//bin/esp8266-esp-12x`. After that you can
start `GDB` in second terminal window using command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
term2> xtensa-esp8266-elf-gdb /path/to/build/dir/image.elf
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To start debugging, you have to connect to `QEMU` from `GDB` with command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) target remote :1234
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note `QEMU` does not support the emulation of hardware interrupts or
special hardware modules like the WiFi module. Applications that rely on
interrupts or the WiFi interface can only be debugged with restrictions
with `QEMU` and `GDB`.

[Back to table of contents](#esp8266_toc)

## Module esp_gdbstub {#esp8266_esp_gdbstub}

### Compilation with `esp_gdbstub`

Using the `esp_gdbstub` module enables the compilation of the `gdbstub`
interface for ESP8266. This interface implements the target side of the
[GDB Remote Protocol](https://sourceware.org/gdb/onlinedocs/gdb/Remote-Protocol.html#Remote-Protocol).
The initial `gdbstub` implementation for ESP8266 was provided by
[Espressif](https://github.com/espressif/esp-gdbstub). However, for using it
with RIOT it had to be changed a lot.

@note Enabling the `gdbstub` interface automatically enables the compilation
with debug information (module `esp_gdb`) `gdbstub`.

[Back to table of contents](#esp8266_toc)

### Start Debugging with `esp_gdbstub`

To start debugging, the application has to be compiled using
module `esp_gdbstub`, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_gdbstub make flash BOARD=esp8266-esp-12x -C tests/sys/shell/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once, the application is flashed to ESP82666, debugging can be started as
following.

1. Start in first terminal window a terminal program which connects to the
port of the ESP8266 module as console window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
term1> python -m serial.tools.miniterm &lt;port&gt; 115200
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
where `&lt;port&gt;` is the serial interface to which the ESP8266 module is connected,
e.g., `/dev/ttyUSB0`.

2. Start GDB with the application in a second terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
term2> xtensa-esp8266-elf-gdb /path/to/the/build/dir/image.elf
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
where `/path/to/build/dir` is the path to the application build directory
`$(BINDIR)` where `$ELFFILE.bin` is generated by the `make` command, for example
`$(RIOTBASE)/tests/sys/shell//bin/esp8266-esp-12x`.

3. Connect from `GDB` to the ESP8266 module with command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) target remote &lt;port&gt;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
where `&lt;port&gt;` is the serial interface to which the ESP8266 module is connected,
e.g., `/dev/ttyUSB0`.

By default, `gdbstub` stops the execution automatically using function
`gdbstub_do_break` after the board initialization and before the kernel
is initialized.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) tar rem /dev/ttyUSB0
Remote debugging using /dev/ttyUSB0
gdbstub_do_break_breakpoint_addr () at cpu/esp8266/vendor/esp-gdbstub/gdbstub-entry.S:400
400	    break 0,0
(gdb) bt
#0  gdbstub_do_break_breakpoint_addr () at cpu/esp8266/vendor/esp-gdbstub/gdbstub-entry.S:400
#1  0x40100f89 in gdbstub_init () at cpu/esp8266/vendor/esp-gdbstub/gdbstub.c:985
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
At this time, you can set breakpoints, execute
the application stepwise or just continue the execution using the `continue`
command. Please note the limitations below. Once you have started the execution
in `GDB`, you can use `Ctrl-C` in `GDB` or the console window to break it anytime.

If your application uses console inputs (stdio), such as the `tests/sys/shell/`
application, you can type characters in the console window that gdbstub will
forward to the application. However, the echo of these inputs occur in `GDB`.

@note Since the terminal program and `GDB` are using the same serial port,
typed characters can be lost sporadically.

When you reset the ESP8266 module, you will observe a message such as the
following after boot messages in the console window.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$T05#b9
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This is simply the first GDB Remote Protocol packet that is generated as a
result of the automatic break during the initialization when the GDB is not
yet connected to the ESP8266 module.

[Back to table of contents](#esp8266_toc)

### Limitations of `esp_gdbstub`

Due to hardware limitations of the Xtensa architecture, `esp_gdbstub`
debugging has the following limitations:

- Software breakpoints (command `br`) only work for code in IRAM with
  addresses `0x4010xxxx`, see below.
- Code in IROM (flash) with addresses `0x402xxxxx` require a hardware
  breakpoint (command `hbr`), see below.
- There is only one hardware breakpoint (command `hbr`).
- There is only one hardware watchpoint (command `watch`).
- Stepwise execution on source code level is only possible in IRAM with
  addresses `0x4010xxxx`, see below.

If you want to use multiple software breakpoints or execute stepwise on
source code level, you have to ensure that the function you want to debug is
located in RAM. For that purpose, add the `IRAM` attribute to that function,
for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#include "esp/common_macros.h"
...
void IRAM my_func(void)
{
    ...
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Then you should be able to set a breakpoint to this function using command `bp`
and to execute it stepwise after break.

Another option is to use `gdbstub_do_break()` wherever you want to break the
execution. If you know where you want to break before downloading the program
to the target, you can use `gdbstub_do_break()` as much as you want.

[Back to table of contents](#esp8266_toc)
