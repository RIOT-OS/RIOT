@defgroup    boards_common_native Common Native Board
@ingroup     boards_common
@brief       Shared files and configuration for native.

[Family: native](https://github.com/RIOT-OS/RIOT/wiki/Family:-native)

# Overview
![Terminal running RIOT native](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/Native.jpg)

# Hardware
- CPU: Host CPU
- RAM: Host RAM
- Flash:
    - for program execution: Host file system
    - for the @ref drivers_periph_flashpage : emulated in RAM
- Network: Tap Interface
- UART: Runtime configurable - `/dev/tty*` are supported
- Timers: Host timer
- LEDs: One red and one green LED - state changes are printed to the UART
- PWM: Dummy PWM
- QDEC: Emulated according to PWM
- SPI: Runtime configurable - `/dev/spidev*` are supported (Linux host only)
- GPIO: Runtime configurable - `/dev/gpiochip*` are supported (Linux host only)

# Required packages

On Debian/Ubuntu you can install the required libraries with

```
sudo apt install gcc-multilib
```

Likewise, for the unittest execution, `libasan5` is needed for 32 bit binaries.
On Debian/Ubuntu you can install the required libraries with

```
sudo apt install lib32asan5
```
