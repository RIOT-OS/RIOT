RIOT Node abstraction
=====================

This provides python object abstraction of a node.
The first goal is to be the starting point for the serial abstraction and
build on top of that to provide higher level abstraction like over the shell.

It could provide an RPC interface to a node in Python over the serial port
and maybe also over network.

The goal is here to be test environment agnostic and be usable in any test
framework and also without it.


Testing
-------

Run `tox` to run the whole test suite:

::

    tox
    ...
    ________________________________ summary ________________________________
      test: commands succeeded
      lint: commands succeeded
      flake8: commands succeeded
      congratulations :)
