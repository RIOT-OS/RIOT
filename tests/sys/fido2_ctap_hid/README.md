# Test Application for FIDO2 CTAP using USB HID transport binding

This test aims to test the FIDO2 CTAP implementation by creating a FIDO2
authenticator which uses CTAPHID as communication protocol.

Note:
* This test application has only been tested on an nrf52840 DK.

The test application requires at least 16536 bytes of stack memory which are
divided as follows:
* 512 bytes isr_stack
* 1024 usbus
* 15000 bytes FIDO2 CTAP

## Usage
The FIDO2 authenticator can be tested in two ways:

### Functional testing
1. Flash the device with `make flash`.
2. Test the authenticator on a website like [Webauthn.io](https://webauthn.io/).

Note:
* Due to limited support of FIDO2 CTAP in browsers as of now, make sure to use the
  Chromium or Google Chrome browser when testing on [Webauthn.io](https://webauthn.io/).
* When registering and authenticating on [Webauthn.io](https://webauthn.io/) you
will need to push button 1 on your device in order to show user presence.

**Resetting the authenticator**
* To reset the authenticator, meaning that all credentials and state information
will be deleted, execute the `reset.py` file located in this directory.
  * This requires you to install the python fido2 package. To install run:
    `pip install fido2==0.8.1`.

### Unit testing
Unit testing is based on the `fido2_tests` package.

There are two test targets (fido2-test, fido2-test-up). The former requires no user
interaction the latter does.

Before running the tests, you need to first flash the device with this test application. The tests communicate with the authenticator via USB. Therefore, when using a board like the nrf52840dk, you **must** use two USB cables: one for programming the SoC and receiving output and another for direct USB communication with the authenticator through the second USB port.

**fido2-test:**

1. Flash the device with `make flash`.
2. Run the unit tests by running `make fido2-test`.

**fido2-test-up:**

1. Make sure to enable user interaction by setting `CONFIG_FIDO2_CTAP_DISABLE_UP=0` in the Makefile. Additionally
it is recommended to also enable LED animations by setting `CONFIG_FIDO2_CTAP_DISABLE_LED=0` to have an indicator that signals when to press the button.
2. Flash the device with `make flash`.
3. Run the unit tests by running `make fido2-test-up` and follow the instructions. E.g. when `.ACTIVATE UP ONCE` is displayed, press the configured UP button (default button 1) once. **Important**: The tests will generate credentials on the device before they begin. As user presence tests are enabled, you must press the button twice at the start to initiate the actual test.

Note:
* Running the tests for the first time will setup a virtual python environment (venv) and install python dependencies of the tests. To check the dependencies please refer to the requirements.txt of the fido2-tests repository.
* The tests require python 3.6+.
* The tests require [swig](http://www.swig.org/) to be installed on your host computer.
* The tests require [libpcsclite-dev](https://packages.debian.org/de/sid/libpcsclite-dev) to be installed on your host computer.
* The unit tests will require you to reboot the authenticator multiple times. Be patient before continuing as it takes a few seconds for the connection between OS and authenticator to be re-established.
* If you keep getting errors while trying to run the tests try `make clean` in order to remove build artifacts. Then re-flash the device with `make flash` and try to run the tests again with `make fido2-test` or `make fido2-test-up`.
