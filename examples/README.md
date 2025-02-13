# Examples

RIOT provides a wide range of examples to demonstrate the capabilities of the OS and its modules.

Each example contains a `README.md` that provides information and instructions how to run it.

Here is a quick overview of the examples available in the RIOT:

## The Essentials

| Example | Description |
|---------|-------------|
| [default](./essentials/default/README.md) | This application is a showcase for RIOT's hardware support. Using it for your board, you should be able to interactively use any hardware that is supported. |
| [hello-world](./essentials/hello-world/README.md) | A simple "Hello World" that shows the basic structure of a RIOT application. |
| [blinky](./essentials/blinky/README.md) | The classic "Hello World" example for embedded systems: blinking an LED (or printing "Blink!" via stdio when none are available). |
| [leds_shell](./essentials/leds_shell/README.md) | The application `leds_shell` is a basic example, which allows easy, interactive control of internal board LEDs, and basic GPIO for externally connected simple devices (for e.g. additional LEDs, relay, motors - via dedicated drivers, etc.) via the shell. |
| [saul](./essentials/saul/README.md) | This example demonstrates the usage of the SAUL (Sensor Actuator Uber Layer) module. |
| [timer_periodic_wakeup](./essentials/timer_periodic_wakeup/README.md) | How to set up a periodic wakeup timer using the RIOT operating system. |
| [ipc_pingpong](./essentials/ipc_pingpong/README.md) | This example is to illustrate the usage of RIOTs IPC messaging system. |
| [filesystem](./essentials/filesystem/README.md) | This example showcases ways to interact/manage the filesystem in RIOT. |
| [subfolders](./essentials/subfolders/README.md) | This example demonstrates how to use subfolders in RIOT applications. |

## RIOT Language Bindings

#### Officially Supported/Targeted

##### _Rust_

| Example | Description |
|---------|-------------|
| [rust-hello-world](./language_bindings/officially_supported/rust-hello-world/README.md) | This example demonstrates how to write a simple RIOT application in Rust. |
| [rust-gcoap](./language_bindings/officially_supported/rust-gcoap/README.md) | This example demonstrates how to write a coap server application in Rust using the RIOTs gcoap module. |
| [rust-async](./language_bindings/officially_supported/rust-async/README.md) | This example demonstrates how to use Rusts async/await syntax in a RIOT application. |

##### _C++_

| Example | Description |
|---------|-------------|
| [riot_and_cpp](./language_bindings/officially_supported/riot_and_cpp/README.md) | Example of using C++ in RIOT applications. |

#### Community Supported

| Example | Description |
|---------|-------------|
| [javascript](./language_bindings/community_supported/javascript/README.md) | How to write IoT applications using javascript using JerryScript. |
| [lua_basic](./language_bindings/community_supported/lua_basic/README.md) | How to write IoT applications using Lua. |
| [lua_REPL](./language_bindings/community_supported/lua_REPL/README.md) | This example demonstrates how to use the Lua Read-Eval-Print Loop (REPL) in RIOT. |
| [micropython](./language_bindings/community_supported/micropython/README.md) | How to use the MicroPython port for RIOT. |
| [wasm](./language_bindings/community_supported/wasm/README.md) | How to use WebAssembly in RIOT. |
| [arduino_hello-world](./language_bindings/community_supported/arduino_hello-world/README.md) | This application demonstrates the usage of Arduino sketches in RIOT. |

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
| [nimble_scanner](./networking/bluetooth_low_energy/nimble/nimble_scanner/README.md) | This example showcases the usage of the `NimBLE` BLE stack as a scanner. |
| [nimble_gatt](./networking/bluetooth_low_energy/nimble/nimble_gatt/README.md) | This example application configures and runs the NimBLE BLE stack as simple GATT server. |
| [nimble_heart_rate_sensor](./networking/bluetooth_low_energy/nimble/nimble_heart_rate_sensor/README.md) | This example demonstrates how to implement asynchronous data transfer using GATT notifications by implementing a mock-up BLE heart rate sensor. |

#### Misc BLE Examples

| Example | Description |
|---------|-------------|
| [skald_eddystone](./networking/bluetooth_low_energy/misc_ble_examples/skald_eddystone/README.md) | This example demonstrates the usage of `Skald` for creating an Google `Eddystone` beacon. |
| [skald_ibeacon](./networking/bluetooth_low_energy/misc_ble_examples/skald_ibeacon/README.md) | This example demonstrates the usage of `Skald` for creating an Apple `iBeacon`. |

### MQTT

| Example | Description |
|---------|-------------|
| [asymcute_mqttsn](./networking/mqtt/asymcute_mqttsn/README.md) | This application demonstrates the usage of the Asymcute (MQTT-SN) module in RIOT. |
| [emcute_mqttsn](./networking/mqtt/emcute_mqttsn/README.md) | This application demonstrates the usage of the emCute (MQTT-SN) module in RIOT. |
| [paho-mqtt](./networking/mqtt/paho-mqtt/README.md) | This example demonstrates the usage of the Paho MQTT client library in RIOT. |

### CoRE Resource Directory

| Example | Description |
|---------|-------------|
| [cord_ep](./networking/core_resource_directory/cord_ep/README.md) | Example of RIOT's Resource Directory (RD) endpoint module, called `cord_ep` |
| [cord_lc](./networking/core_resource_directory/cord_lc/README.md) | Example of RIOT's Resource Directory (RD) lookup module, called `cord_lc` |
| [cord_epsim](./networking/core_resource_directory/cord_epsim/README.md) | This example shows how a node can register with a CoRE resource directory |

### GNRC Networking

| Example | Description |
|---------|-------------|
| [gnrc_minimal](./networking/gnrc_networking/gnrc_minimal/README.md) | This is a minimalistic example for RIOT's gnrc network stack. |
| [gnrc_networking](./networking/gnrc_networking/gnrc_networking/README.md) | This example demonstrates the usage of the GNRC network stack in RIOT. |
| [gnrc_networking_subnets](./networking/gnrc_networking/gnrc_networking_subnets/README.md) | This example demonstrates IPv6 subnet auto-configuration for networks on a tree topology. |
| [gnrc_border_router](./networking/gnrc_networking/gnrc_border_router/README.md) | Example of `gnrc_border_router` using automatic configuration |
| [gnrc_lorawan](./networking/gnrc_networking/gnrc_lorawan/README.md) | Send and receive LoRaWAN packets and perform basic LoRaWAN commands |
| [gnrc_networking_mac](./networking/gnrc_networking/gnrc_networking_mac/README.md) | This example shows you how to try out communications between RIOT instances with duty-cycled MAC layer protocols |

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
| [bindist](./advanced_examples/bindist/README.md) | RIOT allows for creating a "binary distribution", which can be used to ship proprietary, compiled objects in a way that makes it possible to re-link them against a freshly compiled RIOT. This application serves as a simple example. |
| [usbus_minimal](./advanced_examples/usbus_minimal/README.md) | This is a minimalistic example for RIOT's USB stack. |
| [suit_update](./advanced_examples/advanced_examples/suit_update/README.md) | This example shows how to integrate SUIT-compliant firmware updates into a RIOT application. |
| [thread_duel](./advanced_examples/thread_duel/README.md) | This is a thread duel application to show RIOTs abilities to run multiple-threads concurrently, even if they are neither cooperative nor dividable into different scheduler priorities, by using the optional round-robin scheduler module. |
| [posix_select](./advanced_examples/posix_select/README.md) | This example is a showcase for RIOT's POSIX select support |
| [psa_crypto](./advanced_examples/psa_crypto/README.md) | Basic functions of the PSA Crypto API |
| [pio_blink](./advanced_examples/pio_blink/README.md) | How to use the PIO peripheral on the RaspberryPi Pico to blink an LED. |
| [twr_aloha](./advanced_examples/twr_aloha/README.md) | This example allows testing different two-way ranging algorithms between two boards supporting a dw1000 device. This makes use of the uwb-core pkg. |
| [senml_saul](./advanced_examples/senml_saul/README.md) | This example demonstrates the usage of the SAUL (Sensor Actuator Uber Layer) module with the SenML (Sensor Measurement Lists) format. |
| [opendsme](./advanced_examples/opendsme/README.md) | This example demonstrates the usage of the OpenDSME module in RIOT. |
