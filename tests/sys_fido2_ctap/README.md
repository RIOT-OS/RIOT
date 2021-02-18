# Test Application for FIDO2 CTAP

This test aims to test a FIDO2 authenticator build on top of RIOT.

## Usage
The FIDO2 authenticator can be tested in two ways:

### Functional testing
1. Flash the device with `make flash`.
2. Test the authenticator on a website like [Webauthn.io](https://webauthn.io/).

Note: Due to limited support of CTAP2 in browsers as of now, make sure to use the
      Chromium or Google Chrome browser when testing on [Webauthn.io](https://webauthn.io/).

### Unit testing
1. To make benchmarking faster, disable user presence tests by enabling the CFLAG:
   CONFIG_FIDO2_CTAP_DISABLE_UP=1 in the Makefile.
2. Flash the device with `make flash`.
3. Run the unit tests by running `make fido2-test`.

Note: The unit tests will require you to reboot the authenticator multiple times.
      Be patient before continuing as it takes a few seconds for the connection
      between OS and authenticator to be reestablished.