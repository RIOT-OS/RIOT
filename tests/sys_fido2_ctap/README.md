# Test Application for FIDO2 CTAP

This test aims to test the FIDO2 CTAP implementation by creating a FIDO2
authenticator which uses CTAPHID as communication protocol.

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

### Unit testing
1. To make benchmarking faster, disable user presence tests by enabling the CFLAG:
   `CONFIG_FIDO2_CTAP_DISABLE_UP=1` in the Makefile.
2. Flash the device with `make flash`.
3. Run the unit tests by running `make fido2-test`.

Note:
* The tests require python 3.6+.
* The unit tests will require you to reboot the authenticator multiple times. Be patient before continuing as it takes a few seconds for the connection
  between OS and authenticator to be reestablished.
