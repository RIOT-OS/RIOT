Hello World!
============

This is a basic example how to use RIOT in your embedded application.
It prints out the famous text `Hello World!`.

This example should foremost give you an overview how to use the Makefile system:

* First you must give your application a name, which is commonly the same as the name of the directory it resides in.
  Then you can define a default BOARD for which the application was written.
  By using e.g. `make BOARD=msba2` you can override the default board.

* The variable `RIOTBASE` contains an absolute or relative path to the directory where you have checked out RIOT.
  If your code resides in a subdirectory of RIOT, then you can use `$(CURDIR)` as it's done in here.

* The variable `QUIET`, which is either `1` or `0`, defines whether to print verbose compile information, or hide them, respectively.

* The last line of your Makefile must be `include $(RIOTBASE)/Makefile.include`.

The code itself may look like your usual *C* beginners hello-world example.
