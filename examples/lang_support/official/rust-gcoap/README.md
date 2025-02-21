gcoap used with Rust
====================

This is the advanced Rust example; see ../rust-hello-world/ for the basics.

In extension to the basic example, it shows:

* C code can be mixed with Rust code easily; any C file is built and linked
  as in applications without Rust.

  While it's technically possible to have header files for that code,
  it is easier (and likewise often done in C applications)
  to just translate the entry function's signature manually,
  as is done with the `do_vfs_init()` function.

* Code of Rust applications can be spread out into modules,
  even if it builds on RIOT components.

  The CoAP handler built in the main function
  combines generic CoAP components (from `coap_message_demos`)
  with RIOT specific components (from `riot-coap-handler-demos`).

* Many features of RIOT are exposed to Rust through the riot-wrappers crate,
  which provides safe wrappers around RIOT structures.

  In this example, the abovementioned CoAP handler is run on the gcoap server,
  for which the wrappers provide adaptation to the platform independent handler interface.

  Then, ztimer is used to sleep until the network interfaces are expected to be ready.

  Finally, the available network interfaces are iterated over
  and queried for their IP addresses,
  which makes it easier (in absence of an interactive shell) to find which address CoAP requests can be directed at.

How to use
----------

```
$ make all flash term
[...]
main(): This is RIOT! (Version: 2022.01-devel-560-g7f8ed-rust-application)
constfs mounted successfully
CoAP server ready; waiting for interfaces to settle before reporting addresses...
Active interface from PID KernelPID(6) ("gnrc_netdev_tap")
    Address fe80:0000:0000:0000:1234:56ff:fe78:90ab
    Address 2a02:0b18:c13b:8018:1234:56ff:fe78:90ab
```

Once that is ready, in a parallel shell, run:

```
$ aiocoap-client 'coap://[2a02:0b18:c13b:8018:1234:56ff:fe78:90ab]/.well-known/core'
```

Note that no security is enabled by default so far; this is considered a bug.
To mitigate this, no controls that are expected to be harmful are exposed in this example.
(For example, while LEDs are exposed, GPIO pins are not, for they might not tolerate driving to some level depending on the hardware connected to them).
