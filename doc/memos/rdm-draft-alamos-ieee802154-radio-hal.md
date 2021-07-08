- RDM: rdm-draft-alamos-ieee802154-radio-hal
- Title: The IEEE802.15.4 radio HAL
- Authors: José Álamos
- Status: draft
- Type: Design
- Created: March 2020

# 1. Abstract

This memo describes a Hardware Abstraction Layer (HAL) for radios compliant
with the IEEE802.15.4 standard. The work follows a technology-specific approach
to provide well defined hardware access that allows to implement agnostic
IEEE802.15.4 PHY and MAC layers on top. Additionally, the new HAL enables
integration of network stacks that require direct access to the radio.

# 2. Status

This document is currently under open discussions. The content of this document
is licensed with a Creative Commons CC-BY-SA license.

## 2.1 Terminology
This memo uses the [RFC2119](https://www.ietf.org/rfc/rfc2119.txt) terminology
and the following acronyms and definitions:

## 2.2 Acronyms
- RDM: RIOT Developer Memo
- PIB: Physical Information Base.
- MIB: MAC Information Base.

## 2.3 Definitions
- SubMAC: Lower layer of the IEEE802.15.4 MAC that provides the
  retransmissions with CSMA-CA logic, address filtering and CRC validation.
- Standalone CCA: Single run of the Clear Channel Assessment procedure.
- Continuous CCA: Clear Channel Assessment procedure followed by transmission
  (required by the CSMA-CA algorithm)
- Caps: Short word for capabilities. In this context, capabilities are the
        the features (hardware acceleration) present in a radio device.
- Ops: Short word for operations. In this context, operations are a set of
       instructions to control the radio device.

# 3. Introduction
This document is a product of the
[Uniform Network Stack Integration](https://github.com/RIOT-OS/RIOT/issues/13771)
and aims to describe the architecture of a Hardware Abstraction Layer for
IEEE802.15.4 compliant radios.

The IEEE802.15.4 Radio HAL abstracts common functionalities of
IEEE802.15.4 compliant radios such as loading packets, transmitting,
configuring PHY parameters, etc. This abstraction is required for upper layers
that require hardware-independent access to drive IEEE802.15.4 radio devices
(802.15.4 MAC, network stacks, test applications, etc).

In the current RIOT lower network stack architecture, all network interfaces
are driven by the `netdev` interface. The work presented in this document
addresses deficits of using `netdev` as a Hardware Abstraction Layer:

- `netdev` is too generic to be used as a HAL to cover the wide range of
  different technologies in RIOT (IEEE802.15.4, BLE, Ethernet, WiFi,
  Proprietary devices, ...). The semantics of a standarized radio are technology
  specific and in most cases well defined. In the case of IEEE802.15.4 devices,
  they are defined by the IEEE.
- `netdev` includes PHY and MAC components that are not in the scope of a
  hardware abstraction layer. The `netdev` interface is implemented as a device
  driver but it additionally includes technology-dependent components for every
  single device. For the case of IEEE802.15.4, this includes components of the
  802.15.4 MAC/PHY such as transmission of Physical Service Data Units (PSDU),
  or retransmissions with CSMA-CA and ACK handling. As a consequence,
  code is duplicated, feature sets of similar devices heavily depend on the
  specific implementation, and integration of new devices is more complex than
  need be. Furthermore, duplication and unspecified device access complicate
  code maintenance.
- `netdev` hardcodes MAC layer functionalities, which is likely the consequence
  of hardware MAC acceleration on certain devices. These capabilities are
  currently only available if the hardware provides integrated support. An
  indication mechanism which MAC features are provided within a `netdev`
  implementation is missing. A full MAC layer that is situated on top of the HAL
  requires a defined access to specific radio functionalities in order to meet
  timing constraints or energy requirements. That means, varying properties
  between implementations and partly implemented MAC features within the device
  driver interfere with the concept of transparent hardware access by one MAC
  layer implementation.


Other components of the 802.15.4 MAC are present in the GNRC Netif
implementation for the 802.15.4 Link Layer (`gnrc_netif_ieee802154`). These
components prepare and parse 802.15.4 frames in order to send and receive data.
However, mandatory 802.15.4 MAC features are missing (commissioning, security,
channel scanning, etc). One major drawback of this approach is the fact that
802.15.4 MAC components of `gnrc_netif_ieee802154` are GNRC specific and
cannot be reused in other network stacks that require a 802.15.4 MAC.

As a solution, the lower layer should be separated into three main components:
1. 802.15.4 Radio HAL: hardware-agnostic interface to drive radio devices
   (proposed in this RDM).
2. 802.15.4 MAC: full link layer including PHY definition.
3. Network Stack interface (netif): controls the 802.15.4 MAC layer to send
   and receive packets. It provides transparent and technology-independent
   access to the medium.

The 802.15.4 MAC and netif are not part of this document, but they are affected
by this work, thus, they are mentioned to give an outlook for upcoming efforts
on the lower network stack.

The following picture compares the current RIOT lower network stack
architecture (left) with the approach described in this document (right). As
can be seen, the new approach adds IEEE802.15.4 specific APIs and layers
between the lower layer network stack interface (GNRC Netif) and the hardware
dependent device driver. In contrast, the `netdev` based solution misses a
specific Radio HAL which prevents to run a hardware-agnostic MAC on top.


```
         OLD             |                        NEW             
         ===             |                        ===             
                         |                                     
+---------------------+  |  +---------------------+   +---------------------+
|                     |  |  |                     |   |                     |
|  GNRC Network Stack |  |  |  GNRC Network Stack |   |                     |
|                     |  |  |                     |   |                     |
+---------------------+  |  +---------------------+   |                     |
          ^              |            ^               |                     |
          |              |            |               |                     |
     gnrc_netapi         |       gnrc_netapi          | OpenThread, OpenWSN |
          |              |            |               |                     |
          v              |            v               |                     |
+---------------------+  |  +---------------------+   |                     |
|                     |  |  |                     |   |                     |
|     GNRC Netif      |  |  |     GNRC Netif      |   |                     |
|                     |  |  |                     |   |                     |
+---------------------+  |  +---------------------+   +---------------------+
          ^              |            ^                         ^
          |              |            |                         |
   gnrc_netif_ops_t      |     gnrc_netif_ops_t                 |
          |              |            |                         |
          v              |            v                         |
+---------------------+  |  +---------------------+             |              
|                     |  |  |                     |             |              
|gnrc_netif_ieee802154|  |  |gnrc_netif_ieee802154|             |              
|                     |  |  |                     |             |              
+---------------------+  |  +---------------------+             |              
          ^              |            ^                         |              
          |              |            |                         |              
          |              |     802.15.4 MAC API           Radio HAL API
          |              |            |                         |              
          |              |            v                         |              
          |              |  +---------------------+             |              
          |              |  |                     |             |              
    netdev_driver_t      |  |    802.15.4 MAC     |             |              
          |              |  |                     |             |              
          |              |  +---------------------+             |              
          |              |            ^                         |              
          |              |            |                         |              
          |              |      Radio HAL API   ----------------+
          |              |            |         |                             
          v              |            v         v                             
+---------------------+  |  +---------------------+-------------------------+           
|          |          |  |  |                     |                         |           
|  netdev  |  Device  |  |  |  802.15.4 Radio HAL |                         |
|          |  Driver  |  |  |                     |       Device Driver     |
|----------+          |  |  +---------------------+                         |
|                     |  |  |                                               |           
+---------------------+  |  +-----------------------------------------------+           
```


# 4. Architecture
```
+-----------------------------------------------------------------------------+
|                                                                             |
|                               Upper layer                                   |
|                                                                             |
+-----------------------------------------------------------------------------+
      |         ^
      |         |
      |         |
      |         |
 Radio Ops  Event Notification                   +----------------------------+
      |         |                  IRQ Handler   |                            |
      |         |         +----------------------|    Bottom-Half processor   |
      |         |         |                      |                            |
      |         |         |                      +----------------------------+
      |         |         |                                   ^
      |         |         |                                   |
      v         |         v                                  IRQ
+-----------------------------+                               |
|  802.15.4 Radio HAL         |               HW independent  |
|-----------------------------|-------------------------------|----------------
|                             |               HW dependent    |
|                             |                               |
|         Device Driver       |                               |
|                             |-------------------------------+
|                             |
+-----------------------------+
```

As shown in the above figure, the IEEE802.15.4 Radio HAL is a central component
that provides any upper layer a technology-dependent and unified access to the
device driver, by implementing the Radio HAL API.

The HAL uses an Event Notification mechanism to inform the upper layer about
radio events (`TX_DONE`, `RX_DONE`, `CCA_DONE`, etc). This mechanism can either
run in interrupt context or thread context, if the device is not able to
resolve events during ISR (e.g SPI devices). For the latter, the radio HAL
requires an upper layer to take over the Bottom-Half processing which means,
offloading the ISR to thread context.

## 4.1 Upper Layer
Upper layers are users that requires direct access to the primitive operations
of a radio and its hardware acceleration features, if available.

Examples for Upper Layers:
- A MAC layer can use the Radio HAL to implement parts of a PHY layer (data
  communication, set/get parameters, perform CCA, etc.) .
- A network stack that requires low level access to the radio (OpenWSN,
  OpenThread) can use the Radio HAL to implement the integration code.
- A developer who implements a simple application to send and receive data
  between 802.15.4 radios (relying on hardware accelerated MAC features, if
available).

The upper layer accesses the radio using the Radio HAL API. Events that are
triggered by the device (packet received, transmission finished, PLL locked)
are indicated by the event notification mechanism, described below.

## 4.2 Bottom-Half Processor
The Bottom-Half (BH) processor is a component to offload the IRQ processing to
thread context. The component is required for radios that cannot resolve radio
events during ISR (SPI devices).

The component registers an IRQ handler during initialization
which is executed when the device triggers and interrupt. This handler uses
internal mechanisms to call the Radio API IRQ handler from a safe context.

The BH processor can be implemented dependent or independent of the network
stack. A network stack independent solution is preferred in order to reuse
functionality between different network stacks.

The term "Bottom Half" was originally introduced by the Linux kernel. See
[Top and Bottom Halves](http://www.makelinux.net/ldd3/chp-10-sect-4.shtml)

## 4.3 Radio HAL

The Radio HAL is defined by the Radio HAL API which consists of three main
components: Radio Operations, Event Notification, and the Device Specific
IEEE802.15.4 HAL implementation.

The Radio HAL Implementation provides a set of functionalities to control the
operation of the device, to process the IRQ handler and to receive event
notifications from the device.

### 4.3.1 Radio Operations
The Radio Operations (`radio_ops`) interface exposes operations that are common
to control 802.15.4 devices and to request their hardware capabilities
information (i.e., MAC acceleration hardware)

The interface defines a collection of mandatory functions:
- Set the transceiver state
- Set the PHY configuration (channel, tx power, etc)
- Load and transmit a frame
- Get device capabilities

The interface provides a collection of optional functions that may or may not
be implemented, dependent on the hardware acceleration features of a device.
These functions include:
- Read the number of retransmissions
- Set address filter addresses (extended, short, PAN ID)
- Set CSMA-CA backoff parameters.

All `radio_ops` functions are non-blocking and some of them follow a
Request-Confirm scheme which means, an event is generated when the request
finished.

The full list of functions can be found in the Interface Definition section.

### 4.3.2 Event Notification

The Radio HAL provides an Event Notification mechanism to inform the upper
layer about an event (a packet was received, a transmission finished, etc).

The upper layer can subscribe to these events to perform different actions. As
an example, a MAC layer would subscribe to the RX done event to allocate the
received packet. The TX done event is commonly used to release resources or
update statistics.

As described before, the Event Notification mechanism can be called during ISR
or thread context (BH processor). Thus, this must be taken into consideration
for the implementation of the Event Notification callback (e.g the `RX_DONE`
event might post an event to an event queue in order to fetch the packet or
`PLL_LOCK` might unlock a mutex).

The full list of events and implications are defined in the Interface
Definition section.

### 4.3.3 Device Driver

The Device Driver implements the hardware-dependent part of the IEEE802.15.4
Radio HAL by wrapping the `radio_ops` interface around the device specific
code, which grants access to all device operations.

The Device Driver additionally provides a mechanism to expose the ISR of
radios that require the Bottom-Half processor.

The function set of the Device Driver can include device specific features that
are not exposed by the Radio HAL API (e.g., Smart Listening with AT86RF2xx
radios). 

# 5 Implementation Details
## 5.1 Initialization of device drivers
In order to implement the 802.15.4 abstraction on top of a device driver, it
is required an initialization procedure that performs the following tasks:

1. Set up IRQ callback
2. Reset the device
3. Confirm connectivity and perform self tests
4. Bring device into a low power state
5. Set up IRQs and disable them to use less power.

The `radio_ops` interface provides an "on" function that turns on the device
and enables interrupts. It is expected that the upper layer will call this
function to enable the radio device, if the initialization procedure succeeded.

## 5.2 Abstract State Machine

The Radio HAL defines an Abstract State Machine. In order to ensure a uniform
behavior in all devices, all Radio HAL device drivers should be implemented
against this.

```
                                          +---------+
                                          |         |
                                          |   Off   |
                                          |         |
                                          +---------+
                                            |     ^
                                     on()   |     | 
                                    +-------+     |
                                    |             |
                                    v             |
                              +---------+         |
                              |         |         |
                              | WU_BUSY |         |
                              |  *[2]   |         |
                              +---------+         |
                                   |              |
                                   +--------+     |
                                            |     |
                               RF_AWAKE_END |     | off() *[1]
                                            v     |
                                          +---------+
                                          |         |
                                          | TRX_OFF |
                                          |         |
                                          +---------+
                                            |     ^
                        set_trx_state(IDLE) |     | RF_PLL_UNLOCK
                                            v     |
          +---------+                     +---------+
          |         |                     |         |
          | CCA_BUSY|                     | PLL_BUSY|
          |         |                     |         |
          +---------+                     +---------+
            |      ^                        |     ^
RF_CCA_DONE/|      |            RF_PLL_LOCK |     | set_trx_state(TRX_OFF)
RF_CCA_FAIL |      |                        |     |
            |      |                        |     |
            | cca()|                        v     |
            |      +----------------------+---------+  transmit()   +---------+
            +---------------------------->|         |-------------->|         |
                                          |   IDLE  |               | TX_BUSY |
                +------------------------>|         |<--------------|         |
                |       *[2]              +---------+  RF_TX_DONE /   +---------+
                |                           ^    |     RF_CCA_FAIL
                |                           |    |
      RF_RX_DONE|       set_trx_state(IDLE) |    | set_trx_state(RX_ON)
                |                           |    |
                |                           |    |
                |                           |    v
         +---------+                     +---------+
         |         |                     |         |
         | RX_BUSY |<--------------------|  RX_ON  |
         |         |      SFD detected   |         |
         +---------+                     +---------+

*[1]: The `off` function can be called from any state.
*[2]: If the device has a neglectable wake up time, the `on` function
      return 0 and the `RF_AWAKE_END` is not generated.
```

The behavior is undefined if functions are called from the wrong state (for
example, calling `transmit()` from `RX_ON` or `TX_BUSY`).

A specification for each state is described in the following sub sections.

### 5.2.1 Off

If radio initialization succeeds, the Abstract State Machine begins in `Off`
state. During this state the device consumes the least power and all hardware
components (transceiver, crypto acceleration) are disabled.

Calling the `on` function request a state change to `TRX_OFF` state. The
transition finishes when the `on` function returns 0 or the Event Notification
reports `RF_AWAKE_END`.

### 5.2.2 TRX OFF

During this state the device is on but the transceiver is off (PLL not locked).
The power consumption is higher than the `Off` state but the device is able
to operate the transceiver and other hardware components (e.g crypto
accelerator).

Setting the transceiver state to IDLE (`set_trx_state`) leaves the `TRX_OFF`
state. The transition finishes when the Event Notification reports
`RF_PLL_LOCK`.

### 5.2.3 Idle

This state is device specific and represents a state where the device is ready
to transmit, fetch a received frame or perform Stand-Alone CCA.

Assuming a frame is already loaded in the framebuffer, calling `transmit()` will
set the state to `TX_BUSY` and begin the transmission of a frame. The Abstract
State Machine will go back to Idle state on either `RF_TX_DONE` event or
`RF_CCA_FAIL`, if using Auto-CCA and the channel was busy.

A Stand-Alone CCA can be requested with `cca()`. The Abstract State Machine
will change state to `CCA_BUSY` and go back to Idle state on `RF_CCA_IDLE` or
`RF_CCA_FAIL` events.

Finally, setting the transceiver state to `TRX_OFF` or `RX_ON` will leave the
Idle state (this will generate an `PLL_UNLOCK` event in the former).

### 5.2.4 RX ON

During RX ON state the radio is able to detect Start Frame Delimiter (SFD) and
thus, receive frames.

The state is left when an SFD is detected or the transceiver state is set to
Idle. The former will set the Abstract State Machine state to the transient
state RX BUSY and then to Idle on `RF_RX_DONE` event. The latter will change
the state to Idle only if the radio is currently not receiving a frame.

## 5.3 Prepare and Transmit

The Radio HAL doesn't define an explicit send function. Unlike the `netdev`
approach, it bases on separation of the send procedure into frame loading
and triggering the transmissions start.

Although it is possible to load and start using the `netdev` interface with the
`NETOPT_PRELOADING` option, the Radio HAL approach is easier and more
lightweight to implement since it doesn't require internal state variables.

Separated load and start is required for MAC layers
with timing constraints (E.g. TSCH mode of 802.15.4 MAC). 

In the rare case a radio device doesn't support loading the frame buffer without
triggering a transmission, it is still possible to implement the load and
transmit pattern using an internal buffer. However, this case is very unlikely
because such a device could not meet 802.15.4 timing requirements.

It is expected that a higher layer "send" function is defined for convenience
which handles both loading and frame sending. Typically, this would be a
802.15.4 MAC implementation which preloads the devices buffer once accessible,
and triggers a send operation at a scheduled time slot. Alternatively, this
could be a helper function for non MAC users.

## 5.4 TX and RX Information

Sometimes upper layers require information associated to the transmission
or reception of a packet. The TSCH mode of the 802.15.4 MAC may require
LQI and RSSI data from a received packet to schedule new cells.
The 802.15.4 MAC may also require the information associated to the
frame retransmission component (frame pending bit, number of retransmission,
status) if the hardware provides support for hardware retransmissions.

The 802.15.4 Radio HAL API provides functions to retrieve this data.
Note that retrieving this information is optional in cases where the RX
information is not needed or when the device doesn't support frame
retransmissions.

## 5.5 Thread Safety

The 802.15.4 Radio HAL is not thread safe. As a consecuence of this, it is
required that the Bottom-Half processor and all function calls to the Radio HAL
API run in the same thread context. This is the same approach of the `netdev`
interface.

As a future work this work can be adapted to be thread safe.

# 6 802.15.4 Radio HAL Interface definition

## 6.1 Radio Operations
The Radio Ops interface is implemented using function pointers. 

These functions should be implemented with device specific validations only.
Parameters that are not device specific (valid channel settings, address
lengths, etc) should be checked by higher layers in order to avoids redundancy.

Note that the Radio Ops interface only implements a few get functions. The
reason behind this is the fact most members of the PHY and MAC Information
Base (such as address, TX power, channel number) are already stored in RAM.

### Send/Receive
```c
    /**
     * @brief Load packet in the framebuffer of a radio.
     *
     * This function shouldn't do any checks, so the packet MUST be valid.
     * If the radio is still transmitting, it should block until it is safe to
     * write to the frame buffer again.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] pkt the packet to be sent with valid length
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*prepare)(ieee802154_dev_t *dev, iolist_t *pkt);

    /**
     * @brief Transmit a preloaded packet.
     *
     * @pre the PHY state is @ref IEEE802154_TRX_STATE_TX_ON and the packet
     *      is already in the framebuffer.
     *
     * @post the PHY state is @ref IEEE802154_TRX_STATE_TX_ON.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] mode transmissions mode
     *
     * @return 0 on success
     * @return -ENOTSUP if a transmission mode is not supported
     * @return -EBUSY if medium is busy (if sending with CCA)
     * @return negative errno on error
     */
    int (*transmit)(ieee802154_dev_t *dev, ieee802154_tx_mode_t mode);

    /**
     * @brief Get the length of the received packet.
     *
     * @pre the radio already received a packet (e.g
     *      @ref ieee802154_dev_t::cb with @ref IEEE802154_RADIO_RX_DONE).
     * @pre the device is on
     *
     * @post the frame buffer is still protected against new packet arrivals.
     *      
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*len)(ieee802154_dev_t *dev);

    /**
     * @brief Read a packet from the internal framebuffer of the radio.
     *
     * This function should try to write the received packet into @p buf and
     * put the radio in a state where it can receive more packets.
     *
     * @pre the radio already received a packet (e.g
     *      @ref ieee802154_dev_t::cb with @ref IEEE802154_RADIO_RX_DONE).
     * @pre the device is on
     *
     * @post the PHY state is @ref IEEE802154_TRX_STATE_RX_ON and the radio is
     *       in a state where it can receive more packets, regardless of the
     *       return value.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] buf buffer to write the received packet into. If NULL, the
     *             packet is not copied.
     * @param[in] size size of @p buf
     * @param[in] info information of the received packet (LQI, RSSI). Can be
     *            NULL if this information is not needed.
     *
     * @return number of bytes written in @p buffer (0 if @p buf == NULL)
     * @return -ENOBUFS if the packet doesn't fit in @p
     */
    int (*read)(ieee802154_dev_t *dev, void *buf, size_t size,
                ieee802154_rx_info_t *info);
```

### PHY Operations
```c
    /**
     * @brief Perform Standalone Clear Channel Assessment
     *
     * This function performs blocking CCA to check if the channel is clear.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @return true if channel is clear.
     * @return false if channel is busy.
     */
    bool (*cca)(ieee802154_dev_t *dev);

    /**
     * @brief Set the threshold for the Energy Detection (first mode of CCA)
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] threshold the threshold in dBm.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_cca_threshold)(ieee802154_dev_t *dev, int8_t threshold);

    /**
     * @brief Set CCA mode
     *
     * All radios MUST at least implement the first CCA mode (ED Threshold).
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] mode the CCA mode
     *
     * @return 0 on success
     * @return -ENOTSUP if the mode is not supported
     * @return negative errno on error
     */
    int (*set_cca_mode)(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode);

    /**
     * @brief Set IEEE802.15.4 PHY configuration (channel, TX power)
     *
     * This function SHOULD NOT validate the PHY configuration unless
     * it is specific to the device. The upper layer is responsible of all kind
     * of validations.
     *
     * @pre the device is on
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] conf the PHY configuration
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*config_phy)(ieee802154_dev_t *dev, ieee802154_phy_conf_t *conf);

    /**
     * @brief Set the transceiver PHY state
     *
     * @pre the device is on
     *
     * @note the implementation MUST block until the state change occurs.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] state the new state
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_trx_state)(ieee802154_dev_t *dev, ieee802154_trx_state_t state);

```

### Device State Management
```c
    /**
     * @brief Process radio IRQ.
     *
     * This function calls the @ref ieee802154_cb_t::cb function with
     * the corresponding event type.
     *
     * @note if the device is off, this function should do nothing
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     */
    void (*irq_handler)(ieee802154_dev_t *dev);

    /**
     * @brief Turn on the device
     *
     * @pre the init function of the radio succeeded.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @post the transceiver state ins @ref IEEE802154_TRX_STATE_TRX_OFF
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*on)(ieee802154_dev_t *dev);

    /**
     * @brief Turn off the device
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     *
     * @post the transceiver state is @ref IEEE802154_TRX_STATE_TRX_OFF
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*off)(ieee802154_dev_t *dev);
```

### Caps and Optional Functions
```c
    /**
     * @brief Get a cap from the radio
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param cap cap to be checked
     *
     * @return true if the radio supports the cap
     * @return false otherwise
     */
    bool (*get_cap)(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap);

    /**
     * @brief Set IEEE802.15.4 promiscuous mode
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       hardware address filtering.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] enable whether the promiscuous mode should be enabled or not.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_promiscuous)(ieee802154_dev_t *dev, bool enable);

    /**
     * @brief Get the SubMAC TX information (number of retransmissions,
     *        frame pending bit, status, etc).
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       frame retransmissions
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[out] info the TX information
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*get_tx_status)(ieee802154_dev_t *dev, ieee802154_tx_info_t *info);

    /**
     * @brief Set IEEE802.15.4 address in hardware address filter
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       hardware address filtering.
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] short_addr the IEEE802.15.4 short address
     * @param[in] ext_addr the IEEE802.15.4 extended address
     * @param[in] pan_id the IEEE802.15.4 PAN ID
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_hw_addr_filter)(ieee802154_dev_t *dev, uint8_t *short_addr,
                              uint8_t *ext_addr, uint16_t pan_id);

    /**
     * @brief Set number of frame retransmissions
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       frame retransmissions
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] retries the number of retransmissions
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_frame_retries)(ieee802154_dev_t *dev, int retries);

    /**
     * @brief Set IEEE802.15.4 CSMA configuration parameters
     *
     * @pre the device is on
     *
     * @note this function pointer can be NULL if the device doesn't support
     *       frame retransmissions
     *
     * @param[in] dev IEEE802.15.4 device descriptor
     * @param[in] bd parameters of the exponential backoff
     * @param[in] retries number of CSMA-CA retries. If @p retries < 0, 
     *                    retransmissions with CSMA-CA MUST be disabled.
     *
     * @return 0 on success
     * @return negative errno on error
     */
    int (*set_csma_params)(ieee802154_dev_t *dev, ieee802154_csma_be_t *bd,
                           int8_t retries);
```

## 6.2 Event Notification

The Event Notification mechanism is implemented with a function callback.
The callback function is supposed to be implemented by the upper layer.

The callback signature, the events and their expected behavior are defined
in the following block:

```c
/**
 * @brief   Event notification status
 */
typedef enum {
    /**
     * @brief the transceiver detected a valid Start Frame Delimiter, which
              indicates the radio started to receive a packet.
     */
    IEEE802154_RADIO_RX_START,

    /**
     * @brief the transceiver received a packet and there is a packet in the
     *        internal framebuffer.
     *
     * The transceiver is in @ref IEEE802154_TRX_STATE_RX_ON state when
     * this function is called, but with framebuffer protection (either
     * dynamic framebuffer protection or disabled RX). Thus, the packet
     * will not be overwritten before calling the @ref ieee802154_radio_read
     * function. However, @ref ieee802154_radio_read MUST be called in
     * order to receive new packets. If there is no interest in the
     * packet, the function can be called with a NULL buffer to drop
     * the packet.
     */
    IEEE802154_RADIO_RX_DONE,

    /**
     * @brief the transceiver finished sending a packet or the
     *        retransmission procedure.
     *
     * If the radio supports frame retransmissions the
     * @ref ieee802154_radio_get_tx_status MAY be called to retrieve useful
     * information (number of retries, frame pending bit, etc). The
     * transceiver is in @ref IEEE802154_TRX_STATE_TX_ON state when this function
     * is called.
     */
    IEEE802154_RADIO_TX_DONE,
} ieee802154_trx_status_t;

/**
 * @brief Prototype of the IEEE802.15.4 device event callback
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] status the event status
 */
typedef void (*ieee802154_cb_t)(ieee802154_dev_t *dev,
                                ieee802154_trx_status_t status);
```

Other MAC specific events such as TX done with frame pending, CSMA-CA medium
busy or exceeded number of retransmissions are not explicitly reported because
they can be extracted after the TX done event using the Radio HAL API.

The Radio HAL is designed to be able to call the Radio Ops interface from the
event notification callback.

All Radio HAL implementation MUST implement at least the `RX_START`, `RX_DONE`
and `TX_DONE` events. All these event might be disabled via compile time
configurations if the upper layer doesn't require them.

Other events such as `ACK_TIMEOUT` in radios that support ACK timers
and `CSMA_BACKOFF_TIMEOUT` in radios that support CSMA-CA backoff timers are
out of the scope of this document. However, these might be added and implemented
at any time if required by upper layers.

## 6.3 Radio Capabilities
The following list defines the basic capabilities available in common
IEEE802.15.4 compliant radios. Other capabilities such as support for CSMA-CA
backoff and ACK timers are not included in the list because they are out of
the scope of this document.

```c
typedef enum {
    /**
     * @brief the device supports hardware address filter
     */
    IEEE802154_CAP_HW_ADDR_FILTER,
    /**
     * @brief the device support frame retransmissions with CSMA-CA
     */
    IEEE802154_CAP_FRAME_RETRIES,
    /**
     * @brief the device support ACK timeout interrupt
     */
    IEEE802154_CAP_ACK_TIMEOUT,
    /**
     * @brief the device supports Auto ACK
     */
    IEEE802154_CAP_AUTO_ACK,
    /**
     * @brief the device performs CCA when sending
     */
    IEEE802154_CAP_24_GHZ,
    /**
     * @brief the device support the IEEE802.15.4 Sub GHz band
     */
    IEEE802154_CAP_SUB_GHZ,
} ieee802154_rf_caps_t;
```

A 802.15.4 HAL implementation MUST indicate all capabilities supported by the
device and driver implementation.

# 7 Future Proof Considerations

The Radio HAL is designed to be agnostic to different versions of the
IEEE802.15.4 standard.  A single radio device typically implements hardware
acceleration for only one standard, whereas different standards are not always
compatible. As an example, IEEE802.15.4--2006 devices do not support Enhanced
Acknowledgement packets which are required by the TSCH layer of
IEEE802.15.4--2012.  For compatibility, a software MAC can provide such
functionality. The Radio HAL adapts considerations to enable different versions
of the IEEE802.15.4 MAC on top of the abstraction layer.

### Transmission Modes

The Radio HAL interface defines three transmission modes to allow sending
frames using (i) CSMA-CA, (ii) CCA, or (iii) directly without any protection.
In that way, a MAC layer can easily send data frames benefiting from hardware
accelerated CSMA-CA or Beacons that have to meet timing constraints and thus,
require a direct send function.

A HAL implementation can provide several transmit modes, but it MUST implement
at least one. It is recommended that the implementation provides modes that
exploit the internal devices capabilities. Implementing a direct mode is
desired for software MAC layers on top.

### PHY Definition

PHY definitions are specific to a IEEE802.15.4 version. As an example, older
standards define PHY channels with a `channel number`. In modern standards,
channels are represented using a (`channel number`, `channel page`, `channel
modulation`) tuple. The `config_phy` function is called with a pointer to a
`ieee802154_phy_conf_t` structure which describes the PHY configuration. In
order to support newer versions, this structure can be extended without braking
the Radio HAL.

### Future Radio Operations

The Radio Operations interface `radio_ops` can be extended to support
functionalities of newer standards. As an example, most SubGHz radios support a
Listen Before Talk feature that can be implemented as a new and optional
operation
.
# 8 Acknowledgements

Thanks to Peter Kietzmann, Leandro Lanzieri and Martine Lenders for their
reviews, comments and suggestions.

# 9 References

- [Uniform Network Stack Integration](https://github.com/RIOT-OS/RIOT/issues/13771)
- [Top and Bottom Halves](http://www.makelinux.net/ldd3/chp-10-sect-4.shtml)

# 10 Revision

- Rev0: initial document

# 11 Contact

The author of this memo can be contacted via email at jose.alamos@haw-hamburg.de
