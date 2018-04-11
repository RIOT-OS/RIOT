# Introduction

This example intends to explain usage of the riotboot OTA update scheme using
CoAP.

# Quickstart

- run "make" in "dist/tools/firmware"

- run "bin/firmware genkeys sec.key pub.key" in "dist/tools/firmware" to create
  keypair

- flash image and bootloader

    $ "BOARD=samr21-xpro APP_VER=$(date +%s) make -j4 riotboot/flash

- recompile using

    $ BOARD=samr21-xpro APP_VER=$(date +%s) make -j4 clean riotboot

- send upate via coap, e.g.,

    $ coap-cli -m put coap://[<node-ip-address]/firmware -b 64 -f bin/samr21-xpro/ota_example-slot2.bin
