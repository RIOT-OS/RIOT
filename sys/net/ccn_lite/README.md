CCN lite port of RIOT
=====================

RIOT's support for CCN messages is based on the work of [Christian Tschudin from University of Basel](http://cn.cs.unibas.ch/people/cft/) [(CCN-lite code available here)](https://github.com/cn-uofbasel/ccn-lite).

The network stack can handle multiple faces based on the transceiver or from the local device via RIOT's message system.
To communicate with the stack, one can send messages via RIOTs message system to the CCN-lite relay thread or via a physical network transceiver.

All incoming messages get processed in the main io loop which you can find [here](ccn-lite-relay.c#L283).

The public api on the ccn network stack one can find in ccn_lite/include (see in file "ccnl-riot.h").
Client related functions are located in ccn_lite/include/util (see in file "ccn-riot-client.h").

To deal with the ccn lite network stack one only needs these two api declarations.


Running Test Application
------------------------

A test applications are provided in the projects repository.

### ccn-lite-client

It uses RIOTs shell for user interaction.
The network stack is started on demand in it's own thread. You have to enter "ccn" in the shell.

A bunch of ccn user land code is provide to construct interests and content objects.
To request a file e.g. */riot/text*, the user land code request the first segment (chunk): */riot/text/0* and stalls until its received.
If this chunk has the default chunk size the next chunk is requested, ...
If a smaller chunk arrives the user land code prints out the complete file which was requested.

You can test this functionality by typing "interest /riot/test" in the shell. *See HOWTO.md in the projects directory*.

### ccn-lite-relay

It's a stand alone ccn relay without interactive user control
The network stack is started on boot up and is configured set the device address and to serve requests for "/riot/test".
The ccn stack is ready to server requests coming over the transceiver.

Hardware support
----------------

The CCN-lite stack is currently tested and proved to run on the native port of RIOT and the MSBA2 embedded hardware.
On the MSBA2 platform (32 Bit) CCN-lite only needs a stack of 800 bytes.
It uses the heap for the CS, FIB and PIT.

More
----

This README is still under construction.
