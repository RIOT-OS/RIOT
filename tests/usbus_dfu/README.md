# Dependencies

Install dfu-util package.
* On Ubuntu based system:
        apt-get install dfu-util

# Expected result

Build the RIOT firmware you want to flash.
Build and flash the riotboot bootloader
        make -C tests/riotboot riotboot/flash-bootloader
Build and flash the DFU test application onto SLOT1 (avoid SLOT0 for now)
        APP_VER=2 make -C tests/usbus_dfu riotboot/flash-slot1
Finally use dfu-util to flash your firmware (use a .bin file)
        dfu-util  -a 0 -R -D /path/to/your/firmware.bin

Please not that it will erase the riotboot bootloader, this is known limitation
as this feature is still experimental as long as DFU is not properly integrated
to riotboot or any other bootloader

# Background

This test application can be used to verify the USBUS Device Firmware Upgrade
(DFU) implementation.
Assuming dfu-util package is installed on your system, the board under test
should show up on the host computer with a runtime DFU interface. dfu-util will
detect the device and will switch it to DFU mode. After flashing, the board will
be reset to run the newly flashed firmware.
