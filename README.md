[![Nightly CI status master][ci-badge]][ci-link]
[![GitHub release][release-badge]][release-link]
[![License][license-badge]][license-link]
[![API docs][api-badge]][api-link]
[![Getting Started Guide][guide-badge]][guide-link]
[![Mastodon][mastodon-badge]][mastodon-link]
[![Matrix][matrix-badge]][matrix-link]

<p align="center"><img src="doc/doxygen/src/riot-logo.svg" width="66%"><!--
                          ZZZZZZ
                        ZZZZZZZZZZZZ
                      ZZZZZZZZZZZZZZZZ
                     ZZZZZZZ     ZZZZZZ
                    ZZZZZZ        ZZZZZ
                    ZZZZZ          ZZZZ
                    ZZZZ           ZZZZZ
                    ZZZZ           ZZZZ
                    ZZZZ          ZZZZZ
                    ZZZZ        ZZZZZZ
                    ZZZZ     ZZZZZZZZ       777        7777       7777777777
              ZZ    ZZZZ   ZZZZZZZZ         777      77777777    77777777777
          ZZZZZZZ   ZZZZ  ZZZZZZZ           777     7777  7777       777
        ZZZZZZZZZ   ZZZZ    Z               777     777    777       777
       ZZZZZZ       ZZZZ                    777     777    777       777
      ZZZZZ         ZZZZ                    777     777    777       777
     ZZZZZ          ZZZZZ    ZZZZ           777     777    777       777
     ZZZZ           ZZZZZ    ZZZZZ          777     777    777       777
     ZZZZ           ZZZZZ     ZZZZZ         777     777    777       777
     ZZZZ           ZZZZ       ZZZZZ        777     777    777       777
     ZZZZZ         ZZZZZ        ZZZZZ       777     777    777       777
      ZZZZZZ     ZZZZZZ          ZZZZZ      777     7777777777       777
       ZZZZZZZZZZZZZZZ            ZZZZ      777      77777777        777
         ZZZZZZZZZZZ               Z
            ZZZZZ                                                           --></p>

# RIOT OS: The friendly Operating System for IoT!

RIOT is an **open-source microcontroller operating system**, designed to match
the requirements of Internet of Things (IoT) devices and other embedded devices.
It supports a range of devices that are typically found in the Internet of Things (IoT):
*8-bit*, *16-bit* and *32-bit* microcontrollers.

RIOT is based on the following design principles: **energy-efficiency, real-time
capabilities, small memory footprint, modularity, and uniform API access,**
independent of the underlying hardware (this API offers partial POSIX
compliance).

RIOT is developed by an international open source community which is
independent of specific vendors (e.g. similarly to the Linux community).
RIOT is licensed with **LGPLv2.1**, a copyleft license which fosters
indirect business models around the free open-source software platform
provided by RIOT, e.g. it is possible to link closed-source code with the
LGPL code.

[ci-badge]: https://ci.riot-os.org/job/branch/master/badge
[ci-link]: https://ci.riot-os.org/details/branch/master
[release-badge]: https://img.shields.io/github/release/RIOT-OS/RIOT.svg
[release-link]: https://github.com/RIOT-OS/RIOT/releases/latest
[license-badge]: https://img.shields.io/github/license/RIOT-OS/RIOT
[license-link]: https://github.com/RIOT-OS/RIOT/blob/master/LICENSE
[api-badge]: https://img.shields.io/badge/docs-API-informational.svg
[api-link]: https://doc.riot-os.org/
[guide-badge]: https://img.shields.io/badge/docs-Getting_Started_Guide-informational.svg
[guide-link]: https://guide.riot-os.org/getting-started/installing/
[mastodon-badge]: https://img.shields.io/badge/social-Mastodon-informational.svg
[mastodon-link]: https://floss.social/@RIOT_OS
[matrix-badge]: https://img.shields.io/badge/chat-Matrix-brightgreen.svg
[matrix-link]: https://matrix.to/#/#riot-os:matrix.org

## Getting Started

- **Start here:**
  Our comprehensive [Getting Started Guide][getting-started]
  walks you through installation and your first RIOT applications.
- **Learn by doing:**
  - Try our [hands-on tutorials][tutorials] designed for beginners
  - Take the [RIOT online course][riot-course] at your own pace
- **Explore the API:** Check out the [API documentation][api-docs]
  for detailed information on RIOT's features and functions.

[getting-started]: https://guide.riot-os.org/getting-started/installing/
[tutorials]: https://github.com/RIOT-OS/Tutorials
[riot-course]: https://github.com/RIOT-OS/riot-course
[api-docs]: https://doc.riot-os.org/

## Features

RIOT provides features including, but not limited to:
- Platforms Supported
  - 200+ boards based on AVR, MSP430, ESP8266, ESP32, RISC-V, ARM7, and ARM Cortex-M
  - The native port allows to run RIOT as-is on Linux and BSD.
    Multiple instances of RIOT running on a single machine can also be
    interconnected via a simple virtual Ethernet bridge or via a simulated
    IEEE 802.15.4 network (ZEP).
- Programming Environment
  - Write native code in C, C++, or Rust
  - Rely upon POSIX APIs, user friendly hardware abstractions and standard embedded Rust interfaces
  - Integrate dynamic runtimes such as MicroPython, elm (JavaScript) and WASM
- Network Features and Protocols
  - IPv6
  - IPv4 via lwIP
  - 6LoWPAN (RFC4944, RFC6282, RFC6775 and RFC7668)
  - UDP
  - TCP
  - RPL (storing mode, P2P mode)
  - CoAP
  - MQTT via paho-mqtt and MQTT-SN
  - LwM2M
  - Bluetooth (BLE) via [NimBLE][nimble]
  - LoRaWAN
  - UWB
  - CAN
  - CNN-Lite
  - LwIP
  - IEEE 802.15.4
  - Ethernet (also via USB)
  - SLIP (or ethos networking via Serial)
  - Dose (single pair ethernet like networking on serial interface UART)
- System Features
  - a preemptive, tickless scheduler with priorities, optional periodic round robin within priorities
  - flexible memory management
  - high resolution, long-term timers
  - MTD abstraction layer for memory devices
  - File System integration
- Security Features
  - OTA updates via SUIT
  - PSA Cryptographic API
  - DTLS, EDHOC
- Device Drivers
  - SPI, I2C, UART, USB, CAN
  - RTC, PWM, Timer
  - ADC, DAC
  - Various sensors: Environmental sensors, IMUs, magnetometers, particulate matter sensors
  - Various actuators: Servos, motors, dimmers, switches, RGB LEDs
  - Displays and touchscreens
  - GNSS

[nimble]: https://github.com/apache/mynewt-nimble

## Community

You can ask for help on the Forum or on Matrix. Please send bug reports and
feature requests to our [GitHub issue tracker][issues]

- The [Forum][forum] is the default place to start asking for
  help. Our Forum provides an archive of prior questions and answers.
- For chat, we use
  [#riot-os:matrix.org][matrix-chat] on the
  [Matrix][matrix] chat network.

### Governance

For how our community is structured and governed,
please see our [governance document][governance].

### How to Ask

Please include as much detail as you can that is relevant to your question, not
only "this isn't working". These details include:

1. What you want to achieve.
2. What have you tried so far (for example the commands you typed).
3. What happened so far.

### Collaboration

RIOT closely collaborates with and inspires other open source projects,
e.g., [Ariel OS][ariel], an IoT OS written in Rust.
We are more than happy to collaborate with those who share our
[vision][vision].

[issues]: https://github.com/RIOT-OS/RIOT/issues
[forum]: https://forum.riot-os.org
[matrix-chat]: https://matrix.to/#/#riot-os:matrix.org
[matrix]: https://matrix.org/
[governance]: https://guide.riot-os.org/general/governance/
[ariel]: https://ariel-os.org/
[vision]: https://guide.riot-os.org/general/vision/

## Getting RIOT

The most convenient way to get RIOT is to clone it via Git

```console
$ git clone https://github.com/RIOT-OS/RIOT
```

this will ensure that you get all the newest features and bug fixes with the
caveat of an ever changing work environment.

If you prefer things more stable, you can download the source code of one of our
quarter annual releases [via Github][releases]
as ZIP file or tarball. You can also checkout a release in
a cloned Git repository using

```console
$ git pull --tags
$ git checkout <YYYY.MM>
```

For more details on our release cycle, check our
[documentation][release-cycle].

[releases]: https://github.com/RIOT-OS/RIOT/releases
[release-cycle]: https://guide.riot-os.org/misc/release_cycle/

## Contribute

To contribute something to RIOT, please refer to our
[contributing document][contributing].

[contributing]: CONTRIBUTING.md

## Mailing Lists

- Github notifications:
[notifications@riot-os.org][notifications]

[notifications]: https://lists.riot-os.org/postorius/lists/notifications.lists.riot-os.org/

## License

- Most of the code developed by the RIOT community is licensed under the GNU
  Lesser General Public License (LGPL) version 2.1 as published by the Free
  Software Foundation.
- Some external sources, especially files developed by SICS are published under
  a separate license.

All code files contain licensing information.

For more information, see the [RIOT website][riot-website].

[riot-website]: https://www.riot-os.org
