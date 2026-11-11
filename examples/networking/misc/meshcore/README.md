# MeshCore

This example runs a [MeshCore](https://github.com/meshcore-dev/MeshCore) node
on a LoRa transceiver supported by RIOT. The node sends advertisements, learns
its contacts from the advertisements of the other nodes, exchanges text
messages with them and on the Public channel of MeshCore, and can repeat the
packets of the mesh.

MeshCore uses the radio through netdev, so the example works with the sub-GHz
LoRa transceivers of RIOT:

- `sx1261`, `sx1262`, `sx1268`, `llcc68` and `sx126x_stm32wl` (radio of the
  STM32WL), from the sx126x driver
- `sx1272` and `sx1276`, from the sx127x driver

The radio uses the default parameters of MeshCore: 869.618 MHz, 62.5 kHz of
bandwidth, spreading factor 8 and coding rate 4/5. The LLCC68 and the SX1272
don't support the 62.5 kHz bandwidth, so they can only join a mesh using a
wider bandwidth.

With these parameters, the node can communicate with the MeshCore devices using
the EU/UK (Narrow) preset, directly or through the repeaters of the mesh: this
preset uses the coding rate 4/8, which the receivers read from the LoRa header.
On a public mesh, each startup without a stored identity floods the
advertisement of a new identity to all the nodes, so avoid restarting such a
node often.

## Build and flash

By default the application is built for the B-L072Z-LRWAN1 board, which has an
SX1276:

    make BOARD=b-l072z-lrwan1 flash term

Use the `LORA_DRIVER` variable to select another transceiver, for example:

    make BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl flash term
    make BOARD=nucleo-l476rg LORA_DRIVER=sx1262 flash term

The transmission power is 14 dBm by default, it is set in dBm with
`CONFIG_MESHCORE_TX_POWER`:

    CFLAGS=-DCONFIG_MESHCORE_TX_POWER=10 make BOARD=b-l072z-lrwan1 flash term

The other parameters are set with `CONFIG_MESHCORE_FREQUENCY` (in Hz),
`CONFIG_MESHCORE_BANDWIDTH`, `CONFIG_MESHCORE_SPREADING_FACTOR` and
`CONFIG_MESHCORE_CODING_RATE`, to join a mesh using the preset of another
region. For example, a bandwidth of 250 kHz with a spreading factor of 10:

    CFLAGS="-DCONFIG_MESHCORE_BANDWIDTH=LORA_BW_250_KHZ \
        -DCONFIG_MESHCORE_SPREADING_FACTOR=LORA_SF10" make BOARD=b-l072z-lrwan1 flash term

All the nodes of a mesh must use the same parameters. Check that the frequency
and the transmission power comply with the regulations of your region before
transmitting.

The node has a pool of 8 packets and up to 16 contacts by default, set
`CONFIG_MESHCORE_NODE_PACKET_POOL_SIZE` and
`CONFIG_MESHCORE_NODE_MAX_CONTACTS` for a larger mesh, on a board with more
RAM:

    CFLAGS="-DCONFIG_MESHCORE_NODE_PACKET_POOL_SIZE=16 \
        -DCONFIG_MESHCORE_NODE_MAX_CONTACTS=32" make BOARD=nucleo-wl55jc \
        LORA_DRIVER=sx126x_stm32wl flash term

## Usage

At the first startup, the node generates its identity from the noise of the
radio, combined with the hardware random number generator of the board if any.
It prints its identity and sends an advertisement. Its name ends with the
beginning of its public key:

    MeshCore node
    Name: RIOT-5e2d, node
    Public key: 5e2d...
    Time: 1767225600
    Radio: 869618000 Hz, 62500 Hz bandwidth, SF8, CR 4/5, 14 dBm

The `mc` shell command controls the node:

| Command                  | Description                                     |
|--------------------------|-------------------------------------------------|
| `mc info`                | prints the identity, the time and the radio     |
| `mc name <name>`         | sets the name sent in the advertisements        |
| `mc time <seconds>`      | sets the time, in seconds since the epoch       |
| `mc repeat <on\|off>`    | enables or disables the repetition of packets   |
| `mc log <on\|off>`       | enables or disables the printing of the packets |
| `mc advert`              | sends an advertisement                          |
| `mc contacts`            | lists the nodes heard                           |
| `mc msg <name> <text>`   | sends a text message to a contact               |
| `mc public <text>`       | sends a text message on the Public channel      |

For example, with two nodes. On the first node:

    > mc name alice
    > mc advert

On the second node, which receives the advertisement:

    New contact: alice
    > mc name bob
    > mc advert
    > mc msg alice "hello from bob"

The first node receives the message, and acknowledges it:

    New contact: bob
    Message from bob: hello from bob

And the second node prints:

    Message to alice acknowledged

The text of a message must be quoted when it contains spaces.

The messages of the Public channel start with the name of the sender. On the
first node:

    > mc public "hello everyone"

The other nodes print:

    Public: alice: hello everyone

With `mc log on`, the node prints all the packets it receives and sends, which
shows whether a mesh is in range, for example:

    RX ADVERT, flood, 1 hop(s), 109 bytes, SNR 9 dB, RSSI -71 dBm

As the companions of MeshCore, the node doesn't repeat the packets by default.
After `mc repeat on`, it relays the packets of the other nodes, so two nodes
out of range can communicate through it, and it is advertised as a repeater.

## Limitations

- The identity, the name and the repeater setting are stored only on the
  boards with an EEPROM (STM32L0 and STM32L1, like the B-L072Z-LRWAN1), at the
  beginning of the EEPROM, and loaded at startup ("Settings loaded from the
  EEPROM"). On the other boards, the identity is generated at each startup:
  after a reboot, the node is a new contact for the other nodes. The contacts
  are never stored.
- The time starts at 2026-01-01 at each startup. The MeshCore applications
  display the timestamps of the advertisements and of the messages, so set the
  time with `mc time` (the output of `date +%s` on a computer) before sending
  them.
- The messages are flooded: the paths to the contacts aren't stored, and the
  messages which aren't acknowledged aren't sent again.
- Only the text messages between contacts and on the Public channel are
  supported: there is no other channel, room server, or companion protocol for
  the MeshCore applications.
