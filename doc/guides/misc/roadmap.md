---
title: Roadmap
description: Overview of priority areas of RIOT development & enhancements
---

The aim of the roadmap is to identify priority areas of RIOT development and enhancements.
For each area, some near-future plans and concrete next steps are indicated.
The text and items below are tentative, up for discussion, to be updated by regular pull requests.


# Network Stack High Layers
(contact/steering: [Martine](https://github.com/miri64))

- ICN stack support clean-up
- discuss mid- and long-term plans for network stack maintenance & development (GNRC vs other supported stacks)
- revisit network time synchronization
- Provide a border router into 6LoWPANs (a 6LBR) that works out of the box in common scenarios:
  - [x] in IPv6 networks with working Prefix Delegation (PD)
  - [ ] in IPv6 networks without working Prefix Delegation (eg. by means of ND Proxying) <!-- 6man-variable-slaac also sounds nice but is at best a step between PD and just-a-single-v6-address -->
  - [ ] in legacy networks (eg. by means of tunneling)
  - [ ] with configurable on-by-default fallbacks

# Network Stack Low Layers
(contact/steering: [Peter](https://github.com/PeterKietzmann))

- Point-to-Point Protocol (PPP): finalize and merge `gnrc_ppp`


# Integrations
(contact/steering: [Teufelchen](https://github.com/teufelchen1))

- [Home-Assistant](https://www.home-assistant.io/) BTHome integration
- [Home-Assistant](https://www.home-assistant.io/) integration via [MQTT Discovery](https://www.home-assistant.io/integrations/mqtt#mqtt-discovery)

# Power Modes
(contact/steering: [benpicco](https://github.com/benpicco))

- concept to fix shell usage issue while LPM activated
- integrate generic power management functions in device driver APIs (netdev, SAUL, ...)
- more advanced LPM concepts:
  - sleeping for short periods (in cases where it is not feasible to switch to the idle thread and back) -> mitigate active waiting



# Peripheral Drivers
(contact/steering: [maribu](https://github.com/maribu))

## Timers

(contact/steering: [kaspar030](https://github.com/kaspar030), [benpicco](https://github.com/benpicco), [maribu](https://github.com/maribu))

- cleanup and unification of low-level timer interfaces (`timer`, `rtt`, `rtc`)
- implement capture mode

## SPI

- introduction of `spi_slave` interface
- transition to `spi_clk_t` being the frequency in Hz, not an `enum` constant, to allow arbitrary frequencies
    - most implementations have been ported
- allow a way to detect the actual frequency an SPI bus is running at
    - see https://github.com/RIOT-OS/RIOT/pull/16727 for one proposal

## I2C

- introduction of `i2c_slave` interface
    - see https://github.com/RIOT-OS/RIOT/issues/19560 for a discussion

## GPIO

(contact/steering: [gschorcht](https://github.com/gschorcht), [maribu](https://github.com/maribu))

- add a port-based, feature-rich, thread-safe GPIO API
    - [GPIO LL](https://doc.riot-os.org/group__drivers__periph__gpio__ll.html) seems to fit the bill so for
    - many MCUs still need an implementation
- add a high level API that unifies external and internal GPIOs on top

## ADC

(contact/steering: [benpicco](https://github.com/benpicco), [kfessel](https://github.com/kfessel), [gschorcht](https://github.com/gschorcht), [maribu](https://github.com/maribu))

- extend / replace the ADC API to allow advanced use cases
    - selecting reference voltages (think: multimeter app)
    - differential inputs
    - fast (ideally DMA supported) sampling (think: a digital oscilloscope app)
    - unify external and internal ADCs
    - see https://github.com/RIOT-OS/RIOT/pull/13247 for one proposal



# Software Updates
(contact/steering: [Emmanuel](https://github.com/emmanuelsearch))

- Modularize to provide toolbox supporting other image storing (e.g. support external flash), transport (other than CoAP), crypto (e.g. secure element).
- riotboot support for architectures other than Cortex-M



# Documentation
(contact/steering: [Emmanuel](https://github.com/emmanuelsearch))

- Write and publish more RDMs



# Low-Level Hardware Support
(contact/steering: [Alex](https://github.com/aabadie))

- radio support for TI SensorTag
- radio support for Silab Thunderboard



# Testing
(contact/steering: [Kaspar](https://github.com/kaspar030))

- automated network functionality tests (e.g. RPL + UDP/PING tests through border router, multi-hop) in IoTLAB dev sites?



# Security
(contact/steering: [Kaspar](https://github.com/kaspar030))

- RNG unified (secure, or basic), seeding
- RIOT default configuration = secure configuration (that's our goal/motto)


## 802.15.4 Link Layer Security
(contact/steering: [chrysn](https://github.com/chrysn))

Current status: RIOT supports application provided keys,
with no guidance on how to (and no practical ways to) use that securely
(see [CVE-2021-41061](https://nvd.nist.gov/vuln/detail/CVE-2021-41061)).

Goal: Usably secure defaults.

- Figure out applicability of [RFC9031](https://www.rfc-editor.org/rfc/rfc9031) ("CoJP") to non-6TiSCH scenarios.
- Implement RFC9031 with any extensions needed for the MACs RIOT has.
- Provide tools to set up a recommended JRC, and to provision keys between it and the device at flash time.
  This may entail extensions to the build process, as CoJP requires per-device secrets.
