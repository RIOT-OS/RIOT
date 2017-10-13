Hello Bootloader!
=================

This is a basic example which shows a good working bootloader.

This example should foremost give you an overview how to use the RIOT bootloader:

* First you must give your application a name, which is commonly the same as the name of the directory it resides in.
  Then you can define a default BOARD for which the application was written.
  By using e.g. `make BOARD=iotlab-m3` you can override the default board.
  Be aware that the currently working boards are listed in the `BOARD_WHITELIST`
  statement.
  With `make buildtest` the application gets compiled for all supported boards.

* The variable `RIOTBASE` contains an absolute or relative path to the directory where you have checked out RIOT.
  If your code resides in a subdirectory of RIOT, then you can use `$(CURDIR)` as it's done in here.

* The variable `APP_ID` determines a 32-bit hex number to represent an application
specifically compiled for a given architecture. In subsequent updates, it should
not change.

* The variable `APP_VERSION` is a 32-bit hex number which gives encoded information
about the application version. This information may be decoded or not, but the
value must be incremented for an update.

* The variable `QUIET`, which is either `1` or `0`, defines whether to print verbose compile information, or hide them, respectively.

* The last line of your Makefile must be `include $(RIOTBASE)/Makefile.include`.

This code prints the current slot information, which should be equals to the
slots defined in $(BOARD)/Makefile.include file.
