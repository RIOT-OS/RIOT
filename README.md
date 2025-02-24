[![Nightly CI status master][master-ci-badge]][master-ci-link]
[![GitHub release][release-badge]][release-link]
[![License][license-badge]][license-link]
[![API docs][api-badge]][api-link]
[![Wiki][wiki-badge]][wiki-link]
[![Stack Overflow questions][stackoverflow-badge]][stackoverflow-link]
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

The friendly Operating System for IoT!

RIOT is an open-source microcontroller operating system, designed to match
the requirements of Internet of Things (IoT) devices and other embedded devices.
It supports a range of devices that are typically found in the Internet of Things (IoT):
8-bit, 16-bit and 32-bit microcontrollers.

RIOT is based on the following design principles: energy-efficiency, real-time
capabilities, small memory footprint, modularity, and uniform API access,
independent of the underlying hardware (this API offers partial POSIX
compliance).

RIOT is developed by an international open source community which is
independent of specific vendors (e.g. similarly to the Linux community).
RIOT is licensed with LGPLv2.1, a copyleft license which fosters
indirect business models around the free open-source software platform
provided by RIOT, e.g. it is possible to link closed-source code with the
LGPL code.

## Features

RIOT provides features including, but not limited to:

* a preemptive, tickless scheduler with priorities
* flexible memory management
* high resolution, long-term timers
* MTD abstraction layer
* File System integration
* support 200+ boards based on AVR, MSP430, ESP8266, ESP32, RISC-V,
  ARM7 and ARM Cortex-M
* the native port allows to run RIOT as-is on Linux and BSD.
  Multiple instances of RIOT running on a single machine can also be
  interconnected via a simple virtual Ethernet bridge or via a simulated
  IEEE 802.15.4 network (ZEP)
* IPv6
* 6LoWPAN (RFC4944, RFC6282, and RFC6775)
* UDP
* RPL (storing mode, P2P mode)
* CoAP
* OTA updates via SUIT
* MQTT
* USB (device mode)
* Display / Touchscreen support
* CCN-Lite
* LoRaWAN
* UWB
* Bluetooth (BLE) via [NimBLE](https://github.com/apache/mynewt-nimble)

## Getting RIOT

The most convenient way to get RIOT is to clone it via Git

```console
$ git clone https://github.com/RIOT-OS/RIOT
```

this will ensure that you get all the newest features and bug fixes with the
caveat of an ever changing work environment.

If you prefer things more stable, you can download the source code of one of our
quarter annual releases [via Github][releases] as ZIP file or tarball. You can
also checkout a release in a cloned Git repository using

```console
$ git pull --tags
$ git checkout <YYYY.MM>
```

For more details on our release cycle, check our [documentation][release cycle].

[releases]: https://github.com/RIOT-OS/RIOT/releases
[release cycle]: https://doc.riot-os.org/release-cycle.html

## Getting Started
* You want to start the RIOT? Just follow our
[quickstart guide](https://doc.riot-os.org/index.html#the-quickest-start) or
try this
[tutorial](https://github.com/RIOT-OS/Tutorials/blob/master/README.md).
For specific toolchain installation, follow instructions in the
[getting started](https://doc.riot-os.org/getting-started.html) page.
* The RIOT API itself can be built from the code using doxygen. The latest
  version of the documentation is uploaded daily to
  [doc.riot-os.org](https://doc.riot-os.org).

Using Windows? Use [this guide][dev-setup-windows] to
[setup the development environment][dev-setup-windows].

[dev-setup-windows]: doc/guides/setup-windows

## Community

You can ask for help on the Forum or on Matrix. Please send bug reports and
feature requests to our [GitHub issue tracker](https://github.com/RIOT-OS/RIOT/issues)

- [forum](https://forum.riot-os.org) is the default place to start asking for
help. Our Forum provides an archive of prior questions and answers.
- For chat, we use
  [#riot-os:matrix.org](https://matrix.to/#/#riot-os:matrix.org) on the
  [Matrix](https://matrix.org/) chat network.
- [GitHub Issue tracker](https://github.com/RIOT-OS/RIOT/issues) for issues
with the code and documentation.

### Governance

For how our community is structured and governed, please see our [governance document].

[governance document]: GOVERNANCE.md

### How to Ask

Please include as much detail as you can that is relevant to your question, not
only "this isn't working". These details include:

1. What you want to achieve.
2. What have you tried so far (for example the commands you typed).
3. What happened so far.

### Collaboration

RIOT closely collaborates with and inspires other open source projects, e.g., [Ariel OS](https://ariel-os.org/), an IoT OS written in Rust. We are more than happy to collaborate with those who share our [vision](https://doc.riot-os.org/vision.html).

## Contribute

To contribute something to RIOT, please refer to our
[contributing document](CONTRIBUTING.md).

## Mailing Lists
* RIOT commits: [commits@riot-os.org](https://lists.riot-os.org/mailman/listinfo/commits)
* Github notifications: [notifications@riot-os.org](https://lists.riot-os.org/mailman/listinfo/notifications)

## License
* Most of the code developed by the RIOT community is licensed under the GNU
  Lesser General Public License (LGPL) version 2.1 as published by the Free
  Software Foundation.
* Some external sources, especially files developed by SICS are published under
  a separate license.

All code files contain licensing information.

For more information, see the RIOT website:

https://www.riot-os.org


[api-badge]: https://img.shields.io/badge/docs-API-informational.svg
[api-link]: https://doc.riot-os.org/
[license-badge]: https://img.shields.io/github/license/RIOT-OS/RIOT
[license-link]: https://github.com/RIOT-OS/RIOT/blob/master/LICENSE
[master-ci-badge]: https://ci.riot-os.org/job/branch/master/badge
[master-ci-link]: https://ci.riot-os.org/details/branch/master
[matrix-badge]: https://img.shields.io/badge/chat-Matrix-brightgreen.svg
[matrix-link]: https://matrix.to/#/#riot-os:matrix.org
[merge-chance-link]: https://merge-chance.info/target?repo=RIOT-OS/RIOT
[release-badge]: https://img.shields.io/github/release/RIOT-OS/RIOT.svg
[release-link]: https://github.com/RIOT-OS/RIOT/releases/latest
[stackoverflow-badge]: https://img.shields.io/badge/stackoverflow-%5Briot--os%5D-yellow
[stackoverflow-link]: https://stackoverflow.com/questions/tagged/riot-os
[mastodon-badge]: https://img.shields.io/badge/social-Mastodon-informational.svg
[mastodon-link]: https://fosstodon.org/@RIOT_OS
[wiki-badge]: https://img.shields.io/badge/docs-Wiki-informational.svg
[wiki-link]: https://github.com/RIOT-OS/RIOT/wiki
