BOARD ?= samr21-xpro

include ../Makefile.tests_common

# list of arm boards that provide at86rf2xx radios, cc2538_rf or nrf52840
# radios
BOARD_WHITELIST = \
  adafruit-clue \
  adafruit-itsybitsy-nrf52 \
  arduino-nano-33-ble \
  cc2538dk \
  feather-nrf52840 \
  firefly \
  iotlab-m3 \
  iotlab-a8-m3 \
  nrf52840-mdk \
  nrf52840dk \
  nrf52840dongle \
  omote \
  openmote-b \
  openmote-cc2538 \
  particle-argon \
  particle-boron \
  particle-xenon \
  samr21-xpro \
  samr30-xpro \
  reel \
  remote-pa \
  remote-reva \
  remote-revb \
  #

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler
USEMODULE += sock_udp

# Mock OpenWSN scheduler to handle the udp_transmit task.
USEMODULE += openwsn_scheduler_mock
EXTERNAL_MODULE_DIRS += external_modules

# Explicitly include ztimer to enable timeout
USEMODULE += ztimer_usec
DISABLE_MODULE += auto_init_openwsn

CFLAGS +=-DCONFIG_OPENWSN_NETIF_ID=31

include $(RIOTBASE)/Makefile.include
