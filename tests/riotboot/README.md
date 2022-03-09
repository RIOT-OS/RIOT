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

In this test two modules `riotboot_hdr` and `riotboot_slot` are used to showcase
the access to riotboot shared functions.

Automatic test
==============

This application's "test" target can be used to test basic riotboot
functionality:

    BOARD=<board> make flash test

This will:

1. flash bootloader and slot0, invalidate slot1
2. get the running slot and current APP_VER

3. flash slot1 with APP_VER + 1
4. verify slot1 has booted and shows version = APP_VER + 1

5. flash slot0 with APP_VER + 2
4. verify slot0 has booted and shows version = APP_VER + 2

If this test runs correctly, it shows that riotboot's basic functions are
working properly on the target board.
