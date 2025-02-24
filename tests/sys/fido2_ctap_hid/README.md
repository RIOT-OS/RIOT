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

Note:
* The tests require python 3.6+.
* The tests require [swig](http://www.swig.org/) to be installed on your host computer.
* Running the tests for the first time will setup a virtual python environment (venv) and install python dependencies of the tests. To check the dependencies please refer to the `requirements.txt` of the [fido2-tests repository](https://github.com/solokeys/fido2-tests).
* The unit tests will require you to reboot the authenticator multiple times. Be patient before continuing as it takes a few seconds for the connection between OS and authenticator to be re-established.
* If you keep getting errors while trying to run the tests try changing to another git branch and back e.g. `git checkout branch1 && git checkout -` in order to remove build artifacts. Then re-flash the device with `make flash term` and try to run the tests again with `make fido2-test` or `make fido2-test-up`.

fido2-test

1. To make benchmarking faster disable user presence tests by enabling the CFLAG
   `CONFIG_FIDO2_CTAP_DISABLE_UP` in the Makefile or through KConfig.
2. Flash the device with `make flash`.
3. Run the unit tests by running `make fido2-test`.

fido2-test-up

1. Make sure that the CFLAG `CONFIG_FIDO2_CTAP_DISABLE_UP` is disabled as this test target
  requires user interaction.
2. Flash the device with `make flash`.
3. Run the unit tests by running `make fido2-test-up` and follow the instructions. E.g. when `.ACTIVATE UP ONCE` is displayed, press the configured UP button (default button 1) once.
