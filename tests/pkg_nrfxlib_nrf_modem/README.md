nRF modem test application
================

Setup

Firmware modem must be loaded **BEFORE** using this test application.
The easiest for now to do so is by downloading the firmware
binaries at https://www.nordicsemi.com/-/media/Software-and-other-downloads/Dev-Kits/nRF9160-DK/nRF9160-modem-FW/mfw_nrf9160_1.3.0.zip
Then use nrfconnect app (available on both Windows and Linux) to load
the downloaded zip to your board.
nRFConnect is available at https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop/download
Alternatively, you can install nrfjprog tool and let RIOT buildsystem flash the nRF modem
firmware by using
` make BOARD=nrf9160dk PROGRAMMER=nrfjprog -C tests/pkg_nrfxlib_nrf_modem/ nrf_modem_fw_flash`
nrfjprog is available on Linux, macOS, and Windows at https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools
Usage
-----

Simply run the application on nRF9160-based board using:

    make all term

Expected result
---------------

The application will initialize the modem library and
wait for a GPS fix event to occur. If your board has
an user defined LED, it will blink during this time.
Once the fix occurs, NMEA frame will be displayed on
your terminal with your current location.
