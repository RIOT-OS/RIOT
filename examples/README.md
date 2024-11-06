# Examples

RIOT provides a wide range of examples to demonstrate the capabilities of the OS and its modules.

Each example contains a `README.md` that provides information and instructions how to run it.

Here is a quick overview of the examples available in the RIOT:

### The Essentials

- [default](./default)
  - This application is a showcase for RIOT's hardware support. Using it
    for your board, you should be able to interactively use any hardware
    that is supported.
- [hello-world](./hello-world)
  - A simple "Hello World" that shows the basic structure of a RIOT application.
- [blinky](./blinky)
  - The classic "Hello World" example for embedded systems: blinking an LED (or printing "Blink!" via stdio when none are available).
- [leds_shell](./leds_shell)
  - The application `leds_shell` is a basic example, which allows easy, interactive
    control of internal board LEDs, and basic GPIO for externally connected simple
    devices (for e.g. additional LEDs, relay, motors - via dedicated drivers, etc.)
    via the shell.
- [saul](./saul)
  - This example demonstrates the usage of the SAUL (Sensor Abstraction Layer User) module.
- [timer_periodic_wakeup](./timer_periodic_wakeup)
  - How to set up a periodic wakeup timer using the RIOT operating system.
- [ipc_pingpong](./ipc_pingpong)
  - This example is to illustrate the usage of RIOTs IPC messaging system.
- [filesystem](./filesystem)
  - This example showcases ways to interact/manage the filesystem in RIOT.

#### RIOT Language Bindings

##### Officially Supported/Targeted

###### _Rust_

- [rust-hello-world](./rust-hello-world)
  - This example demonstrates how to write a simple RIOT application in Rust.
- [rust-gcoap](./rust-gcoap)
  - This example demonstrates how to write a coap server application in Rust using the RIOTs gcoap module.
- [rust-async](./rust-async)
  - This example demonstrates how to use Rusts async/await syntax in a RIOT application.

###### _C++_

- [riot_and_cpp](./riot_and_cpp)
  - Example of using C++ in RIOT applications.

##### Community Supported

- [javascript](./javascript)
  - How to write IoT applications using javascript using JerryScript.
- [lua_basic](./lua_basic)
  - How to write IoT applications using Lua.
- [lua_REPL](./lua_REPL)
  - This example demonstrates how to use the Lua Read-Eval-Print Loop (REPL) in RIOT.
- [micropython](./micropython)
  - How to use the MicroPython port for RIOT.
- [wasm](./wasm)
  - How to use WebAssembly in RIOT.

### Networking & BLE Examples

#### Constraint Application Protocol (CoAP)

- [gcoap](./gcoap)
  - This example demonstrates the usage of the `gcoap` module, a high-level API for CoAP (Constrained Application Protocol) messaging.
- [gcoap_block_server](./gcoap_block_server)
  - CoAP server handling for Block requests, build with gcoap using nanocoap block handling functions.
- [gcoap_fileserver](./gcoap_fileserver)
  - This example demonstrates the usage of the `gcoap`
    module to serve files over CoAP.
- [gcoap_dtls](./gcoap_dtls)
  - This example demonstrates the usage of the `gcoap` module with DTLS.
- [nanocoap_server](./nanocoap_server)
  - This example demonstrates the usage of the `nanocoap` module, a low-level API for CoAP (Constrained Application Protocol) messaging.

#### Bluetooth Low Energy (BLE)

- [nimble_scanner](./nimble_scanner)
  - This example showcases the usage of the `NimBLE` BLE stack as a scanner.
- [skald_eddystone](./skald_eddystone)
  - This example demonstrates the usage of `Skald` for creating an Google `Eddystone` beacon.
- [skald_ibeacon](./skald_ibeacon)
  - This example demonstrates the usage of `Skald` for creating an Apple `iBeacon`.
- [nimble_gatt](./nimble_gatt)
  - This example application configures and runs the NimBLE BLE stack as simple GATT
    server.
- [nimble_heart_rate_sensor](./nimble_heart_rate_sensor)
  - This example demonstrates how to implement asynchronous data transfer using GATT
    notifications by implementing a mock-up BLE heart rate sensor.

#### MQTT

- [asymcute_mqttsn](./asymcute_mqttsn)
  - This application demonstrates the usage of the Asymcute (MQTT-SN) module in RIOT.
- [emcute_mqttsn](./emcute_mqttsn)
  - This application demonstrates the usage of the emCute (MQTT-SN) module in RIOT.
- [paho-mqtt](./paho-mqtt)
  - This example demonstrates the usage of the Paho MQTT client library in RIOT.

#### CoRE Resource Directory

- [cord_ep](./cord_ep)
  - Example of RIOT's Resource Directory (RD) endpoint module, called `cord_ep`
- [cord_lc](./cord_lc)
  - Example of RIOT's Resource Directory (RD) lookup module, called `cord_lc`
- [cord_epsim](./cord_epsim)
  - This example shows how a node can register with a CoRE resource directory

#### GNRC Networking

- [gnrc_minimal](./gnrc_minimal)
  - This is a minimalistic example for RIOT's gnrc network stack.
- [gnrc_networking](./gnrc_networking)
  - This example demonstrates the usage of the GNRC network stack in RIOT.
- [gnrc_networking_subnets](./gnrc_networking_subnets)
  - This example demonstrates IPv6 subnet auto-configuration for networks on
    a tree topology.
- [gnrc_border_router](./gnrc_border_router)
  - Example of `gnrc_border_router` using automatic configuration
- [gnrc_lorawan](./gnrc_lorawan)
  - Send and receive LoRaWAN packets and perform basic LoRaWAN commands
- [gnrc_networking_mac](./gnrc_networking_mac)
  - This example shows you how to try out communications between RIOT instances with duty-cycled MAC layer protocols

#### Misc

- [lorawan](./lorawan)
  - This application shows a basic LoRaWAN use-case with RIOT.
- [openthread](./openthread)
  - This example demonstrates the usage of the OpenThread stack in RIOT.
- [lwm2m](./lwm2m)
  - Example of a LWM2M client on RIOT
- [ccn-lite-relay](./ccn-lite-relay)
  - This application demonstrates how to use the Content-Centric Networking stack
    from [CCN-Lite](http://www.ccn-lite.net/) on RIOT
- [telnet_server](./telnet_server)
  - Simple telnet server that listens on port 23 over IPv6.
- [posix_sockets](./posix_sockets)
  - Showcase for RIOT's POSIX socket support
- [spectrum-scanner](./spectrum-scanner)
  - This example demonstrates how to monitor energy levels on all available wireless channels
- [sniffer](./sniffer)
  - This application is built to run together with the script `./tools/sniffer.py`
    as a sniffer for (wireless) data traffic.
- [dtls-sock](./dtls-sock)
  - This example shows how to use DTLS sock `sock_dtls_t`
- [dtls-echo](./dtls-echo)
  - This example shows how to use TinyDTLS with sock_udp.
- [dtls-wolfssl](./dtls-wolfssl)
  - This example demonstrates the usage of the DTLS module with the wolfSSL library.

### Advanced Examples

- [bindist](./bindist)
  - RIOT allows for creating a "binary distribution", which can be used to ship
    proprietary, compiled objects in a way that makes it possible to re-link them
    against a freshly compiled RIOT. This application serves as a simple example.
- [usbus_minimal](./usbus_minimal)
  - This is a minimalistic example for RIOT's USB stack.
- [suit_update](./suit_update)
  - This example shows how to integrate SUIT-compliant firmware updates into a
    RIOT application.
- [thread_duel](./thread_duel)
  - This is a thread duel application to show RIOTs abilities to run multiple-threads
    concurrently, even if they are neither cooperative nor dividable into different scheduler priorities, by using the optional round-robin scheduler module.
- [posix_select](./posix_select)
  - This example is a showcase for RIOT's POSIX select support
- [psa_crypto](./psa_crypto)

  - Basic functions of the PSA Crypto API- [nimble_heart_rate_sensor](./nimble_heart_rate_sensor)
  - This example demonstrates how to implement asynchronous data transfer using GATT
    notifications by implementing a mock-up BLE heart rate sensor.

- [pio_blink](./pio_blink)
  - How to use the PIO peripheral on the RaspberryPi Pico to blink an LED.
- [arduino_hello-world](./arduino_hello-world)
  - This application demonstrates the usage of Arduino sketches in RIOT.
- [twr_aloha](./twr_aloha)
  - This example allows testing different two-way ranging algorithms between
    two boards supporting a dw1000 device. This makes use of the uwb-core
    pkg.
