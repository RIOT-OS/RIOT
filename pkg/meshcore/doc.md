@defgroup pkg_meshcore MeshCore
@ingroup  pkg
@brief    Lightweight, hybrid routing mesh protocol for LoRa radios

# Introduction

[MeshCore](https://github.com/meshcore-dev/MeshCore) is a mesh protocol for
LoRa radios: the nodes have an Ed25519 identity, advertise themselves and
exchange encrypted messages, which are relayed by the repeaters with flood or
direct routing.

The package builds the core of MeshCore (`mesh::Mesh`, `mesh::Dispatcher`,
the packets and the identities), the static packet pool and the tables of the
packets already seen. The applications of MeshCore (repeater, room server,
companion radio) depend on the Arduino libraries and are not built.

# RIOT integration

The abstractions of MeshCore are implemented in `meshcore_riot.h`:

- `meshcore_riot::Radio`: radio based on a LoRa netdev device, so the
  sub-GHz LoRa transceivers supported by RIOT (sx126x and sx127x drivers) can
  be used, provided they support the LoRa parameters of the mesh
- `meshcore_riot::MillisClock` and `meshcore_riot::RTCClock`: clocks based on
  ztimer, the real-time clock starts at a fixed time after a reboot
- `meshcore_riot::RNG`: random number generator, based on `periph_hwrng` when
  it is available, else on the `random` module
- `meshcore_riot::RadioRNG`: random number generator combining the noise of
  the radio with `meshcore_riot::RNG`, to generate the identity. Without
  `periph_hwrng`, the `random` module is seeded with the unique ID of the CPU,
  which is not suited to generate keys.
- `meshcore_riot::Runner`: initializes the radio, then processes the mesh, in
  the calling thread, which receives the interrupts of the radio. The mesh must
  be locked to be used from another thread.
- `meshcore_riot::Node` (`meshcore_node` module): a ready-made node built on
  the classes above, see below

`meshcore_riot.h` must be included before the headers of MeshCore: the CMSIS
headers of some MCUs, as the STM32, define `RNG` as a macro, which conflicts
with the `RNG` class of MeshCore, and `meshcore_riot.h` undefines it. The mesh
and the packet manager can't be global objects: the mesh reads the clock when
it is constructed, and the packet manager allocates its pool, before the
initialization of RIOT.

The default LoRa parameters, `meshcore_riot::default_radio_params`, are the
defaults of MeshCore: 869.618 MHz, 62.5 kHz bandwidth, SF8, coding rate 4/5.
They are set with `CONFIG_MESHCORE_FREQUENCY`, `CONFIG_MESHCORE_BANDWIDTH`,
`CONFIG_MESHCORE_SPREADING_FACTOR`, `CONFIG_MESHCORE_CODING_RATE` and
`CONFIG_MESHCORE_TX_POWER`, to use the preset of another region. The SX126x
transceivers support 62.5 kHz, as the SX1276, but not the LLCC68 and the
SX1272.

The SHA-256, HMAC-SHA-256 and AES-128 primitives used by MeshCore are provided
by the RIOT `hashes` and `crypto` modules, the Ed25519 signatures are verified
with the `monocypher` package. The headers of the package (`SHA256.h`, `AES.h`,
`Ed25519.h`) provide them with the API of the
[Arduino Cryptography Library](https://github.com/rweather/arduinolibs) used by
MeshCore.

# Node

The `meshcore_node` module provides `meshcore_riot::Node`
(`meshcore_riot_node.h`), a ready-made node: it advertises itself, keeps the
contacts from the advertisements it receives, sends and acknowledges text
messages, sends and receives the messages of the Public channel, and can repeat
the packets of the other nodes. `meshcore_riot::Node::start()` creates the node
and the thread processing it, initializes the radio and sends an
advertisement. The other threads lock the node to use it.

On the boards with an EEPROM (`periph_eeprom`), the identity, the name and the
repeater setting are stored at `CONFIG_MESHCORE_NODE_EEPROM_POS`, so the node
keeps its identity across the reboots. On the other boards, a new identity is
generated at each startup.

The events (new contact, text message, Public message, acknowledgment) are
printed on stdio by default, a derived class can override them. The pool of
packets and the number of contacts are set with
`CONFIG_MESHCORE_NODE_PACKET_POOL_SIZE` and `CONFIG_MESHCORE_NODE_MAX_CONTACTS`.

```cpp
#include "meshcore_riot_node.h"

meshcore_riot::Node *node = meshcore_riot::Node::start(netdev);
if (node) {
    node->lock();
    node->sendPublic("hello");
    node->unlock();
}
```

@see https://github.com/meshcore-dev/MeshCore
