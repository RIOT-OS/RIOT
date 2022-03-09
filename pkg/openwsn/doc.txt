/**
 @defgroup pkg_openwsn   OpenWSN network stack
 @ingroup  pkg
 @ingroup  net
 @brief    Provides a RIOT adaption of the OpenWSN network stack
 @see      https://github.com/openwsn-berkeley/openwsn-fw
 @experimental

 # OpenWSN RIOT Port

 This implementation integrates the [OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw)
 full network stack (UDP, IPv6 (6LoWPAN), RPL, 6TiSCH) into RIOT.

 It can be used instead of GNRC on supported 802.15.4 radios, and compared to
 GNRC, provides a full 6TiSCH implementation. It supports RIOT's
 @ref net_sock API, so it can be used as a drop-in replacement for application
 built on top of @ref net_sock.

 This port provides a new RIOT "board" to the OpenWSN software. In this way
 RIOT's hardware abstraction connects to OpenWSN's interfaces.

 The simple scheduling mechanism in OpenWSN is run in a RIOT thread with
 second highest priority after the radio thread (THREAD_PRIORITY_MAIN - 4).

 The current port of OpenWSN currently needs a root node that works along an
 external tool that performs routing and handles join procedure:
 [Openvisualizer](https://github.com/openwsn-berkeley/openvisualizer)


 ## Joining a network

 The first thing a new mote will need to do is to find a network. On boot it
 will actively be listening for enhanced beacons. Once a beacon is received it
 will adjust its timers drift and synchronize with the network. Re-synchronization
 will be happening constantly to compensate for oscillator and timer drifts.

 Once synchronized the node will need to join the network. OpenWSN uses CoJP
 [constrained join protocol](https://datatracker.ietf.org/doc/draft-ietf-6tisch-minimal-security/)
 an the stack itself only handles Join Requests. The JRC (join registrar/coordinator,
 a central entity) is not running on the root node, but alongside it, in the
 `OpenVisualizer` external tool. This functionality is now optional but can be
 included through the `openwsn_cjoin` module.

 Once joined the device has the required keys to start listening to DIS (DODAG
 Information Solicitation) messages and to send DIO (DODAG Information Object)
 requests. Once it knows about the topology of the network it is able to
 send packets.

 OpenWSN uses source routing. This means that unless the recipient of a packet
 is one of the parents in the RPL tree the packet will have to go up the tree
 to the root node. But in OpenWSN RPL implementation the node does not know
 how to route, instead it is `OpenVisualizer` which generates an SRH (Source
 Routing Header), attaches to the incoming packet and sends it down the tree.

 ## Hardware abstraction implementation

 Following, details about the implementation of selected hardware modules.

 ### sctimer

 The `sctimer` ("single compare timer") in OpenWSN is the lowest timer
 abstraction which is used by the higher layer timer module `opentimers`. In
 the end it is responsible for scheduling on the MAC layer. To enable low power
 energy modes, this timer usually uses the RTC (real time clock) or RTT (real
 time timer) module.

 This port has two possible implementations or sctimer, one on top of
 periph_rtt and another on top of ztimer, `sctimer_rtt` and `sctimer_ztimer`
 respectively. If possible `ztimer` should be preferred, being a virtual
 timer it will allow RIOT applications/modules to use the low level RTT timer.
 But ztimer (any virtual timer) has some overhead which can be costly
 depending on the different platforms used, specially when `openserial`
 is required. More on this in openserial and Known Issues.

 #### sctimer_ztimer

 In order to get the most portable code, this implementation uses
 ztimer and defines a new `ztimer_clock` (`ZTIMER_32768`) that operates
 at 32768Khz to have a resolution of ~30usec/tick (same as OpenWSN).

 When available `ZTIMER_32768` will be built on top of `periph_rtt` to
 get low power capabilities. If not it will be built on top of a
 regular timer. In either case it will be shifted up if the base
 frequency is lower than 32768Hz or frac if higher.

 When next interrupt to schedule is already late, current time,
 implementations in OpenWSN directly trigger a hardware interrupt.
 Until able to trigger sw isr directly a callback is set 0 ticks in
 the future, which internally will be set to `now + RTT_MIN_OFFSET`.

 #### sctimer_rtt

 In order to reduce overhead this implementation uses bare RTT. It
 expects a RTT running at 32768Hz to have a resolution of ~30usec/tick
 (same as OpenWSN). If `RTT_FREQUENCY` is lower than 32768Hz then a
 simple time-division mechanism will be used to speed up the clock.
 This only works if `RTT_FREQUENCY` is 32768Hz/2.

 When next interrupt to schedule is already late, current time,
 Implementations in OpenWSN directly trigger a hardware interrupt.
 Until able to trigger sw isr directly a callback is set
 `RTT_MIN_OFFSET` ticks in the future.

 ### radio

 The radio adaptation maps to RIOT's @ref netdev API or on
 @ref drivers_ieee802154_hal API. The later is preferred but not yet
 supported by all boards.

 Hardware MAC layer features such as CSMA/CA, ACK handling and retransmissions
 are handled by OpenWSN, so the radio driver must support disabling AUTOACK
 and CSMA handling by the hardware. Frame filtering must as well be disabled.

 OpenWSN needs to be notified when a frame reception/transmission starts and
 when it ends. Without these synchronization can still work but in a deteriorated
 way.

 OpenWSN expects to recover crc information on every received frame even if it
 will simply drop frames with invalid crc. The stack can function correctly if
 radio drivers automatically drop frames with an invalid crc (i.e. the stack
 doesn't get notified about these frames).

 #### ieee802154_hal

 The radio drivers should support the following caps:

     - `IEEE802154_CAP_IRQ_RX_START`
     - `IEEE802154_CAP_IRQ_TX_START`

 For CRC error handling:

     - `IEEE802154_CAP_IRQ_CRC_ERROR`

 #### netdev

 The radio adaptation preloads the buffer so `NETOPT_PRELOADING` must be
 supported.

 The radio drivers should support the following netdev events:

     - `NETDEV_EVENT_RX_STARTED`
     - `NETDEV_EVENT_TX_STARTED`
     - `NETDEV_EVENT_RX_COMPLETE`
     - `NETDEV_EVENT_TX_COMPLETE`

 ### uart

 In RIOT, the first configured uart device is mapped to STDIO in most cases.
 In OpenWSN however, the `openserial` tool uses uart to feed external software
 running on a host computer such as
 [Openvisualizer](https://github.com/openwsn-berkeley/openvisualizer).
 To enable use of these tools, an uart adaptation is provided.

 This is provided through the `openwsn_serial` (`openserial`) module. It
 will use the next available uart that is not used by STDIO
 (checking STDIO_UART_DEV). When multiple uart are available STDIO and
 `openserial` can be used in parallel. If `stdio_null` is used then `openserial`
 will use `STDIO_UART_DEV`, otherwise it will use the next available uart.
 e.g. If `STDIO_UART_DEV = UART_DEV(1)` `OPENWSN_UART_DEV  = UART_DEV(0)` if
 there are uarts.

 OpenWSN uart abstraction makes use of tx hardware interrupts to execute a
 previously registered callback after every byte is sent out. These interrupts
 are currently not defined in RIOT in a generic way, so instead a timer is set
 to fire shortly after a byte is written.

 It uses `ztimer` to set the timer since it's already pulled in as a dependency.

 ### Openserial

 As was mentioned before any OpenWSN network will require a root node which is
 connected to an `OpenVisualizer` instance running on a host computer.
 Interaction between `OpenVisualizer` and the root-node is done over serial. As
 OpenWSN uses source routing, this means that ultimately all network traffic
 must go from the root node to `OpenVisualizer` and back.

 OpenSerial uses software flow control (XonXoff) to turn off serial activity
 while time critical TSCH operation are ongoing. But software flow control
 can cause issues since delays in the serial pipe (either because of a remote
 connection, buffers, etc..) can lead to bytes being transmitted when one side
 is not yet ready.

 Serial data is transmitted as High-Level Data Link Control (HDLC) frames.
 Since network traffic is tunneled through the serial pipe, in order to
 to have a stable connection these packets must not be lost. Packets can be
 lost in multiple ways but at the end it reduces to bytes being overridden in
 the uart reception buffer.

 1. The last byte in an HDCL frame is received
 2. Serial pipe delays hindering XonXoff operation
 3. Interrupts are masked or higher/equal ISR are running

 1. When the last byte of a frame is received some parsing and handling of the
    frame occur. This takes some time and during that time OpenSerial can't
    handle more incoming bytes. This in practice limits the baudrate depending
    on the CPU's speed.

 2. This can occur when a debugger/virtual-port might act as a
    buffer between the mote and the host, or when there is a latency in the
    connection (for example tcp connection to iotlab).

 3. Since uart reception is interrupted based if the uart ISR is not serviced
    for too long then bytes start overriding each other in the reception
    buffer. In an application where only OpenWSN is running then there are 3
    functions/operations that run in ISR or with disabled ISR.

  (a) `opentimers_timer_callback` (OpenWSN timer abstraction callback)
  (b) `sctimer_setCompare` and `sctimer_readCounter`
  (c) `ztimer_handler`

 (a) and (b) are closely related since `opentimers_timer_callback` will
  itself call `sctimer_setCompare` and `sctimer_readCounter`. And this itself
  will depend on `ztimer_set` and `ztimer_now` or `rtt_get_counter` or
  `rtt_set_alarm` execution time.

 (c) is also related to (a) (b) since `ztimer_handler` will also call
 `opentimers_timer_callback` as well as the underlying `rtt` functions.

 Since a 115200 baudrate means ~1byte every 10us, none of the above can take
 longer than that or bytes could be lost at that baudrate.

 Because of the above mentioned reasons, it is preferable to use `sctimer_rtt`
 for the root node or "border router" on an OpenWSN network, since this reduces
 the likeliness of packets being lost. For non root nodes, OpenSerial only
 provides debugging information so no special care needs to be taken.

 It is also recommended that the root node should act as a border router
 running only the OpenWSN stack to avoid other threads/ISR disrupting serial
 reception.

 ## Tested Platforms and Pin configurations

 So far, this has been successfully tested on `iotlab-m3`,`nucleo-f103` and
 `samr21-xpro`, all based on at86rf23x radios. These radios make use of
 the `radio_netdev` adaptation.

 ### Synchronization

 To join a network a node must first receive EB (enhanced beacons). Once an EB
 is received the node will be synchronized with the network. Synchronization
 times are not deterministic, they depend on the following:

    `SLOTFRAME_LENGTH*SLOTDURATION*P_CHANNEL*P_EB`

 `SLOTFRAME_LENGTH` in OpenWSN is 101, and this port uses 20ms as the slotOffset
 duration. `P_EB` specifies the a probability for a node to transmit an EB.
 By default it's 10%, that means that on average it will take 10 tries before an
 EB is transmitted. `P_CHANNEL` is the probability for the transmitter's and
 receiver channel to match. If channel hopping is disabled this means that the
 average worst case scenario is `101*20ms*10 ~= 20s`, so 20s for synchronization
 to take place. `EB_PORTION` can be changed to increase the likelihood of EB to be
 sent. This can also be achieved by reducing `SLOTFRAME_LENGTH`, but the later can
 have an impact on the MSF (Minimal Scheduling Function). If too few cells are
 available this could increase the likelihood of collisions.

 On the other hand if channel hopping is enabled then the joining node picks a
 random channel to start listening on. The transmitter also picks a random
 channel to start transmitting on and then follows a channel hopping template.
 This would take on average ~8 tries for it to hit the correct channel for the
 first time, and then it would hit it every 16 hops, on average this could lead
 synchronization times of around `320s`.

 If nodes are having trouble in staying synchronized increasing `P_EB` by
 reducing the value of `EB_PORTION` can also be done. Note that `EB_PORTION`
 and `SLOTFRAME_LENGTH` are not configurable by default so need to be overridden
 with `CFLAGS`.

 See [Tune parameters in OpenWSN](https://openwsn.atlassian.net/wiki/spaces/OW/pages/132055073/Tune+parameters+in+OpenWSN)
 for more details.

 ### Timing

 Timing is essential for OpenWSN to work properly. For optimal results most
 parameters in `board_info.h` should be measured for the specific hardware used.
 OpenWSN has done that for most of their boards. These values can not be taken
 directly from OpenWSN since they do not necessarily use the same TIMER's or
 clock speeds.

 For more details on those parameters refer to:
 https://openwsn.atlassian.net/wiki/spaces/OW/pages/688251/State+Machine

 Since all these parameters are HW dependent, it also means that hybrid
 networks (different type of underlying hardware) might desynchronize often,
 or not manage to keep in sync at all.

 Print messages during TSCH operation should be avoided since these can disrupt
 TSCH timings.

 ### ledpins & debugpins

 The OpenWSN software provides different hooks all over the stack to toggle
 different LEDs as well as debug pins to examine state and scheduling of a node.
 Default configuration files are provided for both. The LED configuration maps to
 RIOTs `LEDX_PIN` definitions, if available. On Nucleo boards LED0 line is
 shared SPI, so is not used.

 The default configuration can be overwritten by setting `OPENWSN_LEDS_DEFAULT`
 in the form of `leds_config_t`. The debugpins work similarly by setting
 `OPENWSN_DEBUGPINS_BOARD` in the form of `debugpins_config_t`.

 The default configuration maps to OpenWSN reference hardware `openmote-b`.

 ## Optional Modules and Features

 The following modules are optional and can be disabled if not needed, or to
 lower the stack footprint.

  - `openwsn_cjoin`: this enabled the use of Constrained Join Protocol (CoJP)
  - `openwsn_6lo_fragmentation`: this enable 6LoWPAN fragmentation
  - `openwsn_iee802154e_security`: enable link layer security
  - `openwsn_adaptive_msf`: allow the MSF algorithm to dynamically remove and
    allocate slots

 ## Testing and debugging

 List of some items which are helpful to explore the functionality of OpenWSN:

 - LED pins and debug pins as mentioned above in combination with a logic analyzer.
 The expected behavior is described in:
 [OpenWSN wiki](https://openwsn.atlassian.net/wiki/spaces/OW/pages/688257/Schedules).

 - The provided test application provides a UDP client and server. If the UDP
 server is able to receive packets, the mechanism is considered to work correctly.
 You should also be able to ping the device from your host. See
 `tests/pkg_openwsn/README.md` for more details.

 - To speed up synchronization and make sniffing easier you can disable channel
 hopping by setting (`CFLAGS=-DIEEE802154E_SINGLE_CHANNEL=26`).

 - To sniff the packets either use a 802.15.4 capable board and follow at:
 https://github.com/RIOT-OS/applications/blob/master/sniffer/tools/README.md.
 Alternatively use a Raspberry Pi with an external radio such as Openlabs and
 incorporate Linux WPAN tools. In addition to that, there's also other
 hardware such as the ATUSB IEEE 802.15.4 USB Adapter which can directly be
 used on your Linux computer with WPAN tools installed. If you conduct your
 experiments on the IoT-LAB testbed you might want to use a:
 [sniffer profile](https://www.iot-lab.info/tutorials/radio-sniffer).

 - To explore the channel hopping mechanism there are rather expensive
 multi-channel sniffers such as the BeamLogic 802.15.4 Site Analyzer that can
 sniff all channels simultaneously. Alternatively you can set up multiple
 separate sniffer devices locally or make use of the `sniffer_aggregator` on
 the IoT-LAB testbed.

 - To test Openserial on a given platform the target `make openv-serial` can
 be used on a BOARD flashed with `tests/pkg_openwsn` (`USEMODULE=openwsn_serial`)
 must be included as well. The following output should appear:

    ```
    Test Setup:
    ------------------
    Iterations:    100
    Packet length: 100
    Echo timeout:    2

    Test Progress:

      [####################################]  100%

    Test Statistics:
    ------------------
    Pkts send:      100
    Echo success:   100
    Echo timeout:     0
    Echo corrupted:   0
    ```

 The test should be considered passing if success rate is > 98%.

 ## Known Issues

 The following errors might be visible when using `openwsn_serial`:

 - `[OPENSERIAL] wrong CRC in input Buffer`

 Since a timer is set to simulate a uart transmit interrupt, it can happen that
 the interrupt is missed if another interrupt occurs during that time, this
 seems to lead to the input buffer missing a byte and so CRC fails. More details
 where given in the `openserial` section.

 - `[IEEE802154E] wdDataDuration overflows while at state 19 in slotOffset 0`

 This error can show up when the radio starts receiving (receives the SFD) and
 therefore triggers a `NETDEV_RX_STARTED` but then no `NETDEV_TX_STARTED`
 event follows. This happens when packets with invalid CRC are received.
 netdev currently silently drops these packets without notifying upper layers.
 But this does not affect the stack operation, so they can be ignored.

 - `[IEEE802154E] large timeCorr.: -18 ticks (code loc. 0)`

 Most crystals used to clock the RTT will drift even those with a very similarly
 drift (10-30ppm). It's is normal then for motes adjust their timerCorr as long
 as it stays within the above mentioned margins and if motes are able to stay
 synchronized over the long run. If there aren't then maybe board_info.h
 parameters require tuning for the specific platform.

- `[JRC:ERROR] Type-error in conversion of 5N=ex`

This errors happen when a node tries to rejoin the network. This error is only
associated to a log print, so can be ignored.

- `[coap:WARNING] coapRcBadRequest(reason=OSCOAP unprotect failed: oscoapError(reason=Replay protection failed))`

The join procedure uses a replay window. If a node had already joined the network
and for some reason attempts to rejoin again, then the replay windows will need
to expire for it's join request to be accepted.

The following errors are platform specific.

 - samr21-xpro issues:
      - The serial debugger hinders Openserial operation, an ftdi device must
        be used.

 - sam0 issues:
      - sam0 requires 180us busy loops every time an alarm is set or the
        counter is read. Because of the later only `sctimer_rtt` can be used
        and the max. tested baudrate for openserial is of 19200 bauds.

 - iotlab-m3 issues:
      - openserial does not work reliably over 57600 bauds or when using
        sctimer_ztimer.

Other errors:

- missed characters over stdio

TSCH state machine disable occurs in IRQ context and disables IRQ during time
critical sections. This can cause bytes sent over stdio to be missed.

 ## Todos

 - `sctimer` to trigger an ISR immediately using software interrupts.
 - The UART wrapper uses ztimer to fake an interrupt after one byte
   has been sent. This should also be done with software interrupts.

 ##  Future Steps

 It would be desirable to achieve an extraction of the MAC layer.

 */
