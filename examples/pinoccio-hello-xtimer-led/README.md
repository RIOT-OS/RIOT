Hello World!
============

This is a basic example how to use RIOT in your embedded application on a Pinoccio board.

It prints out the text `Pinoccio Hello World!`.

This example should foremost give you an overview how to use the Makefile system:

* First you must give your application a name, which is commonly the same as the name of the directory it resides in.
  Then you can define a default BOARD for which the application was written.
  By using e.g. `make BOARD=pinoccio-mega256rfr2` you can override the default board.

* The variable `RIOTBASE` contains an absolute or relative path to the directory where you have checked out RIOT.
  If your code resides in a subdirectory of RIOT, then you can use `$(CURDIR)` as it's done in here.

* The variable `QUIET`, which is either `1` or `0`, defines whether to print verbose compile information, or hide them, respectively.

* The last line of your Makefile must be `include $(RIOTBASE)/Makefile.include`.

The code itself may look like your usual *C* beginners hello-world example.

This example also uses xtimer to stop the main thread for one second and than continue with a print out of the system time and blink with the red LED.

This example shows the basic feature of riot ported to the open hardware pinoccio board which can be found under https://github.com/Pinoccio . 
