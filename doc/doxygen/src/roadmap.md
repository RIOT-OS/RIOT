# Roadmap     {#roadmap}

The aim of the roadmap is to identify priority areas of RIOT development & enhancements.
For each area, some near-future plans and concrete next steps are indicated.
The text and items below are tentative, up for discussion, to be updated by regular pull requests.


# Network Stack High layers 
(contact/steering: [Martine](https://github.com/miri64))

1. ICN stack support clean-up
2. discuss mid- and long-term plans for network stack maintenance & development (GNRC vs other supported stacks)
3. Support for CoAP blockwise transfer, observe. Also see [Ken's](https://github.com/kb2ma) unofficial [CoAP for RIOT](https://github.com/kb2ma/RIOT/wiki/CoAP-Home) page.
4. revisit network time synchronization 



# Network Stack Low layers 
(contact/steering: [Peter](https://github.com/PeterKietzmann))

1. Towards fully open source support of 6LoWPAN over BLE
2. Re-integrate and clean-up 802.15.4 TSCH integration
3. Retransmissions by MAC: finalize and merge `netdev_retrans`
4. Point-to-Point Protocol (PPP): finalize and merge `gnrc_ppp`



# Power Modes 
(contact/steering: [Hauke](https://github.com/haukepetersen))

1. ~~RFC from \@gebart for arbitrary freq for xtimer~~ done
2. ~~prototype LPM concept from Kaspar, demoed on PLACE-HOLDER hardware. Basic idea is to define unified/simplified layered LP modes that apply to 99% of IoT hardware. More optimized board-specific LPM would~~ prototype done
3. implement PM interface for existing platforms (-> https://github.com/RIOT-OS/RIOT/issues/6802)
4. xtimer use of RTT low-power timer
5. concept to fix shell usage issue while LPM activated
6. integrate generic power management functions in device driver APIs (netdev, SAUL, ...)
7. more advanced LPM concepts:
  - potentially get rid of idle thread
  - sleeping for short periods (in cases where it is not feasible to switch to the idle thread and back) -> mitigate active waiting



# Peripheral drivers
(contact/steering: [Hauke](https://github.com/haukepetersen))

1. remodeling  of the `periph/i2c.h` interface and subsequent adaption/rewrite of all existing implementations (-> https://github.com/RIOT-OS/RIOT/issues/6577)
2. cleanup and unification of low-level timer interfaces (`timer`, `rtt`, `rtc`)
3. introduction of `spi_slave` interface
4. introduction of `i2c_slave` interface



# Software Updates 
(contact/steering: [Emmanuel](https://github.com/emmanuelsearch))

1. ~~Basic bootloader, flashing and booting one of multiple firmware slots~~ done, see [riotboot](https://github.com/RIOT-OS/RIOT/tree/master/bootloaders/riotboot)
2. ~~Secure firmware update over CoAP compliant with SUIT draft spec [draft-ietf-suit-manifest-03](https://tools.ietf.org/html/draft-ietf-suit-manifest-03)~~ done, see [SUIT update example](https://github.com/RIOT-OS/RIOT/tree/master/examples/suit_update)
3. Update SUIT spec support to comply with [draft-ietf-suit-manifest-04](https://tools.ietf.org/html/draft-ietf-suit-manifest-04)
4. Modularize to provide toolbox supporting other image storing (e.g. support external flash), transport (other than CoAP), crypto (e.g. secure element).
5. riotboot support for architectures other than Cortex-M



# Documentation 
(contact/steering: [Emmanuel](https://github.com/emmanuelsearch))

1. ~~publish roadmap~~ done
2. ~~Introduce RIOT Developer Memos~~ done, see [RDM0](https://github.com/RIOT-OS/RIOT/tree/master/doc/memos)
3. Write and publish more RDMs
4. revamp RIOT website (see [Web Revamp Task Force](https://github.com/RIOT-OS/RIOT/wiki/Website-Revamp-Task-Force))



# Low-level Hardware Support 
(contact/steering: [Alex](https://github.com/aabadie))

1. Improved MIPS support
2. radio support for TI SensorTag
3. radio support for Silab Thunderboard
4. ESP32 support



# Testing
(contact/steering: [Kaspar](https://github.com/kaspar030))

1. ~~automated unit tests with hardware in the loop (SAMR21 plugged on CI server?)~~ (done, Murdock and Philipp)
2. automated network functionality tests (e.g. RPL + UDP/PING tests through border router, multi-hop) in IoTLAB dev sites?
3. ~~leverage PiFleet more?~~ done, 1. uses PiFleet
4. On-board CI testing in IoT-LAB (as it will provide soon the possibility to add custom nodes)



# Security
(contact/steering: [Kaspar](https://github.com/kaspar030))

1. RNG unified (secure, or basic), seeding
2. easy TinyDTLS integration in sock, with CoAP etc.
4. RIOT default configuration = secure configuration (that's our goal/motto)
5. 802.15.4 link layer security (gaps in RFCs? How to update keys?)
