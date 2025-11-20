@defgroup    boards_common Common Board Groups
@ingroup     boards
@brief       Common definitions and implementations for board groups

Several boards share many definitions and implementations, these are
collectively stored into a common module per board group.

## Utilise DFU-Util with bootloader to flash RIOT binaries

### Flash the bootloader

1. Download the bootloader binary matching you boards from
   https://github.com/rogerclarkmelbourne/STM32duino-bootloader
   e.g., for the STM32 bluepill use `generic_boot20_pc13.bin`.

2. Flash the bootloader using ST-Link, for instructions see:
   - https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Flashing-Bootloader-for-BluePill-Boards
   - https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Programming-an-STM32F103XXX-with-a-generic-%22ST-Link-V2%22-programmer-from-Linux

### Flash RIOT binaries

The following steps are tested with the STM32 bluepill board:

1. Set Boot1 pin to 1 and reset device via button, this will ensure that the
   bootloader stops and will not load any binary already present on the device.
2. Connect bluepill via USB to your PC, the LED should blink continuously.
3. Run `dfu-util -l` to list available, it should list (at least) 3 device with
   USB device ID `1eaf:0003`. If not check Boot1 pin setting
4. Flash RIOT binary by running `PROGRAMMER=dfu-util BOARD=bluepill make flash`
5. Reset Boot1 pin to 0 and reset device via button, this should boot the RIOT
   binary.
