RIOT bootloader test
====================

This is a basic example how to use RIOT bootloader in your embedded
application.

This test should foremost give you an overview how to use riotboot:

  - `make all` build the test using the target riotboot, which generates
  a binary file of the application with a header on top of it, used by
  the bootloader to recognise a bootable image.

  - `make riotboot/flash` creates the binary files and flashes both
  riotboot and the RIOT image with headers included. This should boot
  as a normal application.

In this test two modules `riot_hdr` and `slot_util` are used to showcase
the access to riotboot shared functions.
