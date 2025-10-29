# Examples

RIOT provides a wide range of examples to demonstrate the capabilities of the OS and its modules.

Each example contains a `README.md` that provides information and instructions how to run it.

Here is a quick overview of the examples available in the RIOT:

## The Essentials

| Example | Description |
|---------|-------------|
| [default](./basic/default/README.md) | This application is a showcase for RIOT's hardware support. Using it for your board, you should be able to interactively use any hardware that is supported. |
| [hello-world](./basic/hello-world/README.md) | A simple "Hello World" that shows the basic structure of a RIOT application. |
| [blinky](./basic/blinky/README.md) | The classic "Hello World" example for embedded systems: blinking an LED (or printing "Blink!" via stdio when none are available). |
| [leds_shell](./basic/leds_shell/README.md) | The application `leds_shell` is a basic example, which allows easy, interactive control of internal board LEDs, and basic GPIO for externally connected simple devices (for e.g. additional LEDs, relay, motors - via dedicated drivers, etc.) via the shell. |
| [saul](./basic/saul/README.md) | This example demonstrates the usage of the SAUL (Sensor Actuator Uber Layer) module. |
| [timer_periodic_wakeup](./basic/timer_periodic_wakeup/README.md) | How to set up a periodic wakeup timer using the RIOT operating system. |
| [ipc_pingpong](./basic/ipc_pingpong/README.md) | This example is to illustrate the usage of RIOTs IPC messaging system. |
| [filesystem](./basic/filesystem/README.md) | This example showcases ways to interact/manage the filesystem in RIOT. |
| [subfolders](./basic/subfolders/README.md) | This example demonstrates how to use subfolders in RIOT applications. |

## RIOT Language Bindings

#### Officially Supported/Targeted

##### _Rust_

| Example | Description |
|---------|-------------|
| [rust-hello-world](./lang_support/official/rust-hello-world/README.md) | This example demonstrates how to write a simple RIOT application in Rust. |
| [rust-gcoap](./lang_support/official/rust-gcoap/README.md) | This example demonstrates how to write a coap server application in Rust using the RIOTs gcoap module. |
| [rust-async](./lang_support/official/rust-async/README.md) | This example demonstrates how to use Rusts async/await syntax in a RIOT application. |

##### _C++_

| Example | Description |
|---------|-------------|
| [riot_and_cpp](./lang_support/official/riot_and_cpp/README.md) | Example of using C++ in RIOT applications. |

#### Community Supported

| Example | Description |
|---------|-------------|
| [javascript](./lang_support/community/javascript/README.md) | How to write IoT applications using javascript using JerryScript. |
| [lua_basic](./lang_support/community/lua_basic/README.md) | How to write IoT applications using Lua. |
| [lua_REPL](./lang_support/community/lua_REPL/README.md) | This example demonstrates how to use the Lua Read-Eval-Print Loop (REPL) in RIOT. |
| [micropython](./lang_support/community/micropython/README.md) | How to use the MicroPython port for RIOT. |
| [wasm](./lang_support/community/wasm/README.md) | How to use WebAssembly in RIOT. |
| [arduino_hello-world](./lang_support/community/arduino_hello-world/README.md) | This application demonstrates the usage of Arduino sketches in RIOT. |

## Networking

### Constraint Application Protocol (CoAP)

| Example | Description |
|---------|-------------|
| [gcoap](./networking/coap/gcoap/README.md) | This example demonstrates the usage of the `gcoap` module, a high-level API for CoAP (Constrained Application Protocol) messaging. |
| [gcoap_block_server](./networking/coap/gcoap_block_server/README.md) | CoAP server handling for Block requests, build with gcoap using nanocoap block handling functions. |
| [gcoap_fileserver](./networking/coap/gcoap_fileserver/README.md) | This example demonstrates the usage of the `gcoap` module to serve files over CoAP. |
| [gcoap_dtls](./networking/coap/gcoap_dtls/README.md) | This example demonstrates the usage of the `gcoap` module with DTLS. |
| [nanocoap_server](./networking/coap/nanocoap_server/README.md) | This example demonstrates the usage of the `nanocoap` module, a high-level API for CoAP (Constrained Application Protocol) messaging. |

### Bluetooth Low Energy (BLE)

#### NimBLE

| Example | Description |
|---------|-------------|
| [nimble_scanner](./networking/ble/nimble/nimble_scanner/README.md) | This example showcases the usage of the `NimBLE` BLE stack as a scanner. |
| [nimble_gatt](./networking/ble/nimble/nimble_gatt/README.md) | This example application configures and runs the NimBLE BLE stack as simple GATT server. |
| [nimble_heart_rate_sensor](./networking/ble/nimble/nimble_heart_rate_sensor/README.md) | This example demonstrates how to implement asynchronous data transfer using GATT notifications by implementing a mock-up BLE heart rate sensor. |

#### Misc BLE Examples

| Example | Description |
|---------|-------------|
| [skald_eddystone](./networking/ble/misc/skald_eddystone/README.md) | This example demonstrates the usage of `Skald` for creating an Google `Eddystone` beacon. |
| [skald_ibeacon](./networking/ble/misc/skald_ibeacon/README.md) | This example demonstrates the usage of `Skald` for creating an Apple `iBeacon`. |

### MQTT

| Example | Description |
|---------|-------------|
| [asymcute_mqttsn](./networking/mqtt/asymcute_mqttsn/README.md) | This application demonstrates the usage of the Asymcute (MQTT-SN) module in RIOT. |
| [emcute_mqttsn](./networking/mqtt/emcute_mqttsn/README.md) | This application demonstrates the usage of the emCute (MQTT-SN) module in RIOT. |
| [paho-mqtt](./networking/mqtt/paho-mqtt/README.md) | This example demonstrates the usage of the Paho MQTT client library in RIOT. |

### CoRE Resource Directory

| Example | Description |
|---------|-------------|
| [cord_ep](./networking/cord/cord_ep/README.md) | Example of RIOT's Resource Directory (RD) endpoint module, called `cord_ep` |
| [cord_lc](./networking/cord/cord_lc/README.md) | Example of RIOT's Resource Directory (RD) lookup module, called `cord_lc` |
| [cord_epsim](./networking/cord/cord_epsim/README.md) | This example shows how a node can register with a CoRE resource directory |

### GNRC Networking

| Example | Description |
|---------|-------------|
| [gnrc_minimal](./networking/gnrc/minimal/README.md) | This is a minimalistic example for RIOT's gnrc network stack. |
| [gnrc_networking](./networking/gnrc/networking/README.md) | This example demonstrates the usage of the GNRC network stack in RIOT. |
| [gnrc_networking_subnets](./networking/gnrc/networking_subnets/README.md) | This example demonstrates IPv6 subnet auto-configuration for networks on a tree topology. |
| [gnrc_border_router](./networking/gnrc/border_router/README.md) | Example of `gnrc_border_router` using automatic configuration |
| [gnrc_lorawan](./networking/gnrc/lorawan/README.md) | Send and receive LoRaWAN packets and perform basic LoRaWAN commands |

### DTLS

| Example | Description |
|---------|-------------|
| [dtls-sock](./networking/dtls/dtls-sock/README.md) | This example shows how to use DTLS sock `sock_dtls_t` |
| [dtls-echo](./networking/dtls/dtls-echo/README.md) | This example shows how to use TinyDTLS with sock_udp. |
| [dtls-wolfssl](./networking/dtls/dtls-wolfssl/README.md) | This example demonstrates the usage of the DTLS module with the wolfSSL library. |

### Misc

| Example | Description |
|---------|-------------|
| [lorawan](./networking/misc/lorawan/README.md) | This application shows a basic LoRaWAN use-case with RIOT. |
| [openthread](./networking/misc/openthread/README.md) | This example demonstrates the usage of the OpenThread stack in RIOT. |
| [lwm2m](./networking/misc/lwm2m/README.md) | Example of a LWM2M client on RIOT |
| [ccn-lite-relay](./networking/misc/ccn-lite-relay/README.md) | This application demonstrates how to use the Content-Centric Networking stack from [CCN-Lite](http://www.ccn-lite.net/) on RIOT |
| [telnet_server](./networking/misc/telnet_server/README.md) | Simple telnet server that listens on port 23 over IPv6. |
| [posix_sockets](./networking/misc/posix_sockets/README.md) | Showcase for RIOT's POSIX socket support |
| [spectrum-scanner](./networking/misc/spectrum-scanner/README.md) | This example demonstrates how to monitor energy levels on all available wireless channels |
| [sniffer](./networking/misc/sniffer/README.md) | This application is built to run together with the script `./tools/sniffer.py` as a sniffer for (wireless) data traffic. |
| [benckmark_udp](./networking/misc/benchmark_udp/README.md) | This example uses the `benchmark_udp` module to create a stress-test for the RIOT network stack. |
| [sock_tcp_echo](./networking/misc/sock_tcp_echo/README.md) | This is a simple TCP echo server / client that uses the SOCK API. |

## Advanced Examples

| Example | Description |
|---------|-------------|
| [bindist](./advanced/bindist/README.md) | RIOT allows for creating a "binary distribution", which can be used to ship proprietary, compiled objects in a way that makes it possible to re-link them against a freshly compiled RIOT. This application serves as a simple example. |
| [usbus_minimal](./advanced/usbus_minimal/README.md) | This is a minimalistic example for RIOT's USB stack. |
| [suit_update](./advanced/advanced/suit_update/README.md) | This example shows how to integrate SUIT-compliant firmware updates into a RIOT application. |
| [thread_duel](./advanced/thread_duel/README.md) | This is a thread duel application to show RIOTs abilities to run multiple-threads concurrently, even if they are neither cooperative nor dividable into different scheduler priorities, by using the optional round-robin scheduler module. |
| [posix_select](./advanced/posix_select/README.md) | This example is a showcase for RIOT's POSIX select support |
| [psa_crypto](./advanced/psa_crypto/README.md) | Basic functions of the PSA Crypto API |
| [pio_blink](./advanced/pio_blink/README.md) | How to use the PIO peripheral on the RaspberryPi Pico to blink an LED. |
| [twr_aloha](./advanced/twr_aloha/README.md) | This example allows testing different two-way ranging algorithms between two boards supporting a dw1000 device. This makes use of the uwb-core pkg. |
| [senml_saul](./advanced/senml_saul/README.md) | This example demonstrates the usage of the SAUL (Sensor Actuator Uber Layer) module with the SenML (Sensor Measurement Lists) format. |
| [opendsme](./advanced/opendsme/README.md) | This example demonstrates the usage of the OpenDSME module in RIOT. |
| [xipfs](./advanced/xipfs/README.md) | This example demonstrates the usage of XIPFS for creating and executing an executable file. |

## Examples from Guides

[Our guides](https://guide.riot-os.org/) walk you through small tutorials to get started. The following examples are the resulting code from completing their respective guide.

| Example | Description |
|---------|-------------|
| [creating_project](./guides/creating_project/README.md) | Teaches you the very first steps. How to setup a RIOT project, build and run a Hello World in it. [Create Project](https://guide.riot-os.org/c_tutorials/create_project/) tutorial |
| [shell](./guides/shell/README.md) | Teaches you how to use the interactive RIOT shell for your application. [Shell](https://guide.riot-os.org/c_tutorials/shell/) tutorial |
| [gpio](./guides/gpio/README.md) | Teaches you how to configure and use GPIO pins for external hardware interaction. [GPIO](https://guide.riot-os.org/c_tutorials/gpio/) tutorial |
| [saul](./guides/saul/README.md) | Teaches you how to interact with sensors and actuators through the SAUL interface. [SAUL](https://guide.riot-os.org/c_tutorials/saul/) tutorial |
| [threads](./guides/threads/README.md) | Teaches you how to create and manage multiple execution threads in your RIOT application. [Threads](https://guide.riot-os.org/c_tutorials/threads/) tutorial |
| [timers](./guides/timers/README.md) | Teaches you how to use timers for periodic tasks and time measurement in RIOT. [Timers](https://guide.riot-os.org/c_tutorials/timers/) tutorial |
