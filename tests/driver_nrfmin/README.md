# Test for the `nrfmin` radio driver
The `nrfmin` driver is a RIOT specific driver that runs the on-chip radios on
Nordic nRF5x-based CPUs in a proprietary mode. But per default, we run the nRF's
radios either in BLE mode (using NImBLE), or in IEEE802.15.4 mode (using the
`nrf802154` driver).

To make sure the `nrfmin` driver is still covered by RIOT's test system, this
test application is added. The test simply contains a basic GNRC configuration
and of course the `nrfmin` driver, which should be enough to verify the drivers
functionality.
