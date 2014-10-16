CCN lite port of RIOT
=====================

RIOT's support for CCN messages is based on the work of [Christian Tschudin from University of Basel](http://cn.cs.unibas.ch/people/cft/) [(CCN-lite code available here)](https://github.com/cn-uofbasel/ccn-lite).

The network stack can handle multiple faces based on the transceiver or from the local device via RIOT's message system.
To communicate with the stack, one can send messages via the RIOT message system to the CCN-lite relay thread or via a physical network transceiver.

All incoming messages are processed in the main IO loop, which you can find [here](ccn-lite-relay.c#L302).

The public API of the ccn network stack is defined in `sys/net/include/ccn_lite/ccnl-riot.h`.
Client related functions are defined in `sys/net/include/ccn_lite/util/ccn-riot-client.h`.

To work with the CCN-lite network stack, only these two API declarations need to be considered.


Running Test Application
------------------------

Test applications are provided in the example directory.

### ccn-lite-client

The CCN-lite client uses the RIOT shell for user interaction.
The network stack is started on demand in its own thread. You have to enter `ccn <CS size>` in the shell.
Further details are explained in `examples/ccn-lite-client/README.md`.

### ccn-lite-relay

The CCN-lite relay is an application without interactive user control.
Further details are explained in `examples/ccn-lite-relay/README.md`.

Hardware support
----------------

The CCN-lite stack is currently tested and proved to run on the native port of RIOT and the MSBA2 embedded hardware.
On the MSBA2 platform (32 Bit) CCN-lite only needs a stack of 800 bytes.
It uses the heap for the CS, FIB, and PIT.

Reference
---------

If you are using this implementation in a scientific context, please cite the implementation as follows:

* Emmanuel Baccelli, Christian Mehlis, Oliver Hahm, Thomas C. Schmidt, and Matthias Wählisch, "Information Centric Networking in the IoT: Experiments with NDN in the Wild," in Proc. of 1st ACM Conference on Information-Centric Networking (ICN). New York: ACM, pp. 77-86.

The paper is publicly [available](http://conferences.sigcomm.org/acm-icn/2014/papers/p77.pdf).

More
----

This README is still under construction.
